#include <ranges>
#include <utility>
#include <OpenImageIO/imageio.h>

#include "ImageReader.h"

struct ReaderChannelInfo
{
    ChannelName name;
    std::string rawChannelName;
    int rawChannelIndex;
};

struct ReaderLayerInfo
{
    LayerName name;
    int subimageIndex;
    std::vector<ReaderChannelInfo> channels;

    ReaderChannelInfo *channelInfo(const ChannelName &name)
    {
        auto it = std::ranges::find(
            channels,
            name,
            &ReaderChannelInfo::name);

        if (it == channels.end())
            return nullptr;

        return &*it;
    }
    std::vector<ChannelName> channelNames() const
    {
        std::vector<ChannelName> names;
        names.reserve(channels.size());
        for (const auto &channel : channels)
        {
            names.push_back(channel.name);
        }
        return names;
    }
    bool channelsContiguous() const
    {
        int prev = -1;
        for (const auto &channel : channels)
        {
            if (prev != -1 &&
                channel.rawChannelIndex != prev + 1)
            {
                return false;
            }
            prev = channel.rawChannelIndex;
        }
        return true;
    }
};

struct ParsedChannelName
{
    LayerName layer;
    ChannelName channel;

    ParsedChannelName(
        const std::string &rawChannelName,
        const std::string &subimageName,
        bool multipart)
    {
        const auto prefixPos = rawChannelName.rfind('.');
        const bool hasPrefix = prefixPos != std::string::npos;

        if (multipart && !subimageName.empty())
        {
            layer = subimageName;
            channel = hasPrefix ? rawChannelName.substr(prefixPos + 1)
                                : rawChannelName;
            return;
        }

        if (hasPrefix)
        {
            layer = rawChannelName.substr(0, prefixPos);
            channel = rawChannelName.substr(prefixPos + 1);
            return;
        }

        if (rawChannelName == "R" ||
            rawChannelName == "G" ||
            rawChannelName == "B" ||
            rawChannelName == "A")
        {
            layer = "rgba";
            channel = rawChannelName;
            return;
        }

        if (rawChannelName == "Z")
        {
            layer = "depth";
            channel = rawChannelName;
            return;
        }

        layer = "other";
        channel = rawChannelName;
    }
};

struct ImageReader::Impl
{
    std::unique_ptr<OIIO::ImageInput> input;
    std::string error;
    std::vector<std::string> warnings;
    std::vector<ReaderLayerInfo> layers;
    int width = 0, height = 0;

    const ReaderLayerInfo *layerInfo(const LayerName &name) const
    {
        const auto it = std::ranges::find(
            layers,
            name,
            &ReaderLayerInfo::name);

        if (it == layers.end())
            return nullptr;

        return &*it;
    }
    ReaderLayerInfo *layerInfo(const LayerName &name)
    {
        return const_cast<ReaderLayerInfo *>(
            std::as_const(*this).layerInfo(name));
    }
    std::vector<LayerName> layerNames() const
    {
        std::vector<LayerName> names;
        names.reserve(layers.size());
        for (const auto &layer : layers)
        {
            names.push_back(layer.name);
        }
        return names;
    }
    std::vector<ChannelName> channelNames(const LayerName &layerName) const
    {
        const auto it = std::ranges::find(layers, layerName, &ReaderLayerInfo::name);

        if (it == layers.end())
            return {};

        return it->channelNames();
    }

    void addChannel(
        const ParsedChannelName &parsed,
        const int subimageIndex,
        const std::string &rawChannelName,
        const int rawChannelIndex)
    {
        const auto &layerName = parsed.layer;
        const auto &channelName = parsed.channel;

        auto *layer = layerInfo(layerName);

        if (layer == nullptr)
        {
            layers.emplace_back(
                layerName,
                subimageIndex,
                std::vector<ReaderChannelInfo>{});

            layer = &layers.back();
        }
        else if (subimageIndex != layer->subimageIndex)
        {
            warnings.push_back(
                "Layer subimage conflict: '" +
                layerName +
                "' is already defined by subimage " +
                std::to_string(layer->subimageIndex) +
                ", cannot add channel '" +
                channelName +
                "' from subimage " +
                std::to_string(subimageIndex) +
                ". Keeping the first occurrence.");

            return;
        }

        if (auto *channel = layer->channelInfo(channelName); channel != nullptr)
        {
            warnings.push_back(
                "Channel name conflict: '" +
                layerName + "." + channelName +
                "' in subimage " +
                std::to_string(layer->subimageIndex) +
                " is defined by both '" +
                channel->rawChannelName +
                "' and '" +
                rawChannelName +
                "'. Keeping the first occurrence.");

            return;
        }

        layer->channels.emplace_back(channelName, rawChannelName, rawChannelIndex);
    }

    void defineLayers()
    {
        if (!input)
            return;

        const auto &rootSpec = input->spec();
        const int nsubimages = rootSpec.get_int_attribute("oiio:subimages", 1);
        const bool multipart = nsubimages > 1;

        // first subimage defines the global display window
        width = rootSpec.full_width;
        height = rootSpec.full_height;

        for (int subimageIndex = 0; subimageIndex < nsubimages; ++subimageIndex)
        {
            const auto &spec = input->spec(subimageIndex, 0);

            std::string subimageName = spec.get_string_attribute("oiio:subimagename", "");

            // for (const std::string &rawChannelName : spec.channelnames)
            // {
            for (int i = 0; i < spec.nchannels; ++i)
            {
                const auto &rawChannelName = spec.channelnames[i];

                addChannel(
                    ParsedChannelName(rawChannelName, subimageName, multipart),
                    subimageIndex,
                    rawChannelName, i);
            }
        }
    }

    Rect dataBoundsForLayer(const LayerName &layerName) const
    {
        const auto *layer = layerInfo(layerName);
        if (layer == nullptr)
            return {};

        const auto spec = input->spec_dimensions(layer->subimageIndex, 0);
        return {spec.x - spec.full_x,
                spec.y - spec.full_y,
                spec.width,
                spec.height};
    }
};

ImageReader::~ImageReader() = default;

ImageReader::ImageReader(const std::filesystem::path &path)
    : m_impl(std::make_unique<Impl>())
{
    m_impl->input = OIIO::ImageInput::open(path);
    if (!m_impl->input)
        m_impl->error = OIIO::geterror();

    m_impl->defineLayers();
}

PixelBlock ImageReader::read(const LayerName &layer)
{
    const auto *info = m_impl->layerInfo(layer);
    if (info == nullptr)
    {
        m_impl->warnings.push_back(
            "Failed to read layer '" + layer +
            "': does not exist");
        return {};
    }
    if (!info->channelsContiguous())
    {
        m_impl->warnings.push_back(
            "Failed to read layer '" + layer +
            "': non-contiguous channel indexes.");
        return {};
    }

    PixelBlock block;

    block.dataBounds = m_impl->dataBoundsForLayer(layer);
    block.channels = m_impl->channelNames(layer);

    int nchannels = block.channels.size();
    block.pixels.resize(block.dataBounds.w * block.dataBounds.h * nchannels);

    const bool ok = m_impl->input->read_image(info->subimageIndex, 0,
                                              info->channels.front().rawChannelIndex,
                                              info->channels.back().rawChannelIndex + 1,
                                              OIIO::TypeDesc::FLOAT,
                                              block.pixels.data());

    if (!ok)
    {
        m_impl->error = m_impl->input->geterror();
        return {};
    }

    return block;
}

std::vector<LayerName> ImageReader::layers() const
{
    return m_impl->layerNames();
}

std::vector<ChannelName> ImageReader::channels(const LayerName &layer) const
{
    return m_impl->channelNames(layer);
}

int ImageReader::width() const
{
    return m_impl->width;
}

int ImageReader::height() const
{
    return m_impl->height;
}

std::string ImageReader::error() const
{
    return m_impl->error;
}

std::vector<std::string> ImageReader::warnings() const
{
    return m_impl->warnings;
}

ImageReader::operator bool() const
{
    return bool(m_impl->input);
}

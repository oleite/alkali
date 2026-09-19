#include <ranges>
#include <map>

#include "ImageReader.h"
#include "ImageImpl.h"

#include <OpenImageIO/imageio.h>

struct ReaderChannelInfo
{
    ChannelName name;
    int subimageIndex;
    std::string rawChannelName;
};

struct ReaderLayerInfo
{
    LayerName name;
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
    int nsubimages;
    int width, height;
    Rect dataBounds;

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
        const ParsedChannelName parsed,
        const int subimageIndex,
        const std::string &rawChannelName)
    {
        const auto &layerName = parsed.layer;
        const auto &channelName = parsed.channel;

        auto layer = layerInfo(layerName);
        if (layer == nullptr)
        {
            layers.emplace_back(layerName);
            layer = &layers.back();
        }

        if (auto channel = layer->channelInfo(channelName); channel != nullptr)
        {
            warnings.push_back(
                "Channel name conflict: '" +
                layerName + "." + channelName +
                "' is defined by subimages " +
                std::to_string(channel->subimageIndex) +
                " and " +
                std::to_string(subimageIndex) +
                ". Keeping the first occurrence.");

            return;
        }

        layer->channels.emplace_back(channelName, subimageIndex, rawChannelName);
    }

    void defineLayers()
    {
        if (!input)
            return;

        const auto &rootSpec = input->spec();
        nsubimages = rootSpec.get_int_attribute("oiio:subimages", 1);
        const bool multipart = nsubimages > 1;

        // first subimage defines the global display window
        width = rootSpec.full_width;
        height = rootSpec.full_height;

        for (int subimageIndex = 0; subimageIndex < nsubimages; ++subimageIndex)
        {
            const auto &spec = input->spec(subimageIndex, 0);
            dataBounds.extend({spec.x - spec.full_x,
                               spec.y - spec.full_y,
                               spec.width,
                               spec.height});

            std::string subimageName = spec.get_string_attribute("oiio:subimagename", "");

            for (const std::string &rawChannelName : spec.channelnames)
            {
                addChannel(
                    ParsedChannelName(rawChannelName, subimageName, multipart),
                    subimageIndex,
                    rawChannelName);
            }
        }
    }

    Rect dataBoundsForLayers(const std::vector<LayerName> &layerNames) const
    {
        Rect bounds;
        for (const auto &layerName : layerNames)
        {
            const auto *layer = layerInfo(layerName);
            if (layer == nullptr)
                continue;

            for (const auto &channel : layer->channels)
            {
                const auto spec = input->spec_dimensions(channel.subimageIndex, 0);
                bounds.extend({spec.x - spec.full_x,
                               spec.y - spec.full_y,
                               spec.width,
                               spec.height});
            }
        }
        return bounds;
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

Image ImageReader::readAll()
{
    return read(m_impl->layerNames());
}

Image ImageReader::read(const std::vector<LayerName> &layers)
{
    auto image = Image();

    image.m_impl->width = m_impl->width;
    image.m_impl->height = m_impl->height;

    for (const auto &layerName : layers)
    {
        auto channelNames = m_impl->channelNames(layerName);
        if (channelNames.empty())
            continue;

        image.m_impl->layers.emplace_back(layerName, channelNames);
    }

    image.m_impl->dataBounds = m_impl->dataBoundsForLayers(layers);

    return image;
}

std::vector<LayerName> ImageReader::layers() const
{
    return m_impl->layerNames();
}

std::vector<ChannelName> ImageReader::channels(const LayerName &layer) const
{
    return m_impl->channelNames(layer);
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

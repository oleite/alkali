#include "ImageReader.h"

#include <OpenImageIO/imageio.h>

struct ChannelInfo
{
    std::string name;
    int subimage;
};

struct LayerInfo
{
    int channelCount;
    std::string name;
    std::vector<std::string> channels;
};

struct ImageReader::Impl
{
    std::unique_ptr<OIIO::ImageInput> input;
    std::string error;
    std::vector<LayerName> layers;

    void defineLayers()
    {
        if (!input)
            return;
        // if (rawChannelName.contains('.'))
        // {
        //     std::tie(layerName, channelName) = rsplit(rawChannelName, '.', 1);
        // }
        // else if (isMultipart)
        // {
        //     layerName = subimageName;
        //     channelName = rawChannelName;
        // }
        // else
        // {
        //     layerName = "";
        //     channelName = rawChannelName;
        // }
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
    return Image();
}

Image ImageReader::read(const std::vector<LayerName> &layers)
{
    return Image();
}

std::vector<LayerName> ImageReader::layers() const
{
    return m_impl->layers;
}

std::string ImageReader::error() const
{
    return m_impl->error;
}

ImageReader::operator bool() const
{
    return bool(m_impl->input);
}

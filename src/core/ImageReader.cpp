#include "ImageReader.h"

#include <OpenImageIO/imageio.h>

struct ImageReader::Impl
{
    std::unique_ptr<OIIO::ImageInput> input;
    std::string error;
};

ImageReader::~ImageReader() = default;

ImageReader::ImageReader(const std::filesystem::path &path)
    : m_impl(std::make_unique<Impl>())
{
    m_impl->input = OIIO::ImageInput::open(path);
    if (!m_impl->input)
        m_impl->error = OIIO::geterror();
}

int ImageReader::width() const
{
    if (!m_impl->input)
        return 0;
    return m_impl->input->spec().width;
}

int ImageReader::height() const
{
    if (!m_impl->input)
        return 0;

    return m_impl->input->spec().height;
}

int ImageReader::channelCount() const
{
    if (!m_impl->input)
        return 0;
    return m_impl->input->spec().nchannels;
}

std::vector<std::string> ImageReader::channelNames() const
{
    if (!m_impl->input)
        return {};

    return m_impl->input->spec().channelnames;
}

std::string ImageReader::error() const
{
    return m_impl->error;
}

ImageReader::operator bool() const
{
    return bool(m_impl->input);
}

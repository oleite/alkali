#include <ranges>

#include "Image.h"
#include "ImageImpl.h"

Rect Image::dataBounds() const
{
    return m_impl->dataBounds;
}

int Image::width() const
{
    return m_impl->width;
}

int Image::height() const
{
    return m_impl->height;
}

std::vector<LayerName> Image::layers() const
{
    return m_impl->layerNames();
}

std::vector<ChannelName> Image::channels(const LayerName &layer) const
{
    return m_impl->channelNames(layer);
}

Image::~Image() = default;
Image::Image(Image &&) noexcept = default;
Image &Image::operator=(Image &&) noexcept = default;

Image::Image()
    : m_impl(std::make_unique<Impl>())
{
}

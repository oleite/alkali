#include "Image.h"

struct Image::Impl
{
};

Rect Image::dataBounds() const
{
    return Rect();
}

int Image::width() const
{
    return 0;
}

int Image::height() const
{
    return 0;
}

std::vector<LayerName> Image::layers() const
{
    return std::vector<LayerName>();
}

std::vector<ChannelName> Image::channels(const LayerName &layer) const
{
    return std::vector<ChannelName>();
}

Image::~Image() = default;
Image::Image(Image &&) noexcept = default;
Image &Image::operator=(Image &&) noexcept = default;

Image::Image()
    : m_impl(std::make_unique<Impl>())
{
}

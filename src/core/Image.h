#pragma once

#include "Rect.h"

#include <memory>
#include <string>
#include <vector>

// 'diffuse', 'forward.right', 'forward.left', 'beauty'
using LayerName = std::string;

// 'red', 'R, 'green', 'G' 'blue', 'B', 'u', 'v', 'mask'
using ChannelName = std::string;

class ImageReader;

class Image
{
public:
    Rect dataBounds() const;
    int width() const;
    int height() const;
    std::vector<LayerName> layers() const;
    std::vector<ChannelName> channels(const LayerName &layer) const;

    ~Image();

    Image(Image &&) noexcept;
    Image &operator=(Image &&) noexcept;

    Image(const Image &) = delete;
    Image &operator=(const Image &) = delete;

private:
    Image();

    friend class ImageReader;

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

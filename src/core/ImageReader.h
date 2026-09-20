#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "Rect.h"

using LayerName = std::string;
using ChannelName = std::string;

struct PixelBlock
{
    Rect dataBounds;
    std::vector<ChannelName> channels;

    // interleaved (x, y, c)
    // size = dataBounds.w * dataBounds.h * channels.size()
    std::vector<float> pixels;
};

class ImageReader
{
public:
    ImageReader(const std::filesystem::path &path);
    ~ImageReader();

    PixelBlock read(const LayerName &layer);

    std::vector<LayerName> layers() const;
    std::vector<ChannelName> channels(const LayerName &layer) const;

    int width() const;
    int height() const;

    std::string error() const;
    std::vector<std::string> warnings() const;

    explicit operator bool() const;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

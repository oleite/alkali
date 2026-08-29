#pragma once

#include <filesystem>

class ImageReader
{
public:
    ImageReader(const std::filesystem::path path);
    int width() const;
    int height() const;
    int channelCount() const;

    explicit operator bool() const;
};
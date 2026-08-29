#pragma once

#include <filesystem>
#include <memory>

class ImageReader
{
public:
    ImageReader(const std::filesystem::path &path);
    ~ImageReader();

    int width() const;
    int height() const;
    int channelCount() const;
    std::vector<std::string> channelNames() const;
    std::string error() const;

    explicit operator bool() const;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};
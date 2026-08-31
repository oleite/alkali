#pragma once

#include "Image.h"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

class ImageReader
{
public:
    ImageReader(const std::filesystem::path &path);
    ~ImageReader();

    Image readAll();
    Image read(const std::vector<LayerName> &layers);

    std::vector<LayerName> layers() const;

    std::string error() const;

    explicit operator bool() const;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

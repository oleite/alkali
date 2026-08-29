#include "ImageReader.h"

ImageReader::ImageReader(const std::filesystem::path path)
{
}

int ImageReader::width() const
{
    return 2048;
}

int ImageReader::height() const
{
    return 1536;
}

int ImageReader::channelCount() const
{
    return 3;
}

ImageReader::operator bool() const
{
    return true;
}

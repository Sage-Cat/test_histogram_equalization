#include "Image.h"

#include <algorithm>
#include <charconv>
#include <cctype>
#include <cmath>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <string_view>

namespace
{
std::string readPpmToken(std::istream &stream)
{
    std::string token;
    char character = 0;

    while (stream.get(character))
    {
        if (character == '#')
        {
            stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        else if (!std::isspace(static_cast<unsigned char>(character)))
        {
            token.push_back(character);
            break;
        }
    }

    while (stream.get(character))
    {
        if (std::isspace(static_cast<unsigned char>(character)))
        {
            if (character == '\r' && stream.peek() == '\n')
            {
                stream.get();
            }
            break;
        }
        if (character == '#')
        {
            stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            break;
        }
        token.push_back(character);
        if (token.size() > 64)
        {
            throw std::runtime_error("Malformed PPM header token");
        }
    }

    if (token.empty())
    {
        throw std::runtime_error("Malformed PPM header");
    }
    return token;
}

std::size_t parseSize(std::string_view token, std::string_view field)
{
    std::size_t value = 0;
    const auto result = std::from_chars(token.data(), token.data() + token.size(), value);
    if (result.ec != std::errc{} || result.ptr != token.data() + token.size() || value == 0)
    {
        throw std::runtime_error("Invalid PPM " + std::string(field));
    }
    return value;
}

std::size_t checkedArea(std::size_t width, std::size_t height)
{
    if (width == 0 || height == 0)
    {
        throw std::invalid_argument("Image dimensions must be positive");
    }
    if (width > std::numeric_limits<std::size_t>::max() / height)
    {
        throw std::overflow_error("Image dimensions are too large");
    }
    const std::size_t area = width * height;
    const auto streamLimit = static_cast<std::size_t>(std::numeric_limits<std::streamsize>::max());
    if (area > std::numeric_limits<std::size_t>::max() / 3 || area > streamLimit / 3)
    {
        throw std::overflow_error("Image byte count is too large");
    }
    return area;
}

unsigned char toByte(double value)
{
    return static_cast<unsigned char>(std::lround(std::clamp(value, 0.0, 1.0) * 255.0));
}
}

Image::Image(const std::string &path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Unable to open input image: " + path);
    }

    if (readPpmToken(file) != "P6")
    {
        throw std::runtime_error("Unsupported image format; expected binary PPM (P6)");
    }

    _width = parseSize(readPpmToken(file), "width");
    _height = parseSize(readPpmToken(file), "height");
    if (parseSize(readPpmToken(file), "maximum value") != 255)
    {
        throw std::runtime_error("Unsupported PPM maximum value; expected 255");
    }

    const std::size_t pixelCount = checkedArea(_width, _height);
    std::vector<unsigned char> bytes(pixelCount * 3);
    if (!file.read(reinterpret_cast<char *>(bytes.data()), static_cast<std::streamsize>(bytes.size())))
    {
        throw std::runtime_error("PPM pixel data is truncated");
    }

    _data.resize(pixelCount);
    for (std::size_t i = 0; i < pixelCount; ++i)
    {
        _data[i] = {
            bytes[i * 3] / 255.0,
            bytes[i * 3 + 1] / 255.0,
            bytes[i * 3 + 2] / 255.0,
        };
    }
}

Image::Image(std::size_t width, std::size_t height, const Pixel &fill)
    : _width(width), _height(height), _data(checkedArea(width, height), fill)
{
}

Pixel Image::getPixel(std::size_t x, std::size_t y) const
{
    if (x >= _width || y >= _height)
    {
        throw std::out_of_range("Pixel coordinates are outside the image");
    }
    return _data.at(_width * y + x);
}

void Image::setPixel(std::size_t x, std::size_t y, const Pixel &value)
{
    if (x >= _width || y >= _height)
    {
        throw std::out_of_range("Pixel coordinates are outside the image");
    }
    _data.at(_width * y + x) = value;
}

std::size_t Image::width() const noexcept
{
    return _width;
}

std::size_t Image::height() const noexcept
{
    return _height;
}

std::size_t Image::area() const noexcept
{
    return _width * _height;
}

bool Image::empty() const noexcept
{
    return _data.empty();
}

void Image::writeToFile(const std::string &path) const
{
    if (empty())
    {
        throw std::runtime_error("Cannot write an empty image");
    }

    std::ofstream file(path, std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Unable to open output image: " + path);
    }

    file << "P6\n" << _width << ' ' << _height << "\n255\n";
    std::vector<unsigned char> bytes(_data.size() * 3);
    for (std::size_t i = 0; i < _data.size(); ++i)
    {
        bytes[i * 3] = toByte(_data[i].r);
        bytes[i * 3 + 1] = toByte(_data[i].g);
        bytes[i * 3 + 2] = toByte(_data[i].b);
    }
    file.write(reinterpret_cast<const char *>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
    if (!file)
    {
        throw std::runtime_error("Unable to write output image: " + path);
    }
}

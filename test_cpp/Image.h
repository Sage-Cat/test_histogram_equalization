#pragma once

#include <vector>
#include <string>

struct Pixel
{
    double r = 0.0, g = 0.0, b = 0.0;
};

class Image
{
public:
    Image() = default;
    Image(const std::string &path);
    Image(size_t width, size_t height) noexcept;
    Pixel getPixel(size_t x, size_t y) const noexcept;
    void setPixel(size_t x, size_t y, const Pixel &value) noexcept;
    size_t width() const noexcept;
    size_t height() const noexcept;
    size_t area() const noexcept;
    void writeToFile(const std::string &path);

private:
    size_t _width = 0, _height = 0;
    std::vector<Pixel> _data;
};

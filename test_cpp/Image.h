#pragma once

#include <cstddef>
#include <string>
#include <vector>

struct Pixel
{
    double r{0.0};
    double g{0.0};
    double b{0.0};
};

class Image
{
public:
    Image() = default;
    explicit Image(const std::string &path);
    Image(std::size_t width, std::size_t height, const Pixel &fill = Pixel{});

    Pixel getPixel(std::size_t x, std::size_t y) const;
    void setPixel(std::size_t x, std::size_t y, const Pixel &value);
    std::size_t width() const noexcept;
    std::size_t height() const noexcept;
    std::size_t area() const noexcept;
    bool empty() const noexcept;
    void writeToFile(const std::string &path) const;

private:
    std::size_t _width{0};
    std::size_t _height{0};
    std::vector<Pixel> _data;
};

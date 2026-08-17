#include "ImageProcessing.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <stdexcept>

namespace
{
constexpr std::size_t histogramSize = 256;
using Histogram = std::array<std::size_t, histogramSize>;

double luminance(const Pixel &pixel)
{
    return std::clamp(0.299 * pixel.r + 0.587 * pixel.g + 0.114 * pixel.b, 0.0, 1.0);
}

std::size_t luminanceBin(const Pixel &pixel)
{
    return static_cast<std::size_t>(std::lround(luminance(pixel) * 255.0));
}

void validateRoi(const Image &image, const Rect &roi)
{
    if (image.empty())
    {
        throw std::invalid_argument("Cannot equalize an empty image");
    }
    if (roi.width == 0 || roi.height == 0 || roi.x > image.width() || roi.y > image.height() ||
        roi.width > image.width() - roi.x || roi.height > image.height() - roi.y)
    {
        throw std::invalid_argument("Region of interest is outside the image");
    }
}
}

void doHistogramEqualization(Image &image, const Rect &roi)
{
    validateRoi(image, roi);

    Histogram histogram{};
    for (std::size_t y = roi.y; y < roi.y + roi.height; ++y)
    {
        for (std::size_t x = roi.x; x < roi.x + roi.width; ++x)
        {
            ++histogram[luminanceBin(image.getPixel(x, y))];
        }
    }

    Histogram cumulative{};
    std::size_t runningTotal = 0;
    std::size_t firstNonZero = 0;
    for (std::size_t i = 0; i < histogram.size(); ++i)
    {
        runningTotal += histogram[i];
        cumulative[i] = runningTotal;
        if (firstNonZero == 0 && histogram[i] != 0)
        {
            firstNonZero = runningTotal;
        }
    }

    const std::size_t pixelCount = roi.width * roi.height;
    if (pixelCount == firstNonZero)
    {
        return; // A constant-luminance image has no contrast to expand.
    }

    std::array<double, histogramSize> mapping{};
    const double denominator = static_cast<double>(pixelCount - firstNonZero);
    for (std::size_t i = 0; i < mapping.size(); ++i)
    {
        const std::size_t adjusted = cumulative[i] > firstNonZero ? cumulative[i] - firstNonZero : 0;
        mapping[i] = static_cast<double>(adjusted) / denominator;
    }

    for (std::size_t y = roi.y; y < roi.y + roi.height; ++y)
    {
        for (std::size_t x = roi.x; x < roi.x + roi.width; ++x)
        {
            const Pixel pixel = image.getPixel(x, y);
            const double oldY = luminance(pixel);
            const double cb = (pixel.b - oldY) * 0.564;
            const double cr = (pixel.r - oldY) * 0.713;
            const double newY = mapping[luminanceBin(pixel)];

            image.setPixel(x, y, {
                                     std::clamp(newY + 1.403 * cr, 0.0, 1.0),
                                     std::clamp(newY - 0.344 * cb - 0.714 * cr, 0.0, 1.0),
                                     std::clamp(newY + 1.773 * cb, 0.0, 1.0),
                                 });
        }
    }
}

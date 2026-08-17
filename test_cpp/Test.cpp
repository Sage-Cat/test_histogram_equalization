#include "Test.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

DifferenceMetrics compareTwoImages(const Image &first, const Image &second)
{
    if (first.empty() || second.empty())
    {
        throw std::invalid_argument("Cannot compare empty images");
    }
    if (first.width() != second.width() || first.height() != second.height())
    {
        throw std::invalid_argument("Image dimensions do not match");
    }

    DifferenceMetrics metrics;
    double totalError = 0.0;
    for (std::size_t y = 0; y < first.height(); ++y)
    {
        for (std::size_t x = 0; x < first.width(); ++x)
        {
            const Pixel lhs = first.getPixel(x, y);
            const Pixel rhs = second.getPixel(x, y);
            const double errors[] = {
                std::abs(lhs.r - rhs.r),
                std::abs(lhs.g - rhs.g),
                std::abs(lhs.b - rhs.b),
            };
            for (const double error : errors)
            {
                metrics.maximumAbsoluteError = std::max(metrics.maximumAbsoluteError, error);
                totalError += error;
            }
        }
    }

    metrics.meanAbsoluteError = totalError / static_cast<double>(first.area() * 3);
    return metrics;
}

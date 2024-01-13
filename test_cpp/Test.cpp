#include "Test.h"

#include <cmath>
#include <cassert>
#include <algorithm>
#include <iostream>

#include "Image.h"

void compareTwoImages(const std::string &imagePath1, const std::string &imagePath2)
{
    Image image1(imagePath1);
    Image image2(imagePath2);

    // Asserting that the dimensions of the two images are the same
    assert(image1.width() == image2.width());
    assert(image1.height() == image2.height());

    // Lambda function to calculate the maximum difference between two pixels
    auto pixelDifference = [](const Pixel &p1, const Pixel &p2)
    {
        return std::max({std::abs(p1.r - p2.r), std::abs(p1.g - p2.g), std::abs(p1.b - p2.b)});
    };

    double maxDiff = 0.0; // Using auto deduction for variable type
    for (size_t x = 0; x < image1.width(); ++x)
    {
        for (size_t y = 0; y < image1.height(); ++y)
        {
            auto pix1 = image1.getPixel(x, y);
            auto pix2 = image2.getPixel(x, y);
            maxDiff = std::max(maxDiff, pixelDifference(pix1, pix2));
        }
    }

    std::cout << "Max pixel difference: " << maxDiff << std::endl;
}

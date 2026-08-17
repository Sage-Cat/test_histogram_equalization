#pragma once

#include "Image.h"

#include <cstddef>

struct Rect
{
    std::size_t x;
    std::size_t y;
    std::size_t width;
    std::size_t height;
};

// Equalizes 8-bit luminance inside the ROI while retaining the chroma channels.
void doHistogramEqualization(Image &image, const Rect &roi);

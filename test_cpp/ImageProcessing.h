#pragma once

#include "Image.h"

struct Rect {
    int x, y, width, height;
};

void doHistogramEqualization(Image& image, Rect roi);

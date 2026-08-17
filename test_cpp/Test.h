#pragma once

#include "Image.h"

struct DifferenceMetrics
{
    double maximumAbsoluteError{0.0};
    double meanAbsoluteError{0.0};
};

DifferenceMetrics compareTwoImages(const Image &first, const Image &second);

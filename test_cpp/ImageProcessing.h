#pragma once

#include "Image.h"

// Define a structure to represent a rectangular region of interest (ROI) in an image
struct Rect
{
    int x;      // X-coordinate of the top-left corner
    int y;      // Y-coordinate of the top-left corner
    int width;  // Width of the rectangle
    int height; // Height of the rectangle
};

// Function to perform histogram equalization on the entire image for each RGB channel
// within a specified region of interest (ROI)
void doHistogramEqualization(Image &image, const Rect &roi);

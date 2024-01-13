#include "ImageProcessing.h"

#include <array>
#include <cassert>
#include <limits>

enum RGB
{
    R = 0,
    G = 1,
    B = 2
};
constexpr auto MAX_VALUE = std::numeric_limits<uint16_t>::max();
using Histogram = std::array<size_t, MAX_VALUE>;

// Calculate histogram for a specific channel
void calculateHistogram(const Image &image, int channel, Histogram &histogram)
{
    for (size_t x = 0; x < image.width(); ++x)
    {
        for (size_t y = 0; y < image.height(); ++y)
        {
            auto pixel = image.getPixel(x, y);
            // Access the specific channel of the pixel safely
            double value = (channel == R) ? pixel.r : (channel == G) ? pixel.g
                                                                     : pixel.b;
            // Increment the histogram bin corresponding to the pixel value
            histogram[static_cast<size_t>(value * (MAX_VALUE - 1))]++;
        }
    }
}

// Calculate the cumulative frequency histogram
void calculateCFH(const Histogram &histogram, Histogram &cfh)
{
    size_t sum = 0;
    for (size_t i = 0; i < histogram.size(); ++i)
    {
        // Cumulative sum of histogram values
        sum += histogram[i];
        cfh[i] = sum;
    }
}

// Perform histogram equalization on the entire image for each RGB channel
void equalizeChannel(Image &image, const Rect &roi, int channel, const Histogram &cfh)
{
    for (size_t x = roi.x; x < roi.x + roi.width; ++x)
    {
        for (size_t y = roi.y; y < roi.y + roi.height; ++y)
        {
            auto pixel = image.getPixel(x, y);
            // Direct reference to the specific channel for modification
            double &currentChannel = (channel == R) ? pixel.r : (channel == G) ? pixel.g
                                                                               : pixel.b;
            // Apply histogram equalization transformation
            currentChannel = cfh[static_cast<size_t>(currentChannel * (MAX_VALUE - 1))] * static_cast<double>(MAX_VALUE) / image.area() / MAX_VALUE;
            // Update the pixel value in the image
            image.setPixel(x, y, pixel);
        }
    }
}

void doHistogramEqualization(Image &image, const Rect &roi)
{
    // Ensure the ROI is within the image boundaries
    assert(roi.x >= 0 && roi.x + roi.width <= image.width());
    assert(roi.y >= 0 && roi.y + roi.height <= image.height());

    for (int ch = 0; ch < 3; ++ch)
    { // Loop through each RGB channel
        Histogram hist{}, cfh{};
        calculateHistogram(image, ch, hist);  // Calculate histogram for the channel
        calculateCFH(hist, cfh);              // Calculate cumulative frequency histogram
        equalizeChannel(image, roi, ch, cfh); // Equalize the channel
    }
}

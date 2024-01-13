#include "ImageProcessing.h"

#include <array>
#include <cassert>

enum RGB {R = 0, G = 1, B = 2};
constexpr auto MAX_VALUE = std::numeric_limits<uint16_t>::max();
using Histogram = std::array<size_t, MAX_VALUE>;

void calculateHistogram(Image image, int channel, Histogram& histogram)
{
    for (size_t x = 0; x < image.width(); ++x) {
        for (size_t y = 0; y < image.height(); ++y) {
            auto pixel = image.getPixel(x, y);
            histogram[(*(reinterpret_cast<double*>(&pixel) + channel) * MAX_VALUE)]++;
        }
    }
}

void calculateCFH(Histogram const histogram, Histogram& cfh)
{
    size_t sum = 0;
    for(size_t i = 0; i < histogram.size(); ++i) {
        cfh[i] = sum += histogram[i];
    }
}

void equalizeChannel(Image& image, Rect roi, int channel, Histogram const cfh)
{
    for (size_t x = roi.x; x < roi.x + roi.width; ++x) {
        for (size_t y = roi.y; y < roi.y + roi.height; ++y) {
            auto pixel = image.getPixel(x, y);
            auto currentChannel = (reinterpret_cast<double*>(&pixel) + channel);
            *currentChannel = cfh[*currentChannel * (MAX_VALUE - 1)] * static_cast<double>(MAX_VALUE) / image.area() / MAX_VALUE;
            image.setPixel(x, y, pixel);
        }
    }
}

void doHistogramEqualization(Image& image, Rect roi)
{
    // Image include ROI ?
    assert(roi.x >= 0);
    assert(roi.x + roi.width <= image.width());
    assert(roi.y >= 0);
    assert(roi.y + roi.height <= image.height());
    
    for (int ch = 0; ch < 3; ++ch) {
        Histogram hist{}, cfh{};
        calculateHistogram(image, ch, hist);
        calculateCFH(hist, cfh);
        equalizeChannel(image, roi, ch, cfh);
    }
}

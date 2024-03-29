#include "ImageProcessing.h"
#include <array>
#include <cassert>
#include <limits>
#include <thread>
#include <vector>
#include <immintrin.h>

enum RGB
{
    R = 0,
    G = 1,
    B = 2
};
constexpr auto MAX_VALUE = std::numeric_limits<uint16_t>::max();
using Histogram = std::array<size_t, MAX_VALUE>;

void calculateHistogram(const Image &image, int channel, Histogram &histogram, const Rect &roi)
{
    double scaleFactor = MAX_VALUE - 1;
    for (size_t y = roi.y; y < roi.y + roi.height; ++y)
    {
        for (size_t x = roi.x; x < roi.x + roi.width; ++x)
        {
            auto pixel = image.getPixel(x, y);
            double value = (channel == R) ? pixel.r : (channel == G) ? pixel.g
                                                                     : pixel.b;
            histogram[static_cast<size_t>(value * scaleFactor)]++;
        }
    }
}

void calculateCFH(const Histogram &histogram, Histogram &cfh)
{
    size_t sum = 0;
    for (size_t i = 0; i < histogram.size(); ++i)
    {
        sum += histogram[i];
        cfh[i] = sum;
    }
}

void equalizeChannel(Image &image, const Rect &roi, int channel, const Histogram &cfh)
{
    double imageArea = static_cast<double>(image.area());
    double maxValueInv = 1.0 / MAX_VALUE;

    for (size_t y = roi.y; y < roi.y + roi.height; ++y)
    {
        for (size_t x = roi.x; x < roi.x + roi.width; ++x)
        {
            auto pixel = image.getPixel(x, y);
            double &currentChannel = (channel == R) ? pixel.r : (channel == G) ? pixel.g
                                                                               : pixel.b;
            currentChannel = cfh[static_cast<size_t>(currentChannel * (MAX_VALUE - 1))] * maxValueInv / imageArea;
            image.setPixel(x, y, pixel);
        }
    }
}

void doHistogramEqualization(Image &image, const Rect &roi)
{
    assert(roi.x >= 0 && roi.x + roi.width <= image.width());
    assert(roi.y >= 0 && roi.y + roi.height <= image.height());

    std::vector<std::thread> threads;
    // Divide image into blocks for parallel processing
    int numThreads = std::thread::hardware_concurrency();
    int blockHeight = roi.height / numThreads;

    for (int ch = 0; ch < 3; ++ch)
    {
        for (int i = 0; i < numThreads; ++i)
        {
            Rect blockRoi = {roi.x, roi.y + i * blockHeight, roi.width, blockHeight};
            if (i == numThreads - 1)
            {
                // Handle the last block
                blockRoi.height = roi.height - (numThreads - 1) * blockHeight;
            }

            threads.emplace_back([&, ch, blockRoi]()
                                 {
                Histogram hist{}, cfh{};
                calculateHistogram(image, ch, hist, blockRoi);
                calculateCFH(hist, cfh);
                equalizeChannel(image, blockRoi, ch, cfh); });
        }
    }

    for (auto &thread : threads)
    {
        thread.join();
    }
}

#include "Image.h"
#include "ImageProcessing.h"
#include "Test.h"

#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string_view>

namespace
{
void expect(bool condition, std::string_view message)
{
    if (!condition)
    {
        std::cerr << "FAILED: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

bool near(double lhs, double rhs, double tolerance = 0.005)
{
    return std::abs(lhs - rhs) <= tolerance;
}

template <typename Function>
void expectThrows(Function function, std::string_view message)
{
    try
    {
        function();
    }
    catch (const std::exception &)
    {
        return;
    }
    expect(false, message);
}
}

int main()
{
    Image gradient(4, 1);
    gradient.setPixel(0, 0, {0.25, 0.25, 0.25});
    gradient.setPixel(1, 0, {0.50, 0.50, 0.50});
    gradient.setPixel(2, 0, {0.50, 0.50, 0.50});
    gradient.setPixel(3, 0, {0.75, 0.75, 0.75});
    doHistogramEqualization(gradient, {0, 0, 4, 1});
    expect(near(gradient.getPixel(0, 0).r, 0.0), "lowest luminance maps to black");
    expect(near(gradient.getPixel(1, 0).r, 2.0 / 3.0), "CDF mapping uses the full ROI");
    expect(near(gradient.getPixel(3, 0).r, 1.0), "highest luminance maps to white");

    Image constant(2, 2, {0.2, 0.4, 0.6});
    const Image constantReference = constant;
    doHistogramEqualization(constant, {0, 0, 2, 2});
    expect(compareTwoImages(constant, constantReference).maximumAbsoluteError == 0.0,
           "constant luminance remains unchanged");

    Image partial(3, 1);
    partial.setPixel(0, 0, {0.4, 0.4, 0.4});
    partial.setPixel(1, 0, {0.25, 0.25, 0.25});
    partial.setPixel(2, 0, {0.75, 0.75, 0.75});
    doHistogramEqualization(partial, {1, 0, 2, 1});
    expect(near(partial.getPixel(0, 0).r, 0.4), "pixels outside the ROI are untouched");
    expect(near(partial.getPixel(1, 0).r, 0.0), "ROI low value maps to black");
    expect(near(partial.getPixel(2, 0).r, 1.0), "ROI high value maps to white");
    expectThrows([&] { doHistogramEqualization(partial, {2, 0, 2, 1}); },
                 "out-of-range ROI is rejected");
    expectThrows([&] { partial.getPixel(3, 0); }, "out-of-range pixel access is rejected");
    Image empty;
    expectThrows([&] { doHistogramEqualization(empty, {0, 0, 1, 1}); },
                 "empty images are rejected");

    const Image black(1, 1, {0.0, 0.0, 0.0});
    const Image mixed(1, 1, {1.0, 0.5, 0.0});
    const DifferenceMetrics knownMetrics = compareTwoImages(black, mixed);
    expect(near(knownMetrics.maximumAbsoluteError, 1.0), "maximum metric is per-channel absolute error");
    expect(near(knownMetrics.meanAbsoluteError, 0.5), "mean metric averages every channel");
    expectThrows([&] { compareTwoImages(black, partial); }, "dimension mismatch is rejected");

    const auto fixture = std::filesystem::temp_directory_path() / "histogram_equalization_fixture.ppm";
    const auto malformed = std::filesystem::temp_directory_path() / "histogram_equalization_malformed.ppm";
    partial.writeToFile(fixture.string());
    const Image roundTrip(fixture.string());
    expect(compareTwoImages(partial, roundTrip).maximumAbsoluteError <= 1.0 / 255.0,
           "generated PPM fixture round-trips within one byte");

    {
        std::ofstream file(malformed, std::ios::binary);
        file << "P6\n2 2\n255\n" << "short";
    }
    expectThrows([&] { static_cast<void>(Image(malformed.string())); }, "truncated PPM is rejected");

    std::filesystem::remove(fixture);
    std::filesystem::remove(malformed);
    std::cout << "All histogram tests passed\n";
    return EXIT_SUCCESS;
}

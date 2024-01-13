#include "ImageProcessing.h"

#include <iostream>

#include "Timer.h"
#include "Test.h"

int main(int argc, const char *argv[])
{
    // Ensure the correct number of arguments
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " <input_image> <output_image> [reference_image]" << std::endl;
        return EXIT_FAILURE;
    }

    // Measure overall processing time
    ScopeTimer overallTimer("Overall time");

    // Load the input image
    Image image(argv[1]);

    // Perform histogram equalization
    {
        ScopeTimer timer("Histogram equalization time");
        Rect roi{0, 0, static_cast<int>(image.width()), static_cast<int>(image.height())};
        doHistogramEqualization(image, roi);
    }

    // Save the processed image
    image.writeToFile(argv[2]);

    // Optional: Compare output image with a reference image
    if (argc == 4)
    {
        compareTwoImages(argv[2], argv[3]);
    }

    return EXIT_SUCCESS;
}

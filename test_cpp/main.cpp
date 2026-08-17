#include "ImageProcessing.h"
#include "Test.h"
#include "Timer.h"

#include <cstdlib>
#include <exception>
#include <iomanip>
#include <iostream>

int main(int argc, const char *argv[])
{
    if (argc != 3 && argc != 4)
    {
        std::cerr << "Usage: histogram_equalize INPUT.ppm OUTPUT.ppm [REFERENCE.ppm]\n";
        return EXIT_FAILURE;
    }

    try
    {
        ScopeTimer overallTimer("Overall time");
        Image image(argv[1]);
        {
            ScopeTimer timer("Histogram equalization time");
            doHistogramEqualization(image, {0, 0, image.width(), image.height()});
        }
        image.writeToFile(argv[2]);

        if (argc == 4)
        {
            constexpr double acceptedMaximumError = 0.005;
            const DifferenceMetrics metrics = compareTwoImages(Image(argv[2]), Image(argv[3]));
            std::cout << std::fixed << std::setprecision(6)
                      << "Maximum absolute channel error: " << metrics.maximumAbsoluteError << '\n'
                      << "Mean absolute channel error: " << metrics.meanAbsoluteError << '\n';
            if (metrics.maximumAbsoluteError > acceptedMaximumError)
            {
                std::cerr << "Reference comparison exceeds the 0.5% channel-error limit\n";
                return EXIT_FAILURE;
            }
        }
    }
    catch (const std::exception &error)
    {
        std::cerr << "Error: " << error.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

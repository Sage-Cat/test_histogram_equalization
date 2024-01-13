#pragma once

#include <string>

/// Compare two images (supports ppm p6 binary format or jpeg)
/// and prints the maximum pixel difference.
void compareTwoImages(const std::string &imagePath1, const std::string &imagePath2);

#include "Image.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstring> // for memcpy
#include <memory>  // for smart pointers

#include "jpeg_decoder.h"

// Get a pixel value from the image
Pixel Image::getPixel(size_t x, size_t y) const noexcept
{
    return _data.at(_width * y + x);
}

// Set a pixel value in the image
void Image::setPixel(size_t x, size_t y, const Pixel &value) noexcept
{
    _data.at(_width * y + x) = value;
}

// Get the width of the image
size_t Image::width() const noexcept
{
    return _width;
}

// Get the height of the image
size_t Image::height() const noexcept
{
    return _height;
}

// Calculate the area of the image
size_t Image::area() const noexcept
{
    return _width * _height;
}

// Constructor to load an image from a file
Image::Image(const std::string &path)
{
    // Lambda for extracting file extension in lowercase
    auto getFileExtension = [](const std::string &path) -> std::string
    {
        auto dotPos = path.rfind(".");
        if (dotPos == std::string::npos)
            return "";
        std::string extension = path.substr(dotPos + 1);
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        return extension;
    };

    std::string extension = getFileExtension(path);

    if (extension == "jpg" || extension == "jpeg")
    {
        // Open file with C-style fopen for performance
        FILE *f = fopen(path.c_str(), "rb");
        if (!f)
        {
            std::cerr << "Error opening the input file." << std::endl;
            return;
        }
        // Determine file size for buffer allocation
        fseek(f, 0, SEEK_END);
        size_t size = ftell(f);
        // Use smart pointer for automatic memory management
        auto buf = std::unique_ptr<unsigned char[]>(new unsigned char[size]);
        fseek(f, 0, SEEK_SET);
        fread(buf.get(), 1, size, f);
        fclose(f);

        // Decode JPEG using external library
        Jpeg::Decoder decoder(buf.get(), size);
        if (decoder.GetResult() != Jpeg::Decoder::OK)
        {
            std::cerr << "Error decoding the JPEG file." << std::endl;
            return;
        }

        // Set image dimensions and reserve memory for pixel data
        _width = decoder.GetWidth();
        _height = decoder.GetHeight();
        _data.resize(_width * _height);
        auto *decodedData = decoder.GetImage();
        // Convert raw data to Pixel objects
        for (size_t i = 0; i < _width * _height; ++i)
        {
            _data[i] = Pixel{decodedData[i * 3] / 255.0, decodedData[i * 3 + 1] / 255.0, decodedData[i * 3 + 2] / 255.0};
        }
    }
    else if (extension == "ppm")
    {
        // Open PPM file with C-style I/O for better performance
        FILE *f = fopen(path.c_str(), "rb");
        if (!f)
        {
            std::cerr << "Error opening the input file \"" << path << "\"." << std::endl;
            return;
        }

        // Read and verify the PPM header
        char header[20];
        if (!fgets(header, sizeof(header), f))
        {
            std::cerr << "Error reading the header of the PPM file." << std::endl;
            fclose(f);
            return;
        }

        if (strncmp(header, "P6", 2) != 0)
        {
            std::cerr << "Unsupported PPM format." << std::endl;
            fclose(f);
            return;
        }

        // Read image dimensions
        unsigned w, h;
        if (fscanf(f, "%u %u\n255\n", &w, &h) != 2)
        {
            std::cerr << "Error reading the width and height from the PPM file." << std::endl;
            fclose(f);
            return;
        }

        _width = w;
        _height = h;
        _data.resize(_width * _height);

        // Allocate buffer for pixel data and read it
        size_t pixelDataSize = _width * _height * 3;
        auto pixelData = std::unique_ptr<unsigned char[]>(new unsigned char[pixelDataSize]);
        if (fread(pixelData.get(), 1, pixelDataSize, f) != pixelDataSize)
        {
            std::cerr << "Error reading the pixel data from the PPM file." << std::endl;
            fclose(f);
            return;
        }

        // Convert raw data to Pixel objects
        for (size_t i = 0; i < _width * _height; ++i)
        {
            _data[i] = Pixel{pixelData[i * 3] / 255.0, pixelData[i * 3 + 1] / 255.0, pixelData[i * 3 + 2] / 255.0};
        }

        fclose(f);
    }
}

// Write the image to a file
void Image::writeToFile(const std::string &path)
{
    // Using C-style fopen for better performance in binary file writing
    FILE *f = fopen(path.c_str(), "wb");
    if (!f)
    {
        std::cerr << "Error opening the output file." << std::endl;
        return;
    }

    // Writing PPM header
    fprintf(f, "P6\n%d %d\n255\n", static_cast<int>(_width), static_cast<int>(_height));

    // Preparing pixel data for writing
    size_t pixelDataSize = _width * _height * 3;
    auto pixelData = std::unique_ptr<unsigned char[]>(new unsigned char[pixelDataSize]);
    for (size_t i = 0; i < _width * _height; ++i)
    {
        size_t offset = i * 3;
        pixelData[offset] = static_cast<unsigned char>(_data[i].r * 255);
        pixelData[offset + 1] = static_cast<unsigned char>(_data[i].g * 255);
        pixelData[offset + 2] = static_cast<unsigned char>(_data[i].b * 255);
    }

    // Writing pixel data in a single operation for performance
    fwrite(pixelData.get(), 1, pixelDataSize, f);

    fclose(f);
}

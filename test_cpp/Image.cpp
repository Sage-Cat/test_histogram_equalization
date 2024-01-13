#include "Image.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <memory>
#include <vector>
#include <sstream>

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
        // Open the PPM file in binary mode and position the file pointer at the end
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file)
        {
            std::cerr << "Error opening the input file \"" << path << "\"." << std::endl;
            return;
        }

        // Determine the file size and allocate a buffer
        auto fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        std::vector<unsigned char> buffer(fileSize);

        // Read the entire file into the buffer
        if (!file.read(reinterpret_cast<char *>(buffer.data()), fileSize))
        {
            std::cerr << "Error reading the PPM file." << std::endl;
            return;
        }

        // Parse the PPM header from the buffer
        std::istringstream headerStream(std::string(buffer.begin(), buffer.begin() + 15)); // Assuming header is within first 15 bytes
        std::string type;
        headerStream >> type; // Should be "P6"
        if (type != "P6")
        {
            std::cerr << "Unsupported PPM format: " << type << std::endl;
            return;
        }

        size_t w, h;
        headerStream >> w >> h;
        _width = w;
        _height = h;

        // Skip to the end of the header (after "255\n")
        size_t headerEnd = headerStream.tellg();
        auto pixelDataStart = std::find(buffer.begin() + headerEnd, buffer.end(), '\n') - buffer.begin() + 1;

        // Process the pixel data
        _data.resize(_width * _height);
        const unsigned char *pixelData = buffer.data() + pixelDataStart;
        for (size_t i = 0; i < _width * _height; ++i)
        {
            _data[i] = Pixel{
                pixelData[i * 3] / 255.0,
                pixelData[i * 3 + 1] / 255.0,
                pixelData[i * 3 + 2] / 255.0};
        }
    }
}

// Write the image to a file
void Image::writeToFile(const std::string &path)
{
    std::ofstream file(path, std::ios::binary);
    if (!file)
    {
        std::cerr << "Error opening the output file." << std::endl;
        return;
    }

    file << "P6\n"
         << _width << " " << _height << "\n255\n";

    std::vector<unsigned char> pixelData(_width * _height * 3);
    for (size_t i = 0; i < _width * _height; ++i)
    {
        pixelData[i * 3] = static_cast<unsigned char>(_data[i].r * 255);
        pixelData[i * 3 + 1] = static_cast<unsigned char>(_data[i].g * 255);
        pixelData[i * 3 + 2] = static_cast<unsigned char>(_data[i].b * 255);
    }

    file.write(reinterpret_cast<const char *>(pixelData.data()), pixelData.size());
    if (!file)
    {
        std::cerr << "Error writing pixel data to file." << std::endl;
    }
}

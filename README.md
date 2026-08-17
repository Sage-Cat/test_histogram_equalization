# Histogram Equalization Demo

[![CI](https://github.com/Sage-Cat/test_histogram_equalization/actions/workflows/ci.yml/badge.svg)](https://github.com/Sage-Cat/test_histogram_equalization/actions/workflows/ci.yml)

A compact C++17 command-line demonstration of global histogram equalization.
It reads an 8-bit binary PPM image (`P6`), equalizes luminance within a region,
retains the two chroma components, and writes another `P6` image. Processing
luminance instead of the three RGB channels independently reduces color shifts.
The algorithm builds a 256-bin luminance cumulative distribution and remaps the
darkest and brightest occupied levels across the available output range.

## Build and test

Prerequisites are a C++17 compiler, CMake 3.20 or newer, and optionally Ninja.
Ubuntu 24.04 is continuously tested.

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

Tests generate their PPM fixtures at runtime; no sample photographs are needed.

## Usage

```sh
./build/histogram_equalize input.ppm output.ppm
./build/histogram_equalize input.ppm output.ppm reference.ppm
```

The optional reference comparison reports maximum and mean absolute channel
errors on the normalized `[0, 1]` scale. It exits unsuccessfully when the
maximum error exceeds `0.005` (0.5%). Processing and overall elapsed times are
also printed.

## Limitations

- Only 8-bit RGB `P6` PPM files are accepted; JPEG, grayscale PPM, and higher
  bit depths are deliberately outside this small demo.
- Equalization uses an 8-bit luminance histogram and standard BT.601-style
  RGB/luminance coefficients. Conversion and gamut clipping can slightly alter
  highly saturated colors.
- This is global equalization, not adaptive histogram equalization (CLAHE).

## License

The original project code is available under the [MIT License](LICENSE).

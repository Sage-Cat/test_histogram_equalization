## Program Description

In the `src` folder, there is the code of a program that can:

- Open an image in jpg or ppm p6 binary format.
- Perform histogram equalization.
- Save the result in ppm p6 binary format.
- Compare differences between two photographs saved in ppm p6 binary format.

## Examples of Program Operation

**Open an image, apply the effect, save the result:**

```bash
./histo_equalis_test input.jpg output.ppm
```

**Open an image, apply the effect, save the result and compare it with a reference:**

```bash
./histo_equalis_test input.jpg output.ppm reference.ppm
```

## Task Objective

The task is aimed at improving the current code in terms of performance as much as possible. You can use everything available in the standard C++14 library, multithreading, as well as SIMD instructions up to the AVX version. The result should be as identical as possible to the base. A difference of no more than half a percent per pixel per channel is allowed.

### Key Areas for Optimization

- **Opening:** Excluding the work of the external library.
- **Processing:** This is the most important part.
- **Saving:** Ensuring efficient saving of the processed image.

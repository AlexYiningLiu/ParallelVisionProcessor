# ParallelVisionProcessor

A C++20 experiment comparing single-threaded vs multi-threaded image processing performance.

## Overview

This project demonstrates how dividing an image into regions for parallel processing affects performance compared to single-threaded processing. It implements several threading strategies using modern C++20 features.

## Building the Project

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## Usage

```bash
ParallelVisionProcessor <image_path> [num_threads] [threading_strategy]
```

Parameters:
- `image_path`: Path to the input image
- `num_threads`: Number of threads to use (default: CPU core count)
- `threading_strategy`: `threadpool`, `async`, or `jthread` (default: threadpool)

## Examples

```bash
# Process with default settings
ParallelVisionProcessor image.jpg

# Process with 8 threads using thread pool
ParallelVisionProcessor image.jpg 8 threadpool

# Process with 12 threads using async
ParallelVisionProcessor image.jpg 12 async

# Process with 16 threads using jthread
ParallelVisionProcessor image.jpg 16 jthread
```

## Requirements

- C++20 compatible compiler (MSVC, GCC, Clang)
- OpenCV 4.x
- CMake 3.14+

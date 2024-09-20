# Compression Handlers Library

This library provides a set of handlers for working with compressed files, including support for Gzip and Zstd compression formats, as well as XRootD file access.

## Features

- Gzip compression and decompression
- Zstd compression and decompression
- XRootD file handling
- Local file handling

## Requirements

- CMake (3.14 or higher)
- C++17 compatible compiler
- ZLIB library
- ZSTD library
- XRootD library
- Google Test (for running tests)

## Building the Library

1. Clone the repository:
   ```
   git clone https://github.com/zhipzhang/compression-handlers.git
   cd compression-handlers
   ```

2. Create a build directory and navigate to it:
   ```
   mkdir build && cd build
   ```

3. Run CMake and build the project:
   ```
   cmake ..
   make
   ```

## Running Tests

After building the project, you can run the tests using: ctest

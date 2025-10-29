# Image Manipulation Program - User Guide

## Overview
This program provides command-line image manipulation tools using OpenCV.

## System Requirements
- **Operating System**: Built on macOS Sequoia. Windows/Linux users must rebuild from source.
- **Dependencies**: OpenCV 4.x, CMake 3.10+, C++11 compiler

## Installation & Build
1. Install OpenCV: `brew install opencv` (macOS) or equivalent for your OS
2. Install CMake: `brew install cmake` (macOS)
3. Build:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

## Usage
```bash
./ImageManipulation <option> <input_image_path>
```

### Available Options
- `-rgb2gray <image>`: Convert color image to grayscale
- `-brightness <image>`: Adjust brightness (interactive GUI)
- `-contrast <image>`: Adjust contrast (interactive GUI)
- `-avg <image>`: Apply average blur filter (interactive GUI)
- `-gauss <image>`: Apply Gaussian blur filter (interactive GUI)

## Examples
```bash
./ImageManipulation -rgb2gray input.jpg
./ImageManipulation -brightness input.jpg
```

## Output
- RGB to grayscale: Saves `output_rgb2gray.jpg`
- Other operations: Interactive windows (close window to exit)

## Troubleshooting
- Ensure input image exists and is valid format (JPG, PNG, etc.)
- On Windows/Linux: Rebuild from Sources/ directory
- GUI operations require display server
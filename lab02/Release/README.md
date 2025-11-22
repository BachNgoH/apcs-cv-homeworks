# Exercise A - Harris Corner Detection

## Executable Information

**Program Name:** exercise_a  
**Version:** 1.0  
**Build Date:** November 22, 2025  
**Platform:** macOS (compatible with Linux and Windows with recompilation)

## Quick Start

### Running with an Image File:
```bash
./exercise_a path/to/image.jpg
```

### Running with Camera:
```bash
./exercise_a
```

## System Requirements

### Required Dependencies:
- **OpenCV 4.x** - Computer Vision library

### Installation Instructions:

**macOS:**
```bash
brew install opencv
```

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install libopencv-dev
```

**Windows:**
1. Download OpenCV from https://opencv.org
2. Extract to C:\opencv
3. Add C:\opencv\build\x64\vc15\bin to PATH

## Features

- ✓ Harris corner detection algorithm
- ✓ Real-time camera processing
- ✓ Static image processing
- ✓ Interactive parameter adjustment via trackbars
- ✓ Keyboard shortcuts for quick actions
- ✓ Works with any image size
- ✓ Help system built-in

## Controls

### Keyboard Shortcuts:
- **h** or **H** - Display help
- **r** or **R** - Reset to original image
- **s** or **S** - Save current result
- **q** or **Q** or **ESC** - Quit program

### Adjustable Parameters (Trackbars):
- **Block Size** (2-10): Neighborhood window size
- **Aperture Size** (0-7): Sobel operator aperture (maps to 3, 5, 7)
- **K Value** (1-10): Harris detector sensitivity (×100)
- **Threshold** (0-255): Corner detection threshold

## Usage Examples

### Example 1: Process a checkerboard image
```bash
./exercise_a checkerboard.jpg
```

### Example 2: Live camera corner detection
```bash
./exercise_a
# Press 's' to save frames
# Adjust trackbars for optimal detection
```

## Recommended Parameter Settings

| Image Type | Block Size | Aperture | K Value | Threshold |
|------------|------------|----------|---------|-----------|
| Checkerboard | 2 | 0 (→3) | 4 (0.04) | 200 |
| Text/Book | 3 | 1 (→5) | 5 (0.05) | 180 |
| Building | 4 | 1 (→5) | 6 (0.06) | 150 |
| Texture | 3 | 1 (→5) | 5 (0.05) | 170 |

## Output Files

When you press 's' to save:
- **From file mode:** `harris_result.jpg`
- **From camera mode:** `harris_result_camera.jpg`

## Troubleshooting

### "Error: Could not open or find the image"
- Check that the image file path is correct
- Ensure the image file exists and is readable
- Try using absolute path instead of relative path

### "Error: Could not open camera"
- Check camera permissions in System Preferences (macOS)
- Ensure no other application is using the camera
- Try camera index 1 if 0 doesn't work (modify source code)

### "Library not loaded: @rpath/libopencv"
- OpenCV is not installed or not in library path
- Run: `brew install opencv` (macOS)
- Check OpenCV installation with: `pkg-config --modversion opencv4`

### Program runs slowly
- Reduce image size for faster processing
- Lower aperture size for faster gradient computation
- Use smaller block size

## Technical Information

**Algorithm:** Harris Corner Detection (1988)  
**Language:** C++11  
**Libraries:** OpenCV 4.x  
**License:** Educational Use

## Support

For detailed information about the algorithm and implementation, see:
- `../Document/Exercise_A_Report.md`
- `../Source Code/exercise_a.cpp`

---

**END OF README**

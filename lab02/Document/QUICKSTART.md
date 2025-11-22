# Harris Corner Detection - Exercise A

## Quick Start Guide

This guide will help you get started with testing the Harris corner detection implementation.

## Project Structure

```
lab02/
├── Document/
│   └── Exercise_A_Report.md      # Comprehensive technical report
├── Source Code/
│   └── exercise_a.cpp             # Main implementation
├── Release/
│   ├── exercise_a                 # Compiled executable
│   ├── README.md                  # Usage documentation
│   └── dll_requirements.txt       # Dependency list
├── Makefile                       # Build configuration
└── test.sh                        # Interactive test script
```

## Building the Program

### Method 1: Using Makefile (Recommended)
```bash
make
```

### Method 2: Manual Compilation
```bash
g++ -std=c++11 -Wall "Source Code/exercise_a.cpp" -o Release/exercise_a `pkg-config --cflags --libs opencv4`
```

### Clean Build
```bash
make clean
make
```

## Running the Program

### Option 1: Interactive Test Script
```bash
./test.sh
```
Follow the prompts to select camera or file mode.

### Option 2: Direct Execution

**Camera Mode:**
```bash
./Release/exercise_a
```

**File Mode:**
```bash
./Release/exercise_a path/to/image.jpg
```

## Testing Checklist

### ✓ Basic Functionality Tests

- [ ] Test with different image sizes:
  - Small (320×240)
  - Medium (640×480)
  - Large (1920×1080)
  - Very large (4000×3000)

- [ ] Test with camera:
  - [ ] Launch camera mode
  - [ ] Adjust parameters with trackbars
  - [ ] Save frame with 's' key
  - [ ] Exit with 'q' or ESC

- [ ] Test keyboard controls:
  - [ ] Press 'h' to show help
  - [ ] Press 'r' to reset
  - [ ] Press 's' to save
  - [ ] Press 'q' to quit

### ✓ Parameter Tuning Tests

Test different parameter combinations on various image types:

**Checkerboard Pattern:**
- Block Size: 2
- Aperture: 0 (maps to 3)
- K Value: 4 (0.04)
- Threshold: 200

**Text/Documents:**
- Block Size: 3
- Aperture: 1 (maps to 5)
- K Value: 5 (0.05)
- Threshold: 180

**Building/Architecture:**
- Block Size: 4
- Aperture: 1 (maps to 5)
- K Value: 6 (0.06)
- Threshold: 150

### ✓ Performance Tests

For your report, gather test data:

1. **5 Objects × 3 Images Each = 15 Total Images**

   Suggested objects:
   - Checkerboard pattern (calibration target)
   - Book cover or magazine
   - Computer keyboard
   - Building corner/facade
   - Textured surface (brick, wood, fabric)

2. **Record for each test:**
   - Number of corners detected
   - Optimal parameters used
   - Processing time
   - Visual quality assessment
   - False positives/negatives

3. **Evaluation metrics:**
   - Detection accuracy
   - Repeatability across views
   - Sensitivity to lighting
   - Sensitivity to scale
   - Robustness to noise

## Expected Output

When running correctly, you should see:
1. Window titled "Harris Corner Detection"
2. Red circles marking detected corners
3. Green text showing corner count
4. Yellow text showing current parameters
5. Trackbars for real-time adjustment

## Sample Commands for Report

```bash
# Test 1: Checkerboard
./Release/exercise_a test_images/object1_checkerboard/image1.jpg

# Test 2: Different sizes
./Release/exercise_a small_image.jpg
./Release/exercise_a large_image.jpg

# Test 3: Camera mode
./Release/exercise_a
```

## Gathering Test Data for Report

### Step 1: Collect Images
Create a test_images directory:
```
test_images/
├── object1_checkerboard/
│   ├── view1.jpg
│   ├── view2.jpg
│   └── view3.jpg
├── object2_book/
│   ├── view1.jpg
│   ├── view2.jpg
│   └── view3.jpg
└── ... (3 more objects)
```

### Step 2: Document Results
For each image, record:
- Image filename
- Image size (WxH)
- Optimal parameters
- Corners detected
- Processing time (approximate)
- Comments on accuracy

### Step 3: Capture Screenshots
Press 's' to save the processed results for inclusion in your report.

## Troubleshooting

### Build Errors

**"Package opencv4 was not found"**
```bash
# macOS
brew install opencv

# Ubuntu
sudo apt-get install libopencv-dev
```

**"Command not found: pkg-config"**
```bash
# macOS
brew install pkg-config
```

### Runtime Errors

**"Could not open camera"**
- Grant camera permissions
- Close other apps using camera
- Try a different camera index (modify source code)

**"Could not open image"**
- Check file path
- Verify image format (JPG, PNG supported)
- Use absolute path

### Performance Issues

**Slow processing on large images**
- Resize images to ~1024×768 for testing
- Reduce aperture size
- Use smaller block size

## Report Writing Tips

Your report should include:

1. **Algorithm Description**
   - Mathematical foundation
   - Harris corner detection theory
   - Parameter explanations

2. **Implementation Details**
   - Code structure
   - OpenCV functions used
   - Design decisions

3. **Experimental Results**
   - Test data description (5 objects × 3 images)
   - Parameter tuning results
   - Performance measurements
   - Sample output images

4. **Evaluation**
   - Quantitative analysis (corner count, processing time)
   - Qualitative analysis (visual quality, accuracy)
   - Comparison across different object types

5. **Strengths and Weaknesses**
   - What works well (geometric patterns, corners)
   - What doesn't work well (smooth regions, noise)
   - Limitations (not scale-invariant)
   - Comparison with other detectors

## Additional Resources

- Full documentation: `Document/Exercise_A_Report.md`
- Source code: `Source Code/exercise_a.cpp`
- Release notes: `Release/README.md`

## Contact

For questions about this implementation, refer to the comprehensive report in the Document folder.

---

**Good luck with your testing and evaluation!**

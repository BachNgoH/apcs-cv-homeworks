# Test Images for Harris Corner Detection

This folder contains sample images for testing the Harris corner detection implementation.

## Available Test Images

### 1. checkerboard.png
- **Source**: OpenCV official samples
- **Type**: Calibration pattern
- **Size**: ~27 KB
- **Best for**: Testing corner detection accuracy
- **Expected corners**: 48 internal corners (7×7 grid)
- **Recommended parameters**:
  - Block Size: 2
  - Aperture: 0 (→3)
  - K Value: 4 (0.04)
  - Threshold: 200

### 2. building.jpg
- **Source**: OpenCV official samples
- **Type**: Architecture/outdoor scene
- **Size**: ~78 KB
- **Best for**: Testing on real-world images
- **Expected**: Multiple structural corners
- **Recommended parameters**:
  - Block Size: 4
  - Aperture: 1 (→5)
  - K Value: 6 (0.06)
  - Threshold: 150

## Running Tests

### Test 1: Checkerboard Pattern
```bash
./Release/exercise_a test_images/checkerboard.png
```
This should detect all internal corners of the checkerboard pattern.

### Test 2: Building
```bash
./Release/exercise_a test_images/building.jpg
```
This tests detection on a real-world scene with various architectural features.

## Quick Test Script

Run both tests automatically:
```bash
# Test checkerboard
echo "Testing checkerboard pattern..."
./Release/exercise_a test_images/checkerboard.png

# Test building
echo "Testing building image..."
./Release/exercise_a test_images/building.jpg
```

## Adding Your Own Test Images

To add more test images for the assignment (5 objects × 3 images):

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
└── ...
```

---

**Note**: These sample images are from the OpenCV repository and are free to use for testing and educational purposes.

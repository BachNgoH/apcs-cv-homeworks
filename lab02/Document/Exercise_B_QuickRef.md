# Exercise B - Blob Detection

## Quick Reference

**Program:** exercise_b  
**Algorithm:** SimpleBlobDetector (OpenCV)  
**Purpose:** Detect blob-like circular regions in images

---

## Quick Start

### Run with Image
```bash
./Release/exercise_b test_images/checkerboard.png
./Release/exercise_b test_images/building.jpg
```

### Run with Camera
```bash
./Release/exercise_b
```

---

## Controls

### Keyboard
- **h** - Show help
- **r** - Reset
- **s** - Save result
- **c** - Toggle blob color (dark ⇄ bright)
- **q** - Quit

### Trackbars (6 Parameters)
1. **Min Threshold** (0-255): Lower threshold for detection
2. **Max Threshold** (0-255): Upper threshold for detection
3. **Min Area** (0-5000): Minimum blob size in pixels
4. **Circularity %** (0-100): How circular (100% = perfect circle)
5. **Convexity %** (0-100): How convex (smooth edges)
6. **Inertia %** (0-100): How round vs elongated

---

## Recommended Settings

### For Coins/Circular Objects
- Min Threshold: 10
- Max Threshold: 200
- Min Area: 500
- Circularity: 80%
- Convexity: 85%
- Inertia: 60%

### For Balls/Spheres
- Min Threshold: 30
- Max Threshold: 180
- Min Area: 1000
- Circularity: 70%
- Convexity: 80%
- Inertia: 50%

### For General Round Objects
- Min Threshold: 10
- Max Threshold: 200
- Min Area: 300
- Circularity: 60%
- Convexity: 70%
- Inertia: 40%

---

## What Are Blobs?

**Blob** = A region of connected pixels with similar properties (brightness/color) that differs from the background.

**SimpleBlobDetector** finds circular/blob-like regions by:
1. Converting image to multiple binary versions (thresholding)
2. Finding connected components in each
3. Grouping similar components across thresholds
4. Filtering by size, shape, and other properties

---

## Tips

### If NO blobs are detected:
- ✓ Lower all filter percentages
- ✓ Press 'c' to toggle dark/bright blob detection
- ✓ Reduce Min Area
- ✓ Check image has circular objects

### If TOO MANY false positives:
- ✓ Increase Circularity %
- ✓ Increase Convexity %
- ✓ Raise Min Area
- ✓ Narrow threshold range

### Best Results:
- Use images with clear circular objects
- Good lighting and contrast
- Objects well-separated (not touching)
- Adjust "Min Area" to match object size

---

## Visualization

Detected blobs are shown as:
- **Red circles** around each blob
- **Circle size** matches blob diameter
- **Green text** shows blob count
- **Yellow text** shows current parameters

---

## Documentation

- Full technical report: `../Document/Exercise_B_Report.md`
- Source code: `../Source Code/exercise_b.cpp`

---

**Best For:** Coins, balls, circular markers, eyes, circular objects  
**Not Good For:** Corners, edges, irregular shapes, overlapping objects

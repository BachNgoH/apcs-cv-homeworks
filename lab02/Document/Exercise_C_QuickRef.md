# DoG (Difference of Gaussians) Detection - Exercise C

## Quick Reference

**Program:** exercise_c  
**Algorithm:** DoG (Difference of Gaussians) / SIFT  
**Purpose:** Detect scale-invariant keypoints for feature matching

---

## Quick Start

### Run with Image
```bash
./Release/exercise_c test_images/building.jpg
./Release/exercise_c test_images/checkerboard.png
```

### Run with Camera
```bash
./Release/exercise_c
```

---

## Controls

### Keyboard
- **h** - Show help
- **r** - Reset
- **s** - Save result
- **q** - Quit

### Trackbars (5 Parameters)
1. **Max Features** (0-5000): Maximum keypoints to detect (0=unlimited)
2. **Octave Layers** (0-8): Layers per octave in scale-space pyramid
3. **Contrast x100** (0-20): Contrast threshold (4 = 0.04, filters weak features)
4. **Edge Threshold** (0-20): Edge response threshold (10 = default, filters edges)
5. **Sigma x10** (0-30): Initial Gaussian sigma (16 = 1.6, smoothing level)

---

## What is DoG?

**DoG (Difference of Gaussians)** is the foundation of SIFT (Scale-Invariant Feature Transform):

### How It Works:
1. **Scale-Space Pyramid**: Creates multiple versions of the image at different blur levels (scales)
2. **Compute Differences**: Subtracts adjacent blurred images to get DoG images
3. **Find Extrema**: Looks for local maxima/minima in DoG space
4. **Filter Keypoints**: Removes low-contrast and edge responses
5. **Assign Orientation**: Computes dominant direction for rotation invariance

### Why DoG?
- **Scale-Invariant**: Finds same features at different zoom levels
- **Rotation-Invariant**: Robust to image rotation
- **Distinctive**: Keypoints are repeatable and matchable
- **Proven**: Foundation of SIFT, one of the most reliable feature detectors

---

## Recommended Settings

### For General Images (Default)
- Max Features: 0 (unlimited)
- Octave Layers: 3
- Contrast x100: 4 (0.04)
- Edge Threshold: 10
- Sigma x10: 16 (1.6)

### For Dense Features (Many Keypoints)
- Max Features: 0
- Octave Layers: 4-5
- Contrast x100: 2-3 (lower = more features)
- Edge Threshold: 10
- Sigma x10: 16

### For Sparse Features (Few Strong Keypoints)
- Max Features: 100-500
- Octave Layers: 3
- Contrast x100: 8-10 (higher = fewer, stronger features)
- Edge Threshold: 15
- Sigma x10: 16

### For Fast Processing
- Max Features: 500-1000
- Octave Layers: 2-3
- Contrast x100: 5
- Edge Threshold: 10
- Sigma x10: 16

---

## Parameter Explanations

### Max Features
- **0**: Detect as many as possible (can be hundreds or thousands)
- **500-1000**: Good balance for most applications
- **100-200**: Sparse keypoints for specific matching

### Octave Layers
- **2-3**: Faster, fewer scale levels
- **4-5**: More thorough, better scale invariance
- **Higher values**: Slower but more robust to scale changes

### Contrast Threshold (x100)
- **Low (2-3)**: More keypoints, including weaker ones
- **Medium (4-6)**: Balanced (default=4)
- **High (8-15)**: Only very strong, high-contrast features

### Edge Threshold
- **Low (5-8)**: Accept more edge-like features
- **Medium (10)**: Default, good balance
- **High (15-20)**: Reject edges more aggressively

### Sigma (x10)
- **Low (10-14)**: Less initial smoothing
- **Medium (16)**: Default (1.6)
- **High (20-25)**: More smoothing, larger-scale features

---

## Visualization

Detected keypoints are shown as:
- **Red circles** around each keypoint
- **Circle size** indicates the scale (larger = detected at coarser scale)
- **Line direction** shows dominant orientation (if visible)
- **Green text** shows total keypoint count
- **Yellow text** shows current parameters

---

## Typical Applications

### ✅ Great For:
- **Image Matching**: Find corresponding points in different views
- **Object Recognition**: Match object features despite scale/rotation
- **Panorama Stitching**: Align images for panoramas
- **3D Reconstruction**: Structure from motion
- **Tracking**: Follow features across video frames
- **Image Registration**: Align medical images, satellite photos

### ❌ Not Ideal For:
- Real-time high-FPS applications (relatively slow)
- Low-texture images (few features to detect)
- Very repetitive patterns (many similar keypoints)

---

## Tips for Best Results

### If TOO FEW keypoints:
- ✓ Lower **Contrast x100** (e.g., 2-3)
- ✓ Increase **Octave Layers** (e.g., 4-5)
- ✓ Set **Max Features** to 0 (unlimited)
- ✓ Lower **Edge Threshold** (e.g., 8)

### If TOO MANY keypoints:
- ✓ Increase **Contrast x100** (e.g., 6-10)
- ✓ Set **Max Features** limit (e.g., 500)
- ✓ Decrease **Octave Layers** (e.g., 2-3)
- ✓ Increase **Edge Threshold** (e.g., 15)

### For Better Performance:
- ✓ Limit **Max Features** to what you need
- ✓ Use fewer **Octave Layers**
- ✓ Process smaller images

---

## Comparison with Other Detectors

| Feature | DoG/SIFT | Harris | Blob | FAST |
|---------|----------|--------|------|------|
| Scale Invariant | ✓ | ✗ | ✗→✓ | ✗ |
| Rotation Invariant | ✓ | ✓ | ✗ | ✗ |
| Speed | Slow | Fast | Fast | Very Fast |
| Descriptor | Built-in | None | None | None |
| Use Case | Matching | Corners | Circles | Tracking |

---

## Important Notes

### OpenCV Requirements
DoG/SIFT requires **opencv_contrib** (xfeatures2d module):

```bash
# macOS
brew install opencv

# The opencv formula includes contrib modules by default
```

If you get an error about missing SIFT, you need opencv_contrib installed.

### SIFT Patent
SIFT was patented but the patent **expired in 2020**. It's now free to use!

---

## Documentation

- Full technical report: `../Document/Exercise_C_Report.md` (TODO)
- Source code: `../Source Code/exercise_c.cpp`

---

**Perfect For:** Feature matching, panoramas, object recognition, 3D reconstruction  
**Key Advantage:** Scale and rotation invariant - finds same features at different zoom/rotation

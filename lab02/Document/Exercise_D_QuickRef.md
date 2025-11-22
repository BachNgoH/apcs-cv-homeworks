# Exercise D - Harris + SIFT Feature Matching

## Quick Reference

**Program:** exercise_d  
**Algorithm:** Harris corners + SIFT descriptors + Feature matching  
**Purpose:** Match features between two images of the same scene

---

## Quick Start

### Run with Two Images
```bash
./Release/exercise_d test_images/pisa_1.png test_images/pisa_2.png
./Release/exercise_d image1.jpg image2.jpg
```

**Note:** Requires TWO image arguments!

---

## Controls

### Keyboard
- **h** - Show help
- **r** - Reset parameters
- **s** - Save result
- **q** - Quit

### Trackbars (5 Parameters)

**Harris Detection:**
1. **Block Size** (0-10): Harris neighborhood size
2. **Aperture Size** (0-7): Sobel operator aperture
3. **K Value x100** (0-10): Harris k parameter
4. **Threshold** (0-255): Corner strength threshold

**Matching:**
5. **Match Ratio x100** (0-100): Lowe's ratio test (75 = 0.75)

---

## What is Harris + SIFT Matching?

This combines **two powerful algorithms**:

### 1. Harris Corner Detection (Keypoint Detection)
- Finds **corners** and **interest points** in images
- Fast and efficient
- Good localization

### 2. SIFT Descriptors (Feature Description)
- Computes **128-dimensional descriptor** at each corner
- **Scale-invariant** - robust to zoom changes
- **Rotation-invariant** - robust to rotation
- Highly distinctive for matching

### 3. Feature Matching
- Compares descriptors between two images
- Uses **Lowe's ratio test** to filter ambiguous matches
- Finds corresponding points in different views

---

## How It Works

```
Image 1:
1. Detect Harris corners → Get keypoint locations
2. Compute SIFT descriptors at those locations
3. Match descriptors with Image 2

Image 2:
1. Detect Harris corners → Get keypoint locations  
2. Compute SIFT descriptors at those locations
3. Match descriptors with Image 1

Result:
- Lines connecting matched features
- Shows correspondence between images
```

---

## Recommended Settings

### Default Settings (Good Starting Point)
- Block Size: 2
- Aperture Size: 3 (maps to aperture 9)
- K Value x100: 4 (0.04)
- Threshold: 200
- Match Ratio x100: 75 (0.75)

### For More Matches
- **Lower Threshold**: 150-180 (more corners)
- **Higher Match Ratio**: 80-90 (accept more matches)
- Block Size: 2-3

### For Better Quality Matches
- **Higher Threshold**: 220-240 (stronger corners only)
- **Lower Match Ratio**: 60-70 (stricter matching)
- Increase K value slightly

### For Dense Features
- Lower Threshold: 100-150
- Smaller Block Size: 2
- Match Ratio: 75

---

## Visualization

The result shows:
- **Left side**: Image 1 with keypoints
- **Right side**: Image 2 with keypoints
- **Green lines**: Matched features connecting the two images
- **Text overlay**: Match count and parameters

**Green text** shows:
- Total matches found
- Keypoints detected in each image

**Yellow text** shows:
- Current Harris and matching parameters

---

## Typical Applications

### ✅ Perfect For:
- **Panorama Stitching**: Match features to align images
- **Image Registration**: Align medical scans, satellite images
- **Object Recognition**: Find object in different views
- **3D Reconstruction**: Match points for stereo vision
- **Image Mosaicing**: Combine multiple images
- **Camera Calibration**: Find corresponding points

### ✅ Best When:
- Same scene from different angles
- Different lighting conditions
- Partial overlap between images
- Scale differences (zoom in/out)

### ❌ Not Ideal For:
- Completely different scenes
- Very repetitive patterns (many false matches)
- Motion blur or poor image quality
- No overlapping content

---

## Tips for Best Results

### If TOO FEW matches:
- ✓ **Lower Harris Threshold** (150-180)
- ✓ **Increase Match Ratio** (80-85)
- ✓ Check images have overlapping content
- ✓ Ensure images are of same scene

### If TOO MANY matches (including wrong ones):
- ✓ **Increase Harris Threshold** (220-240)
- ✓ **Decrease Match Ratio** (60-70)
- ✓ Use stronger k value (5-6)

### If matches look WRONG:
- ✓ Check images are related (same scene)
- ✓ Lower Match Ratio for stricter filtering (65-70)
- ✓ Increase Harris Threshold for stronger corners

### For FASTER processing:
- ✓ Increase Threshold (fewer keypoints)
- ✓ Process smaller images
- ✓ Limit keypoints detected

---

## Understanding the Ratio Test

**Match Ratio (Lowe's Ratio Test):**
- Compares distance to best match vs second-best match
- **0.75 (75)**: Default, good balance
- **Lower (0.60-0.70)**: Stricter, fewer but better matches
- **Higher (0.80-0.90)**: More permissive, more matches but some may be wrong

**Formula:** `if (dist[0] < ratio * dist[1]) → accept match`

---

## Comparison with Other Approaches

| Method | Detector | Descriptor | Speed | Quality |
|--------|----------|------------|-------|---------|
| **Harris + SIFT** | Fast | Excellent | Medium | Very Good |
| DoG + SIFT | Medium | Excellent | Slow | Excellent |
| ORB | Very Fast | Good | Very Fast | Good |
| SURF | Fast | Very Good | Fast | Very Good |

**Why Harris + SIFT?**
- Combines **fast detection** (Harris) with **robust description** (SIFT)
- Good compromise between speed and quality
- Harris finds well-localized corners
- SIFT provides distinctive descriptors

---

## Common Issues

### "No keypoints detected"
- Threshold too high
- Images too smooth/uniform
- Reduce threshold to 150 or lower

### "No matches found"
- Images don't overlap
- Different scenes
- Try increasing match ratio

### "Matches all over the place"
- Match ratio too high
- Images too similar (repetitive patterns)
- Lower match ratio to 0.65-0.70

---

## Example Usage

```bash
# Match two views of the same building
./Release/exercise_d building_left.jpg building_right.jpg

# Match same object at different scales
./Release/exercise_d object_near.jpg object_far.jpg

# Match rotated images
./Release/exercise_d original.jpg rotated.jpg

# Process the Pisa tower images
./Release/exercise_d test_images/pisa_1.png test_images/pisa_2.png
```

---

## Output

**Saved file:** `harris_sift_matches.jpg` (press 's')

The saved image shows both input images side-by-side with:
- Keypoints marked
- Match lines connecting corresponding points
- Statistics overlay

---

## Documentation

- Full technical report: `../Document/Exercise_D_Report.md` (TODO)
- Source code: `../Source Code/exercise_d.cpp`

---

**Perfect For:** Panoramas, 3D reconstruction, object recognition, image alignment  
**Key Advantage:** Combines fast corner detection with robust descriptors for reliable matching

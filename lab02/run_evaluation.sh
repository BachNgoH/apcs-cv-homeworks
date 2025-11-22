#!/bin/bash

# CV Lab 02 - Automated Algorithm Evaluation Script
# This script runs all detection and matching algorithms on test images
# and saves results to organized folders using the non-interactive cvlab_auto tool

echo "=========================================="
echo "CV Lab 02 - Automated Evaluation"
echo "=========================================="

# Create results directory structure
mkdir -p results/{eiffel_tower,pisa_tower,statue_liberty,big_ben,taj_mahal}/{harris,blob,dog,matching}

# Define test objects and images
OBJECTS=("eiffel_tower" "pisa_tower" "statue_liberty" "big_ben" "taj_mahal")

echo ""
echo "Phase 1: Running Detection Algorithms"
echo "--------------------------------------"

# Run detection algorithms on all test images
for obj in "${OBJECTS[@]}"; do
    echo "Processing: $obj"
    
    # Check if directory exists
    if [ ! -d "test_images/$obj" ]; then
        echo "  Warning: test_images/$obj not found, skipping..."
        continue
    fi
    
    # Process each image in the object folder
    for img in test_images/$obj/*.{jpg,png,jpeg}; do
        # Check if file exists (handles case where no images match)
        [ -e "$img" ] || continue
        
        filename=$(basename "$img")
        echo "  - Processing $filename"
        
        # Run Harris detection
        ./Release/cvlab_auto harris "$img" "results/$obj/harris/${filename%.*}_harris.jpg"
        
        # Run Blob detection
        ./Release/cvlab_auto blob "$img" "results/$obj/blob/${filename%.*}_blob.jpg"
        
        # Run DoG detection
        ./Release/cvlab_auto dog "$img" "results/$obj/dog/${filename%.*}_dog.jpg"
    done
done

echo ""
echo "Phase 2: Running Matching Algorithms"
echo "-------------------------------------"

# Run matching algorithms (compare first two images of each object)
for obj in "${OBJECTS[@]}"; do
    echo "Processing matches for: $obj"
    
    if [ ! -d "test_images/$obj" ]; then
        continue
    fi
    
    # Get first two images
    images=(test_images/$obj/*.{jpg,png,jpeg})
    if [ ${#images[@]} -lt 2 ]; then
        echo "  Warning: Need at least 2 images for matching, skipping..."
        continue
    fi
    
    img1="${images[0]}"
    img2="${images[1]}"
    
    if [ ! -f "$img1" ] || [ ! -f "$img2" ]; then
        continue
    fi
    
    echo "  Matching: $(basename "$img1") vs $(basename "$img2")"
    
    # Harris + SIFT
    ./Release/cvlab_auto m harris sift "$img1" "$img2" "results/$obj/matching/harris_sift.jpg"
    
    # DoG + SIFT  
    ./Release/cvlab_auto m dog sift "$img1" "$img2" "results/$obj/matching/dog_sift.jpg"
    
    # Blob + SIFT
    ./Release/cvlab_auto m blob sift "$img1" "$img2" "results/$obj/matching/blob_sift.jpg"
    
    # Harris + LBP
    ./Release/cvlab_auto m harris lbp "$img1" "$img2" "results/$obj/matching/harris_lbp.jpg"
    
    # DoG + LBP
    ./Release/cvlab_auto m dog lbp "$img1" "$img2" "results/$obj/matching/dog_lbp.jpg"
    
    # Blob + LBP
    ./Release/cvlab_auto m blob lbp "$img1" "$img2" "results/$obj/matching/blob_lbp.jpg"
done

echo ""
echo "=========================================="
echo "Evaluation Complete!"
echo "=========================================="
echo "Results saved to: results/"
echo ""
echo "Summary:"
find results -name "*.jpg" | wc -l | xargs echo "Total result images:"
echo ""
echo "Next steps:"
echo "1. Review results in results/ folder"
echo "2. Analyze algorithm performance"
echo "3. Document findings in reports"
echo "=========================================="

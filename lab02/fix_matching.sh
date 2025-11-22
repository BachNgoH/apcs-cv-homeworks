#!/bin/bash
# Fix missing matching results

run_matching() {
    obj=$1
    ext=$2
    echo "Processing matches for $obj (extension: $ext)..."
    
    # Find first two images with specific extension
    img1=$(find test_images/$obj -name "*.$ext" | sort | head -n 1)
    img2=$(find test_images/$obj -name "*.$ext" | sort | head -n 2 | tail -n 1)
    
    if [ -z "$img1" ] || [ -z "$img2" ]; then
        echo "  Could not find 2 images for $obj"
        return
    fi
    
    echo "  Matching $img1 vs $img2"
    
    ./Release/cvlab_auto m harris sift "$img1" "$img2" "results/$obj/matching/harris_sift.jpg"
    ./Release/cvlab_auto m dog sift "$img1" "$img2" "results/$obj/matching/dog_sift.jpg"
    ./Release/cvlab_auto m blob sift "$img1" "$img2" "results/$obj/matching/blob_sift.jpg"
    ./Release/cvlab_auto m harris lbp "$img1" "$img2" "results/$obj/matching/harris_lbp.jpg"
    ./Release/cvlab_auto m dog lbp "$img1" "$img2" "results/$obj/matching/dog_lbp.jpg"
    ./Release/cvlab_auto m blob lbp "$img1" "$img2" "results/$obj/matching/blob_lbp.jpg"
}

# Eiffel and Pisa are PNGs
run_matching "eiffel_tower" "png"
run_matching "pisa_tower" "png"

# Big Ben are JPGs
run_matching "big_ben" "png"

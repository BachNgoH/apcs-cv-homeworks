#!/bin/bash

# Harris Corner Detection - Quick Test Script
# This script demonstrates the usage of exercise_a

echo "=========================================="
echo "Harris Corner Detection - Test Script"
echo "=========================================="
echo ""

# Check if executable exists
if [ ! -f "./Release/exercise_a" ]; then
    echo "Error: exercise_a not found in Release folder"
    echo "Please run 'make' first to build the program"
    exit 1
fi

# Display menu
echo "Select test mode:"
echo "1. Test with camera (real-time)"
echo "2. Test with image file"
echo "3. Show help"
echo ""
read -p "Enter choice (1-3): " choice

case $choice in
    1)
        echo ""
        echo "Starting camera test..."
        echo "Controls: h=help, s=save, q=quit"
        echo ""
        ./Release/exercise_a
        ;;
    2)
        echo ""
        read -p "Enter image file path: " imgpath
        if [ -f "$imgpath" ]; then
            echo "Processing image: $imgpath"
            echo "Controls: h=help, r=reset, s=save, q=quit"
            echo ""
            ./Release/exercise_a "$imgpath"
        else
            echo "Error: Image file not found: $imgpath"
            exit 1
        fi
        ;;
    3)
        echo ""
        ./Release/exercise_a --help 2>&1 || ./Release/exercise_a
        ;;
    *)
        echo "Invalid choice"
        exit 1
        ;;
esac

echo ""
echo "Test completed"

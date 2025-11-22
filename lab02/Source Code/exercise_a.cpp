#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

// Global variables for trackbars
int blockSize = 2;
int apertureSize = 3;
int kValue = 4; // k * 100 (for 0.04)
int cornerThreshold = 200;
const int MAX_THRESHOLD = 255;
const int MAX_BLOCK_SIZE = 10;
const int MAX_APERTURE = 7;
const int MAX_K_VALUE = 10;

Mat srcImage, srcGray, dstImage, dstNorm, dstNormScaled;
const string windowName = "Harris Corner Detection";
bool fromCamera = false;

void showHelp() {
    cout << "\n===== HARRIS CORNER DETECTION - HELP =====" << endl;
    cout << "Usage: ./exercise_a [image_file]" << endl;
    cout << "\nDescription:" << endl;
    cout << "  Detects keypoints using Harris corner detection algorithm" << endl;
    cout << "  If no image file is provided, captures from camera" << endl;
    cout << "\nKeyboard Controls:" << endl;
    cout << "  'h' or 'H' - Show this help" << endl;
    cout << "  'r' or 'R' - Reset to original image" << endl;
    cout << "  's' or 'S' - Save current result" << endl;
    cout << "  'q' or 'Q' or ESC - Quit" << endl;
    cout << "\nTrackbar Parameters:" << endl;
    cout << "  Block Size    - Size of neighborhood considered for corner detection (2-10)" << endl;
    cout << "  Aperture Size - Aperture parameter for Sobel operator (3, 5, or 7)" << endl;
    cout << "  K Value       - Harris detector free parameter (0-1, displayed as 0-10)" << endl;
    cout << "  Threshold     - Threshold for corner detection (0-255)" << endl;
    cout << "\nAlgorithm:" << endl;
    cout << "  Harris Corner Detection identifies corners by analyzing" << endl;
    cout << "  intensity changes in all directions at each pixel." << endl;
    cout << "=========================================\n" << endl;
}

void harrisCornerDetection(int, void*) {
    if (srcGray.empty()) return;
    
    // Ensure aperture size is odd and valid (3, 5, or 7)
    int actualApertureSize = (apertureSize * 2) + 3; // Maps 0->3, 1->5, 2->7
    if (actualApertureSize > 7) actualApertureSize = 7;
    
    // Ensure block size is at least 2
    int actualBlockSize = blockSize;
    if (actualBlockSize < 2) actualBlockSize = 2;
    
    // Calculate k value (0.01 to 0.10)
    double k = (kValue + 1) / 100.0;
    
    // ========== MANUAL HARRIS IMPLEMENTATION ==========
    
    // Step 1: Compute image gradients using Sobel (Ix and Iy)
    Mat Ix, Iy;
    Sobel(srcGray, Ix, CV_32F, 1, 0, actualApertureSize);  // Gradient in X
    Sobel(srcGray, Iy, CV_32F, 0, 1, actualApertureSize);  // Gradient in Y
    
    // Step 2: Compute products of gradients
    Mat Ixx, Iyy, Ixy;
    multiply(Ix, Ix, Ixx);  // Ix²
    multiply(Iy, Iy, Iyy);  // Iy²
    multiply(Ix, Iy, Ixy);  // Ix·Iy
    
    // Step 3: Apply Gaussian smoothing to gradient products
    // This creates the structure tensor M at each pixel
    Mat Sxx, Syy, Sxy;
    int kernelSize = actualBlockSize * 2 + 1;  // Make it odd
    GaussianBlur(Ixx, Sxx, Size(kernelSize, kernelSize), 0);
    GaussianBlur(Iyy, Syy, Size(kernelSize, kernelSize), 0);
    GaussianBlur(Ixy, Sxy, Size(kernelSize, kernelSize), 0);
    
    // Step 4: Compute Harris corner response at each pixel
    // R = det(M) - k * trace(M)²
    // det(M) = Sxx * Syy - Sxy²
    // trace(M) = Sxx + Syy
    Mat harrisResponse(srcGray.size(), CV_32F);
    
    for (int i = 0; i < srcGray.rows; i++) {
        for (int j = 0; j < srcGray.cols; j++) {
            float sxx = Sxx.at<float>(i, j);
            float syy = Syy.at<float>(i, j);
            float sxy = Sxy.at<float>(i, j);
            
            // Compute determinant and trace
            float det = sxx * syy - sxy * sxy;
            float trace = sxx + syy;
            
            // Harris corner response
            float response = det - k * trace * trace;
            harrisResponse.at<float>(i, j) = response;
        }
    }
    
    // Step 5: Normalize the response for visualization
    normalize(harrisResponse, dstNorm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
    convertScaleAbs(dstNorm, dstNormScaled);
    
    // ========== END MANUAL IMPLEMENTATION ==========
    
    // Draw corners on the original image (OPTIMIZED)
    Mat resultImage = srcImage.clone();
    
    // Use thresholding instead of nested loops for better speedup
    Mat cornerMask;
    threshold(dstNorm, cornerMask, cornerThreshold, 255, THRESH_BINARY);
    cornerMask.convertTo(cornerMask, CV_8U);
    
    // Find corner locations efficiently
    vector<Point> corners;
    findNonZero(cornerMask, corners);
    int cornerCount = corners.size();
    
    // Draw corners (much faster with vector)
    for (size_t i = 0; i < corners.size(); i++) {
        circle(resultImage, corners[i], 5, Scalar(0, 0, 255), 2);
    }
    
    // Display corner count
    string info = "Corners detected: " + to_string(cornerCount);
    putText(resultImage, info, Point(10, 30), FONT_HERSHEY_SIMPLEX, 
            0.7, Scalar(0, 255, 0), 2);
    
    // Display parameters
    string params = "Block:" + to_string(actualBlockSize) + 
                   " Aperture:" + to_string(actualApertureSize) +
                   " K:" + to_string(k).substr(0, 4) +
                   " Thresh:" + to_string(cornerThreshold);
    putText(resultImage, params, Point(10, 60), FONT_HERSHEY_SIMPLEX, 
            0.5, Scalar(255, 255, 0), 1);
    
    imshow(windowName, resultImage);
}

void processImage(const Mat& img) {
    srcImage = img.clone();
    
    // Convert to grayscale
    if (srcImage.channels() == 3) {
        cvtColor(srcImage, srcGray, COLOR_BGR2GRAY);
    } else {
        srcGray = srcImage.clone();
    }
    
    // Perform Harris corner detection
    harrisCornerDetection(0, 0);
}

int main(int argc, char** argv) {
    showHelp();
    
    // Check if image file is provided
    if (argc > 1) {
        // Read image from file
        string imagePath = argv[1];
        Mat image = imread(imagePath, IMREAD_COLOR);
        
        if (image.empty()) {
            cerr << "Error: Could not open or find the image: " << imagePath << endl;
            return -1;
        }
        
        cout << "Image loaded: " << imagePath << endl;
        cout << "Image size: " << image.cols << "x" << image.rows << endl;
        
        fromCamera = false;
        
        // Create window and trackbars
        namedWindow(windowName, WINDOW_AUTOSIZE);
        createTrackbar("Block Size", windowName, &blockSize, MAX_BLOCK_SIZE, harrisCornerDetection);
        createTrackbar("Aperture Size", windowName, &apertureSize, MAX_APERTURE, harrisCornerDetection);
        createTrackbar("K Value (x100)", windowName, &kValue, MAX_K_VALUE, harrisCornerDetection);
        createTrackbar("Threshold", windowName, &cornerThreshold, MAX_THRESHOLD, harrisCornerDetection);
        
        // Process the image
        processImage(image);
        
        // Wait for key press
        while (true) {
            int key = waitKey(30);
            if (key == 27 || key == 'q' || key == 'Q') { // ESC or q
                break;
            } else if (key == 'h' || key == 'H') {
                showHelp();
            } else if (key == 'r' || key == 'R') {
                cout << "Resetting to original image..." << endl;
                processImage(image);
            } else if (key == 's' || key == 'S') {
                string outputPath = "harris_result.jpg";
                Mat resultImage;
                Mat displayed = imread("temp.jpg"); // Get current display
                imwrite(outputPath, dstNormScaled);
                cout << "Result saved to: " << outputPath << endl;
            }
        }
        
    } else {
        // Capture from camera
        cout << "No image file provided. Attempting to capture from camera..." << endl;
        
        VideoCapture cap(0);
        if (!cap.isOpened()) {
            cerr << "Error: Could not open camera" << endl;
            return -1;
        }
        
        // Set camera resolution for better performance
        cap.set(CAP_PROP_FRAME_WIDTH, 640);
        cap.set(CAP_PROP_FRAME_HEIGHT, 480);
        cap.set(CAP_PROP_FPS, 30);
        
        cout << "Camera opened successfully. Press 'q' or ESC to quit." << endl;
        cout << "Resolution: " << cap.get(CAP_PROP_FRAME_WIDTH) << "x" 
             << cap.get(CAP_PROP_FRAME_HEIGHT) << endl;
        fromCamera = true;
        
        // Create window and trackbars
        namedWindow(windowName, WINDOW_AUTOSIZE);
        createTrackbar("Block Size", windowName, &blockSize, MAX_BLOCK_SIZE, harrisCornerDetection);
        createTrackbar("Aperture Size", windowName, &apertureSize, MAX_APERTURE, harrisCornerDetection);
        createTrackbar("K Value (x100)", windowName, &kValue, MAX_K_VALUE, harrisCornerDetection);
        createTrackbar("Threshold", windowName, &cornerThreshold, MAX_THRESHOLD, harrisCornerDetection);
        
        // Continuous capture and processing
        while (true) {
            Mat frame;
            cap >> frame;
            
            if (frame.empty()) {
                cerr << "Error: Could not capture frame" << endl;
                break;
            }
            
            processImage(frame);
            
            int key = waitKey(30);
            if (key == 27 || key == 'q' || key == 'Q') { // ESC or q
                break;
            } else if (key == 'h' || key == 'H') {
                showHelp();
            } else if (key == 's' || key == 'S') {
                string outputPath = "harris_result_camera.jpg";
                imwrite(outputPath, frame);
                cout << "Frame saved to: " << outputPath << endl;
            }
        }
        
        cap.release();
    }
    
    destroyAllWindows();
    cout << "Program terminated." << endl;
    
    return 0;
}

#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

// Global variables for trackbars
int minThreshold = 10;
int maxThreshold = 200;
int minArea = 100;
int minCircularity = 30; // 0-100 (percentage)
int minConvexity = 50;   // 0-100 (percentage)
int minInertia = 10;     // 0-100 (percentage)
int filterByColor = 1;   // 0 or 1
int blobColor = 255;     // 0 or 255

const int MAX_THRESHOLD = 255;
const int MAX_AREA = 5000;
const int MAX_PERCENTAGE = 100;

Mat srcImage, resultImage;
const string windowName = "Blob Detection";
bool fromCamera = false;

void showHelp() {
    cout << "\n===== BLOB DETECTION - HELP =====" << endl;
    cout << "Usage: ./exercise_b [image_file]" << endl;
    cout << "\nDescription:" << endl;
    cout << "  Detects keypoints using blob detection algorithm" << endl;
    cout << "  If no image file is provided, captures from camera" << endl;
    cout << "\nKeyboard Controls:" << endl;
    cout << "  'h' or 'H' - Show this help" << endl;
    cout << "  'r' or 'R' - Reset to original image" << endl;
    cout << "  's' or 'S' - Save current result" << endl;
    cout << "  'c' or 'C' - Toggle blob color (dark/bright)" << endl;
    cout << "  'q' or 'Q' or ESC - Quit" << endl;
    cout << "\nTrackbar Parameters:" << endl;
    cout << "  Min Threshold  - Minimum threshold for blob detection" << endl;
    cout << "  Max Threshold  - Maximum threshold for blob detection" << endl;
    cout << "  Min Area       - Minimum blob area in pixels" << endl;
    cout << "  Circularity    - How circular the blob must be (0-100%)" << endl;
    cout << "  Convexity      - How convex the blob must be (0-100%)" << endl;
    cout << "  Inertia Ratio  - Elongation measure (0-100%)" << endl;
    cout << "\nAlgorithm:" << endl;
    cout << "  SimpleBlobDetector identifies blobs by converting the image" << endl;
    cout << "  to binary using multiple thresholds and finding connected components." << endl;
    cout << "  Blobs are filtered based on area, circularity, convexity, and inertia." << endl;
    cout << "================================\n" << endl;
}

void blobDetection(int, void*) {
    if (srcImage.empty()) return;
    
    // Validate thresholds to prevent crash
    if (minThreshold >= maxThreshold) {
        maxThreshold = minThreshold + 10;
        if (maxThreshold > MAX_THRESHOLD) {
            maxThreshold = MAX_THRESHOLD;
            minThreshold = maxThreshold - 10;
        }
        setTrackbarPos("Max Threshold", windowName, maxThreshold);
    }
    
    // Ensure minimum difference between thresholds
    if (maxThreshold - minThreshold < 10) {
        maxThreshold = minThreshold + 10;
        if (maxThreshold > MAX_THRESHOLD) {
            maxThreshold = MAX_THRESHOLD;
            minThreshold = maxThreshold - 10;
        }
        setTrackbarPos("Max Threshold", windowName, maxThreshold);
    }
    
    // Set up blob detector parameters
    SimpleBlobDetector::Params params;
    
    // Thresholds
    params.minThreshold = minThreshold;
    params.maxThreshold = maxThreshold;
    params.thresholdStep = 10;
    
    // Filter by Area
    params.filterByArea = true;
    params.minArea = minArea;
    params.maxArea = 100000;
    
    // Filter by Circularity
    params.filterByCircularity = true;
    params.minCircularity = minCircularity / 100.0f;
    
    // Filter by Convexity
    params.filterByConvexity = true;
    params.minConvexity = minConvexity / 100.0f;
    
    // Filter by Inertia
    params.filterByInertia = true;
    params.minInertiaRatio = minInertia / 100.0f;
    
    // Filter by Color
    params.filterByColor = (filterByColor == 1);
    params.blobColor = blobColor;
    
    // Create detector
    Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
    
    // Detect blobs
    vector<KeyPoint> keypoints;
    detector->detect(srcImage, keypoints);
    
    // Draw detected blobs
    resultImage = srcImage.clone();
    drawKeypoints(resultImage, keypoints, resultImage, 
                  Scalar(0, 0, 255), 
                  DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    
    // Display blob count and info
    string info = "Blobs detected: " + to_string(keypoints.size());
    putText(resultImage, info, Point(10, 30), FONT_HERSHEY_SIMPLEX, 
            0.7, Scalar(0, 255, 0), 2);
    
    // Display parameters
    string params_str = "MinT:" + to_string(minThreshold) + 
                       " MaxT:" + to_string(maxThreshold) +
                       " Area:" + to_string(minArea);
    putText(resultImage, params_str, Point(10, 60), FONT_HERSHEY_SIMPLEX, 
            0.5, Scalar(255, 255, 0), 1);
    
    string filters = "Circ:" + to_string(minCircularity) + "% " +
                    "Conv:" + to_string(minConvexity) + "% " +
                    "Iner:" + to_string(minInertia) + "%";
    putText(resultImage, filters, Point(10, 85), FONT_HERSHEY_SIMPLEX, 
            0.5, Scalar(255, 255, 0), 1);
    
    imshow(windowName, resultImage);
}

void processImage(const Mat& img) {
    srcImage = img.clone();
    
    // Convert to grayscale if needed
    if (srcImage.channels() == 3) {
        cvtColor(srcImage, srcImage, COLOR_BGR2GRAY);
    }
    
    // Perform blob detection
    blobDetection(0, 0);
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
        createTrackbar("Min Threshold", windowName, &minThreshold, MAX_THRESHOLD, blobDetection);
        createTrackbar("Max Threshold", windowName, &maxThreshold, MAX_THRESHOLD, blobDetection);
        createTrackbar("Min Area", windowName, &minArea, MAX_AREA, blobDetection);
        createTrackbar("Circularity %", windowName, &minCircularity, MAX_PERCENTAGE, blobDetection);
        createTrackbar("Convexity %", windowName, &minConvexity, MAX_PERCENTAGE, blobDetection);
        createTrackbar("Inertia %", windowName, &minInertia, MAX_PERCENTAGE, blobDetection);
        
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
                string outputPath = "blob_result.jpg";
                imwrite(outputPath, resultImage);
                cout << "Result saved to: " << outputPath << endl;
            } else if (key == 'c' || key == 'C') {
                blobColor = (blobColor == 0) ? 255 : 0;
                cout << "Blob color toggled to: " << (blobColor == 0 ? "dark" : "bright") << endl;
                blobDetection(0, 0);
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
        createTrackbar("Min Threshold", windowName, &minThreshold, MAX_THRESHOLD, blobDetection);
        createTrackbar("Max Threshold", windowName, &maxThreshold, MAX_THRESHOLD, blobDetection);
        createTrackbar("Min Area", windowName, &minArea, MAX_AREA, blobDetection);
        createTrackbar("Circularity %", windowName, &minCircularity, MAX_PERCENTAGE, blobDetection);
        createTrackbar("Convexity %", windowName, &minConvexity, MAX_PERCENTAGE, blobDetection);
        createTrackbar("Inertia %", windowName, &minInertia, MAX_PERCENTAGE, blobDetection);
        
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
                string outputPath = "blob_result_camera.jpg";
                imwrite(outputPath, resultImage);
                cout << "Frame saved to: " << outputPath << endl;
            } else if (key == 'c' || key == 'C') {
                blobColor = (blobColor == 0) ? 255 : 0;
                cout << "Blob color toggled to: " << (blobColor == 0 ? "dark" : "bright") << endl;
            }
        }
        
        cap.release();
    }
    
    destroyAllWindows();
    cout << "Program terminated." << endl;
    
    return 0;
}

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
int maxThreshold = 220;
int minArea = 100;
int minCircularity = 4;  // 0-100 (percentage) - matches image
int minConvexity = 58;   // 0-100 (percentage) - matches image
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
    
    // ========== MANUAL MULTI-THRESHOLD BLOB DETECTION ==========
    
    // Keep color image for display
    Mat colorImage = srcImage.clone();
    
    // Step 1: Convert to grayscale for detection
    Mat gray;
    if (srcImage.channels() == 3) {
        cvtColor(srcImage, gray, COLOR_BGR2GRAY);
    } else {
        gray = srcImage.clone();
        cvtColor(gray, colorImage, COLOR_GRAY2BGR);
    }
    
    // Step 2: Multi-threshold detection (OPTIMIZED)
    vector<vector<Point2f>> allCenters;  // Centers at each threshold level
    int thresholdStep = 20;  // Increased from 10 for better performance
    
    cout << "Detecting blobs across " << ((maxThreshold - minThreshold) / thresholdStep) << " threshold levels..." << flush;
    
    for (int thresh = minThreshold; thresh < maxThreshold; thresh += thresholdStep) {
        // Apply threshold
        Mat binary;
        if (blobColor == 255) {
            threshold(gray, binary, thresh, 255, THRESH_BINARY);
        } else {
            threshold(gray, binary, thresh, 255, THRESH_BINARY_INV);
        }
        
        // Find contours
        vector<vector<Point>> contours;
        findContours(binary, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
        
        // Store centers of blobs at this threshold
        vector<Point2f> centersAtThisThreshold;
        
        for (size_t i = 0; i < contours.size(); i++) {
            double area = contourArea(contours[i]);
            if (area < minArea) continue;
            
            // Calculate center
            Moments m = moments(contours[i]);
            if (m.m00 == 0) continue;
            
            Point2f center(m.m10 / m.m00, m.m01 / m.m00);
            centersAtThisThreshold.push_back(center);
        }
        
        allCenters.push_back(centersAtThisThreshold);
    }
    
    // Step 3: Group blob centers across thresholds
    // Centers that are close together across thresholds are the same blob
    vector<vector<Point2f>> blobGroups;
    vector<bool> used(allCenters.size());
    
    for (size_t i = 0; i < allCenters.size(); i++) {
        for (size_t j = 0; j < allCenters[i].size(); j++) {
            Point2f center = allCenters[i][j];
            
            // Find if this center belongs to an existing group
            bool foundGroup = false;
            for (size_t g = 0; g < blobGroups.size(); g++) {
                // Check if close to any center in this group
                for (size_t k = 0; k < blobGroups[g].size(); k++) {
                    float dist = norm(center - blobGroups[g][k]);
                    if (dist < 5.0) {  // Distance threshold for grouping
                        blobGroups[g].push_back(center);
                        foundGroup = true;
                        break;
                    }
                }
                if (foundGroup) break;
            }
            
            // Create new group if not found
            if (!foundGroup) {
                vector<Point2f> newGroup;
                newGroup.push_back(center);
                blobGroups.push_back(newGroup);
            }
        }
    }
    
    // Step 4: Calculate final blob centers and filter
    vector<KeyPoint> keypoints;
    
    for (size_t g = 0; g < blobGroups.size(); g++) {
        if (blobGroups[g].size() < 2) continue;  // Require blob to appear in at least 2 thresholds
        
        // Calculate average center
        Point2f avgCenter(0, 0);
        for (size_t i = 0; i < blobGroups[g].size(); i++) {
            avgCenter += blobGroups[g][i];
        }
        avgCenter.x /= blobGroups[g].size();
        avgCenter.y /= blobGroups[g].size();
        
        // Get contour at middle threshold for filtering
        int midThresh = (minThreshold + maxThreshold) / 2;
        Mat binary;
        if (blobColor == 255) {
            threshold(gray, binary, midThresh, 255, THRESH_BINARY);
        } else {
            threshold(gray, binary, midThresh, 255, THRESH_BINARY_INV);
        }
        
        vector<vector<Point>> contours;
        findContours(binary, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
        
        // Find contour closest to avgCenter
        int bestContour = -1;
        double minDist = 1e9;
        
        for (size_t i = 0; i < contours.size(); i++) {
            Moments m = moments(contours[i]);
            if (m.m00 == 0) continue;
            
            Point2f contourCenter(m.m10 / m.m00, m.m01 / m.m00);
            double dist = norm(avgCenter - contourCenter);
            
            if (dist < minDist) {
                minDist = dist;
                bestContour = i;
            }
        }
        
        if (bestContour == -1 || minDist > 10.0) continue;
        
        // Apply shape filters
        double area = contourArea(contours[bestContour]);
        if (area < minArea) continue;
        
        double perimeter = arcLength(contours[bestContour], true);
        if (perimeter == 0) continue;
        
        double circularity = 4.0 * CV_PI * area / (perimeter * perimeter);
        if (circularity < minCircularity / 100.0f) continue;
        
        vector<Point> hull;
        convexHull(contours[bestContour], hull);
        double hullArea = contourArea(hull);
        if (hullArea == 0) continue;
        
        double convexity = area / hullArea;
        if (convexity < minConvexity / 100.0f) continue;
        
        Moments m = moments(contours[bestContour]);
        double denominator = sqrt(pow(2 * m.mu11, 2) + pow(m.mu20 - m.mu02, 2));
        double ratio = 1.0;
        
        if (denominator > 1e-2) {
            double cosmin = (m.mu20 - m.mu02) / denominator;
            double sinmin = 2 * m.mu11 / denominator;
            double imin = 0.5 * (m.mu20 + m.mu02) - 0.5 * (m.mu20 - m.mu02) * cosmin - m.mu11 * sinmin;
            double imax = 0.5 * (m.mu20 + m.mu02) + 0.5 * (m.mu20 - m.mu02) * cosmin + m.mu11 * sinmin;
            ratio = imin / imax;
        }
        
        if (ratio < minInertia / 100.0f) continue;
        
        // Create keypoint
        float radius = sqrt(area / CV_PI);
        keypoints.push_back(KeyPoint(avgCenter, radius * 2));
    }
    
    cout << " Done! Found " << keypoints.size() << " blobs." << endl;
    
    // ========== END MANUAL BLOB DETECTION ==========
    
    // Draw detected blobs on color image
    resultImage = colorImage.clone();
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
    
    // Perform blob detection (will handle grayscale conversion internally)
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

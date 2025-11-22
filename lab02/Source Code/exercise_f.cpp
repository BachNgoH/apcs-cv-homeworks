#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;

// Global variables for trackbars
int minThreshold = 10;
int maxThreshold = 220;
int minArea = 100;
int minCircularity = 4;  // Matches exercise_b defaults
int minConvexity = 58;   // Matches exercise_b defaults
int minInertia = 10;
int matchRatio = 75;

const int MAX_AREA = 5000;
const int MAX_PERCENTAGE = 100;
const int MAX_THRESHOLD = 255;
const int MAX_RATIO = 100;

Mat img1, img2, gray1, gray2;
Mat resultImage;
vector<KeyPoint> keypoints1, keypoints2;
vector<DMatch> goodMatches;
const string windowName = "Blob + SIFT Matching";

void showHelp() {
    cout << "\n===== BLOB + SIFT MATCHING - HELP =====" << endl;
    cout << "Usage: ./exercise_f image1.jpg image2.jpg" << endl;
    cout << "\nDescription:" << endl;
    cout << "  Detects keypoints using manual blob detector" << endl;
    cout << "  Computes SIFT descriptors at blob locations" << endl;
    cout << "  Matches features between two images" << endl;
    cout << "\nKeyboard Controls:" << endl;
    cout << "  'h' or 'H' - Show this help" << endl;
    cout << "  'r' or 'R' - Reset to original parameters" << endl;
    cout << "  's' or 'S' - Save current result" << endl;
    cout << "  'q' or 'Q' or ESC - Quit" << endl;
    cout << "\nTrackbar Parameters:" << endl;
    cout << "  Min Threshold  - Minimum threshold (0-255)" << endl;
    cout << "  Max Threshold  - Maximum threshold (0-255)" << endl;
    cout << "  Min Area       - Minimum blob area in pixels" << endl;
    cout << "  Circularity %  - How circular (0-100%)" << endl;
    cout << "  Convexity %    - How convex (0-100%)" << endl;
    cout << "  Threshold      - Binary threshold (0-255)" << endl;
    cout << "  Match Ratio x100 - Distance ratio test (75 = 0.75)" << endl;
    cout << "\nAlgorithm:" << endl;
    cout << "  1. Detect blobs (manual) in both images" << endl;
    cout << "  2. Compute SIFT descriptors at blob centers" << endl;
    cout << "  3. Match descriptors using ratio test" << endl;
    cout << "  4. Display matches between images" << endl;
    cout << "==========================================\n" << endl;
}

void matchFeatures(int, void*) {
    if (gray1.empty() || gray2.empty()) return;
    
    try {
        // Set up blob detector parameters
        SimpleBlobDetector::Params params;
        params.minThreshold = minThreshold;
        params.maxThreshold = maxThreshold;
        params.thresholdStep = 10;
        params.filterByArea = true;
        params.minArea = minArea;
        params.filterByCircularity = true;
        params.minCircularity = minCircularity / 100.0f;
        params.filterByConvexity = true;
        params.minConvexity = minConvexity / 100.0f;
        params.filterByInertia = true;
        params.minInertiaRatio = minInertia / 100.0f;
        
        // Create detector
        Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
        
        // Detect blobs in both images
        detector->detect(gray1, keypoints1);
        detector->detect(gray2, keypoints2);
        
        cout << "Blobs detected: " << keypoints1.size() << " (image1), " 
             << keypoints2.size() << " (image2)" << endl;
        
        if (keypoints1.empty() || keypoints2.empty()) {
            cout << "No blobs detected. Adjust parameters." << endl;
            resultImage = Mat();
            return;
        }
        
        // Compute SIFT descriptors at blob locations
        Ptr<SIFT> sift = SIFT::create();
        Mat descriptors1, descriptors2;
        
        sift->compute(gray1, keypoints1, descriptors1);
        sift->compute(gray2, keypoints2, descriptors2);
        
        if (descriptors1.empty() || descriptors2.empty()) {
            cout << "No descriptors computed." << endl;
            resultImage = Mat();
            return;
        }
        
        // Match descriptors
        BFMatcher matcher(NORM_L2);
        vector<vector<DMatch>> knnMatches;
        matcher.knnMatch(descriptors1, descriptors2, knnMatches, 2);
        
        // Ratio test
        goodMatches.clear();
        float ratio = matchRatio / 100.0f;
        
        for (size_t i = 0; i < knnMatches.size(); i++) {
            if (knnMatches[i].size() >= 2) {
                if (knnMatches[i][0].distance < ratio * knnMatches[i][1].distance) {
                    goodMatches.push_back(knnMatches[i][0]);
                }
            }
        }
        
        cout << "Good matches found: " << goodMatches.size() << endl;
        
        // Draw matches
        drawMatches(img1, keypoints1, img2, keypoints2, goodMatches, resultImage,
                    Scalar::all(-1), Scalar::all(-1), vector<char>(),
                    DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
        
        // Add info text
        string info = "Matches: " + to_string(goodMatches.size()) + 
                     " | Blob KP1: " + to_string(keypoints1.size()) +
                     " | Blob KP2: " + to_string(keypoints2.size());
        putText(resultImage, info, Point(10, 30), FONT_HERSHEY_SIMPLEX,
                0.7, Scalar(0, 255, 0), 2);
        
        string params_str = "Area:" + to_string(minArea) + 
                       " Circ:" + to_string(minCircularity) + "%" +
                       " Thresh:" + to_string(minThreshold) + "-" + to_string(maxThreshold);
        putText(resultImage, params_str, Point(10, 60), FONT_HERSHEY_SIMPLEX,
                0.5, Scalar(255, 255, 0), 1);
        
        imshow(windowName, resultImage);
        
    } catch (const cv::Exception& e) {
        cerr << "OpenCV Error: " << e.what() << endl;
    }
}

int main(int argc, char** argv) {
    showHelp();
    
    if (argc < 3) {
        cerr << "Error: Two image files required!" << endl;
        cerr << "Usage: " << argv[0] << " image1.jpg image2.jpg" << endl;
        return -1;
    }
    
    // Load images
    img1 = imread(argv[1], IMREAD_COLOR);
    img2 = imread(argv[2], IMREAD_COLOR);
    
    if (img1.empty() || img2.empty()) {
        cerr << "Error: Could not open images" << endl;
        return -1;
    }
    
    cout << "Images loaded: " << img1.cols << "x" << img1.rows 
         << " and " << img2.cols << "x" << img2.rows << endl;
    
    // Resize for visualization
    int targetHeight = 600;
    if (img1.rows > targetHeight) {
        resize(img1, img1, Size(), (double)targetHeight/img1.rows, (double)targetHeight/img1.rows);
    }
    if (img2.rows > targetHeight) {
        resize(img2, img2, Size(), (double)targetHeight/img2.rows, (double)targetHeight/img2.rows);
    }
    
    // Convert to grayscale
    cvtColor(img1, gray1, COLOR_BGR2GRAY);
    cvtColor(img2, gray2, COLOR_BGR2GRAY);
    
    // Create window and trackbars
    namedWindow(windowName, WINDOW_NORMAL);
    createTrackbar("Min Threshold", windowName, &minThreshold, MAX_THRESHOLD, matchFeatures);
    createTrackbar("Max Threshold", windowName, &maxThreshold, MAX_THRESHOLD, matchFeatures);
    createTrackbar("Min Area", windowName, &minArea, MAX_AREA, matchFeatures);
    createTrackbar("Circularity %", windowName, &minCircularity, MAX_PERCENTAGE, matchFeatures);
    createTrackbar("Convexity %", windowName, &minConvexity, MAX_PERCENTAGE, matchFeatures);
    createTrackbar("Inertia %", windowName, &minInertia, MAX_PERCENTAGE, matchFeatures);
    createTrackbar("Match Ratio x100", windowName, &matchRatio, MAX_RATIO, matchFeatures);
    
    // Initial matching
    matchFeatures(0, 0);
    
    // Wait for key press
    while (true) {
        int key = waitKey(30);
        if (key == 27 || key == 'q' || key == 'Q') break;
        else if (key == 'h' || key == 'H') showHelp();
        else if (key == 'r' || key == 'R') {
            minThreshold = 10; maxThreshold = 220;
            minArea = 100; minCircularity = 4; minConvexity = 58;
            minInertia = 10; matchRatio = 75;
            setTrackbarPos("Min Threshold", windowName, minThreshold);
            setTrackbarPos("Max Threshold", windowName, maxThreshold);
            setTrackbarPos("Min Area", windowName, minArea);
            setTrackbarPos("Circularity %", windowName, minCircularity);
            setTrackbarPos("Convexity %", windowName, minConvexity);
            setTrackbarPos("Inertia %", windowName, minInertia);
            setTrackbarPos("Match Ratio x100", windowName, matchRatio);
            matchFeatures(0, 0);
        }
        else if (key == 's' || key == 'S') {
            if (!resultImage.empty()) {
                imwrite("blob_sift_matches.jpg", resultImage);
                cout << "Result saved" << endl;
            }
        }
    }
    
    destroyAllWindows();
    return 0;
}

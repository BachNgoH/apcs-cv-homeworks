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
int harrisBlockSize = 2;
int harrisApertureSize = 3;
int harrisK = 4;  // k * 100
int harrisThreshold = 200;
int matchRatio = 75;  // 0.75 * 100

const int MAX_BLOCK_SIZE = 10;
const int MAX_APERTURE = 7;
const int MAX_K_VALUE = 10;
const int MAX_THRESHOLD = 255;
const int MAX_RATIO = 100;

Mat img1, img2, gray1, gray2;
Mat resultImage;
vector<KeyPoint> keypoints1, keypoints2;
vector<DMatch> goodMatches;
const string windowName = "Harris + SIFT Matching";

void showHelp() {
    cout << "\n===== HARRIS + SIFT MATCHING - HELP =====" << endl;
    cout << "Usage: ./exercise_d image1.jpg image2.jpg" << endl;
    cout << "\nDescription:" << endl;
    cout << "  Detects keypoints using Harris corner detector" << endl;
    cout << "  Computes SIFT descriptors at Harris corners" << endl;
    cout << "  Matches features between two images" << endl;
    cout << "\nKeyboard Controls:" << endl;
    cout << "  'h' or 'H' - Show this help" << endl;
    cout << "  'r' or 'R' - Reset to original parameters" << endl;
    cout << "  's' or 'S' - Save current result" << endl;
    cout << "  'q' or 'Q' or ESC - Quit" << endl;
    cout << "\nTrackbar Parameters:" << endl;
    cout << "  Block Size     - Harris neighborhood size (2-10)" << endl;
    cout << "  Aperture Size  - Sobel aperture (3, 5, 7)" << endl;
    cout << "  K Value        - Harris k parameter x100" << endl;
    cout << "  Threshold      - Harris corner threshold (0-255)" << endl;
    cout << "  Match Ratio    - Distance ratio test x100 (75 = 0.75)" << endl;
    cout << "\nAlgorithm:" << endl;
    cout << "  1. Detect Harris corners in both images" << endl;
    cout << "  2. Compute SIFT descriptors at corner locations" << endl;
    cout << "  3. Match descriptors using ratio test" << endl;
    cout << "  4. Display matches between images" << endl;
    cout << "==========================================\n" << endl;
}

void matchFeatures(int, void*) {
    if (gray1.empty() || gray2.empty()) return;
    
    try {
        // ========== MANUAL HARRIS CORNER DETECTION ==========
        
        int actualApertureSize = (harrisApertureSize * 2) + 3;
        if (actualApertureSize > 7) actualApertureSize = 7;
        
        int actualBlockSize = harrisBlockSize;
        if (actualBlockSize < 2) actualBlockSize = 2;
        
        double k = (harrisK + 1) / 100.0;
        
        // === IMAGE 1: Manual Harris Detection ===
        
        // Step 1: Compute gradients
        Mat Ix1, Iy1;
        Sobel(gray1, Ix1, CV_32F, 1, 0, actualApertureSize);
        Sobel(gray1, Iy1, CV_32F, 0, 1, actualApertureSize);
        
        // Step 2: Compute gradient products
        Mat Ixx1, Iyy1, Ixy1;
        multiply(Ix1, Ix1, Ixx1);
        multiply(Iy1, Iy1, Iyy1);
        multiply(Ix1, Iy1, Ixy1);
        
        // Step 3: Apply Gaussian smoothing (structure tensor)
        Mat Sxx1, Syy1, Sxy1;
        int kernelSize = actualBlockSize * 2 + 1;
        GaussianBlur(Ixx1, Sxx1, Size(kernelSize, kernelSize), 0);
        GaussianBlur(Iyy1, Syy1, Size(kernelSize, kernelSize), 0);
        GaussianBlur(Ixy1, Sxy1, Size(kernelSize, kernelSize), 0);
        
        // Step 4: Compute Harris response
        Mat harris1(gray1.size(), CV_32F);
        for (int i = 0; i < gray1.rows; i++) {
            for (int j = 0; j < gray1.cols; j++) {
                float sxx = Sxx1.at<float>(i, j);
                float syy = Syy1.at<float>(i, j);
                float sxy = Sxy1.at<float>(i, j);
                
                float det = sxx * syy - sxy * sxy;
                float trace = sxx + syy;
                float response = det - k * trace * trace;
                
                harris1.at<float>(i, j) = response;
            }
        }
        
        // Normalize
        Mat harrisNorm1;
        normalize(harris1, harrisNorm1, 0, 255, NORM_MINMAX, CV_32FC1);
        
        // === IMAGE 2: Manual Harris Detection ===
        
        // Step 1: Compute gradients
        Mat Ix2, Iy2;
        Sobel(gray2, Ix2, CV_32F, 1, 0, actualApertureSize);
        Sobel(gray2, Iy2, CV_32F, 0, 1, actualApertureSize);
        
        // Step 2: Compute gradient products
        Mat Ixx2, Iyy2, Ixy2;
        multiply(Ix2, Ix2, Ixx2);
        multiply(Iy2, Iy2, Iyy2);
        multiply(Ix2, Iy2, Ixy2);
        
        // Step 3: Apply Gaussian smoothing
        Mat Sxx2, Syy2, Sxy2;
        GaussianBlur(Ixx2, Sxx2, Size(kernelSize, kernelSize), 0);
        GaussianBlur(Iyy2, Syy2, Size(kernelSize, kernelSize), 0);
        GaussianBlur(Ixy2, Sxy2, Size(kernelSize, kernelSize), 0);
        
        // Step 4: Compute Harris response
        Mat harris2(gray2.size(), CV_32F);
        for (int i = 0; i < gray2.rows; i++) {
            for (int j = 0; j < gray2.cols; j++) {
                float sxx = Sxx2.at<float>(i, j);
                float syy = Syy2.at<float>(i, j);
                float sxy = Sxy2.at<float>(i, j);
                
                float det = sxx * syy - sxy * sxy;
                float trace = sxx + syy;
                float response = det - k * trace * trace;
                
                harris2.at<float>(i, j) = response;
            }
        }
        
        // Normalize
        Mat harrisNorm2;
        normalize(harris2, harrisNorm2, 0, 255, NORM_MINMAX, CV_32FC1);
        
        // ========== END MANUAL HARRIS IMPLEMENTATION ==========
        
        // Convert corners to KeyPoints (OPTIMIZED - use threshold instead of nested loops)
        keypoints1.clear();
        keypoints2.clear();
        
        // Image 1 corners - find and sort by strength
        Mat cornerMask1;
        threshold(harrisNorm1, cornerMask1, harrisThreshold, 255, THRESH_BINARY);
        cornerMask1.convertTo(cornerMask1, CV_8U);
        
        vector<Point> corners1;
        findNonZero(cornerMask1, corners1);
        
        // Sort corners by Harris response strength (strongest first)
        vector<pair<float, Point>> cornersWithStrength1;
        for (size_t i = 0; i < corners1.size(); i++) {
            float strength = harrisNorm1.at<float>(corners1[i]);
            cornersWithStrength1.push_back(make_pair(strength, corners1[i]));
        }
        sort(cornersWithStrength1.begin(), cornersWithStrength1.end(),
             [](const pair<float, Point>& a, const pair<float, Point>& b) {
                 return a.first > b.first;  // Sort descending by strength
             });
        
        // Keep only top N strongest corners (limit to 2000 to prevent matcher overflow)
        int maxKeypoints = 2000;
        for (size_t i = 0; i < min((size_t)maxKeypoints, cornersWithStrength1.size()); i++) {
            keypoints1.push_back(KeyPoint(cornersWithStrength1[i].second.x, 
                                         cornersWithStrength1[i].second.y, 1));
        }
        
        // Image 2 corners - find and sort by strength
        Mat cornerMask2;
        threshold(harrisNorm2, cornerMask2, harrisThreshold, 255, THRESH_BINARY);
        cornerMask2.convertTo(cornerMask2, CV_8U);
        
        vector<Point> corners2;
        findNonZero(cornerMask2, corners2);
        
        // Sort corners by Harris response strength
        vector<pair<float, Point>> cornersWithStrength2;
        for (size_t i = 0; i < corners2.size(); i++) {
            float strength = harrisNorm2.at<float>(corners2[i]);
            cornersWithStrength2.push_back(make_pair(strength, corners2[i]));
        }
        sort(cornersWithStrength2.begin(), cornersWithStrength2.end(),
             [](const pair<float, Point>& a, const pair<float, Point>& b) {
                 return a.first > b.first;
             });
        
        // Keep only top N strongest corners
        for (size_t i = 0; i < min((size_t)maxKeypoints, cornersWithStrength2.size()); i++) {
            keypoints2.push_back(KeyPoint(cornersWithStrength2[i].second.x,
                                         cornersWithStrength2[i].second.y, 1));
        }
        
        cout << "Harris corners detected: " << keypoints1.size() << " (image1), " 
             << keypoints2.size() << " (image2)" << endl;
        
        if (keypoints1.empty() || keypoints2.empty()) {
            cout << "No keypoints detected. Adjust Harris threshold." << endl;
            resultImage = Mat();
            return;
        }
        
        // 2. COMPUTE SIFT DESCRIPTORS at Harris corners
        Ptr<SIFT> sift = SIFT::create();
        Mat descriptors1, descriptors2;
        
        sift->compute(gray1, keypoints1, descriptors1);
        sift->compute(gray2, keypoints2, descriptors2);
        
        if (descriptors1.empty() || descriptors2.empty()) {
            cout << "No descriptors computed." << endl;
            resultImage = Mat();
            return;
        }
        
        // 3. MATCH DESCRIPTORS using BFMatcher
        BFMatcher matcher(NORM_L2);
        vector<vector<DMatch>> knnMatches;
        matcher.knnMatch(descriptors1, descriptors2, knnMatches, 2);
        
        // 4. RATIO TEST (Lowe's ratio test)
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
        
        // 5. DRAW MATCHES
        drawMatches(img1, keypoints1, img2, keypoints2, goodMatches, resultImage,
                    Scalar::all(-1), Scalar::all(-1), vector<char>(),
                    DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
        
        // Add info text
        string info = "Matches: " + to_string(goodMatches.size()) + 
                     " | KP1: " + to_string(keypoints1.size()) +
                     " | KP2: " + to_string(keypoints2.size());
        putText(resultImage, info, Point(10, 30), FONT_HERSHEY_SIMPLEX,
                0.7, Scalar(0, 255, 0), 2);
        
        string params = "Harris: Block=" + to_string(actualBlockSize) +
                       " K=" + to_string(k).substr(0, 4) +
                       " Thresh=" + to_string(harrisThreshold) +
                       " | Ratio=" + to_string(ratio).substr(0, 4);
        putText(resultImage, params, Point(10, 60), FONT_HERSHEY_SIMPLEX,
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
    string imagePath1 = argv[1];
    string imagePath2 = argv[2];
    
    img1 = imread(imagePath1, IMREAD_COLOR);
    img2 = imread(imagePath2, IMREAD_COLOR);
    
    if (img1.empty()) {
        cerr << "Error: Could not open image 1: " << imagePath1 << endl;
        return -1;
    }
    
    if (img2.empty()) {
        cerr << "Error: Could not open image 2: " << imagePath2 << endl;
        return -1;
    }
    
    cout << "Image 1 loaded: " << imagePath1 << " (" << img1.cols << "x" << img1.rows << ")" << endl;
    cout << "Image 2 loaded: " << imagePath2 << " (" << img2.cols << "x" << img2.rows << ")" << endl;
    
    // Resize images to roughly the same height for better visualization
    int targetHeight = 600;  // Target height
    
    if (img1.rows != img2.rows) {
        double scale1 = (double)targetHeight / img1.rows;
        double scale2 = (double)targetHeight / img2.rows;
        
        if (img1.rows > targetHeight) {
            resize(img1, img1, Size(), scale1, scale1, INTER_AREA);
            cout << "Resized image 1 to: " << img1.cols << "x" << img1.rows << endl;
        }
        
        if (img2.rows > targetHeight) {
            resize(img2, img2, Size(), scale2, scale2, INTER_AREA);
            cout << "Resized image 2 to: " << img2.cols << "x" << img2.rows << endl;
        }
    }
    
    // Convert to grayscale
    cvtColor(img1, gray1, COLOR_BGR2GRAY);
    cvtColor(img2, gray2, COLOR_BGR2GRAY);
    
    // Create window and trackbars
    namedWindow(windowName, WINDOW_NORMAL);
    createTrackbar("Block Size", windowName, &harrisBlockSize, MAX_BLOCK_SIZE, matchFeatures);
    createTrackbar("Aperture Size", windowName, &harrisApertureSize, MAX_APERTURE, matchFeatures);
    createTrackbar("K Value x100", windowName, &harrisK, MAX_K_VALUE, matchFeatures);
    createTrackbar("Threshold", windowName, &harrisThreshold, MAX_THRESHOLD, matchFeatures);
    createTrackbar("Match Ratio x100", windowName, &matchRatio, MAX_RATIO, matchFeatures);
    
    // Initial matching
    matchFeatures(0, 0);
    
    // Wait for key press
    while (true) {
        int key = waitKey(30);
        if (key == 27 || key == 'q' || key == 'Q') { // ESC or q
            break;
        } else if (key == 'h' || key == 'H') {
            showHelp();
        } else if (key == 'r' || key == 'R') {
            cout << "Resetting parameters..." << endl;
            harrisBlockSize = 2;
            harrisApertureSize = 3;
            harrisK = 4;
            harrisThreshold = 200;
            matchRatio = 75;
            setTrackbarPos("Block Size", windowName, harrisBlockSize);
            setTrackbarPos("Aperture Size", windowName, harrisApertureSize);
            setTrackbarPos("K Value x100", windowName, harrisK);
            setTrackbarPos("Threshold", windowName, harrisThreshold);
            setTrackbarPos("Match Ratio x100", windowName, matchRatio);
            matchFeatures(0, 0);
        } else if (key == 's' || key == 'S') {
            if (!resultImage.empty()) {
                string outputPath = "harris_sift_matches.jpg";
                imwrite(outputPath, resultImage);
                cout << "Result saved to: " << outputPath << endl;
            }
        }
    }
    
    destroyAllWindows();
    cout << "Program terminated." << endl;
    
    return 0;
}

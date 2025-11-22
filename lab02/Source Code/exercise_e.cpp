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
int nFeatures = 0;        // 0 = unlimited
int nOctaveLayers = 3;
int contrastThreshold = 4; // x100 (actual 0.04)
int edgeThreshold = 10;
int matchRatio = 75;  // 0.75 * 100

const int MAX_FEATURES = 5000;
const int MAX_OCTAVE_LAYERS = 8;
const int MAX_CONTRAST = 20;
const int MAX_EDGE = 20;
const int MAX_RATIO = 100;

Mat img1, img2, gray1, gray2;
Mat resultImage;
vector<KeyPoint> keypoints1, keypoints2;
vector<DMatch> goodMatches;
const string windowName = "DoG + SIFT Matching";

void showHelp() {
    cout << "\n===== DOG + SIFT MATCHING - HELP =====" << endl;
    cout << "Usage: ./exercise_e image1.jpg image2.jpg" << endl;
    cout << "\nDescription:" << endl;
    cout << "  Detects keypoints using DoG (Difference of Gaussians)" << endl;
    cout << "  Computes SIFT descriptors at DoG keypoints" << endl;
    cout << "  Matches features between two images" << endl;
    cout << "\nKeyboard Controls:" << endl;
    cout << "  'h' or 'H' - Show this help" << endl;
    cout << "  'r' or 'R' - Reset to original parameters" << endl;
    cout << "  's' or 'S' - Save current result" << endl;
    cout << "  'q' or 'Q' or ESC - Quit" << endl;
    cout << "\nTrackbar Parameters:" << endl;
    cout << "  Max Features    - Maximum keypoints (0=unlimited)" << endl;
    cout << "  Octave Layers   - Layers per octave (3-8)" << endl;
    cout << "  Contrast x100   - Contrast threshold (4 = 0.04)" << endl;
    cout << "  Edge Threshold  - Edge threshold (default 10)" << endl;
    cout << "  Match Ratio x100- Distance ratio test (75 = 0.75)" << endl;
    cout << "\nAlgorithm:" << endl;
    cout << "  1. Detect DoG keypoints in both images (SIFT detector)" << endl;
    cout << "  2. Compute SIFT descriptors at keypoint locations" << endl;
    cout << "  3. Match descriptors using ratio test" << endl;
    cout << "  4. Display matches between images" << endl;
    cout << "=========================================\n" << endl;
}

void matchFeatures(int, void*) {
    if (gray1.empty() || gray2.empty()) return;
    
    try {
        // Convert parameters
        double actualContrastThresh = contrastThreshold / 100.0;
        if (actualContrastThresh < 0.01) actualContrastThresh = 0.01;
        if (edgeThreshold < 1) edgeThreshold = 1;
        if (nOctaveLayers < 1) nOctaveLayers = 1;
        
        // ========== DOG + SIFT DETECTION AND MATCHING ==========
        
        // Create SIFT detector (SIFT uses DoG internally for detection)
        Ptr<SIFT> sift = SIFT::create(
            nFeatures,              // nfeatures
            nOctaveLayers,          // nOctaveLayers
            actualContrastThresh,   // contrastThreshold
            edgeThreshold,          // edgeThreshold
            1.6                     // sigma
        );
        
        // Detect keypoints and compute descriptors for image 1
        Mat descriptors1;
        sift->detectAndCompute(gray1, noArray(), keypoints1, descriptors1);
        
        // Detect keypoints and compute descriptors for image 2
        Mat descriptors2;
        sift->detectAndCompute(gray2, noArray(), keypoints2, descriptors2);
        
        cout << "DoG keypoints detected: " << keypoints1.size() << " (image1), " 
             << keypoints2.size() << " (image2)" << endl;
        
        if (keypoints1.empty() || keypoints2.empty()) {
            cout << "No keypoints detected. Adjust parameters." << endl;
            resultImage = Mat();
            return;
        }
        
        if (descriptors1.empty() || descriptors2.empty()) {
            cout << "No descriptors computed." << endl;
            resultImage = Mat();
            return;
        }
        
        // Match descriptors using BFMatcher
        BFMatcher matcher(NORM_L2);
        vector<vector<DMatch>> knnMatches;
        matcher.knnMatch(descriptors1, descriptors2, knnMatches, 2);
        
        // Lowe's ratio test
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
                     " | DoG KP1: " + to_string(keypoints1.size()) +
                     " | DoG KP2: " + to_string(keypoints2.size());
        putText(resultImage, info, Point(10, 30), FONT_HERSHEY_SIMPLEX,
                0.7, Scalar(0, 255, 0), 2);
        
        string params = "Features:" + to_string(nFeatures) + 
                       " Octaves:" + to_string(nOctaveLayers) +
                       " Contrast:" + to_string(actualContrastThresh).substr(0, 4) +
                       " | Ratio:" + to_string(ratio).substr(0, 4);
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
    int targetHeight = 600;
    
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
    createTrackbar("Max Features", windowName, &nFeatures, MAX_FEATURES, matchFeatures);
    createTrackbar("Octave Layers", windowName, &nOctaveLayers, MAX_OCTAVE_LAYERS, matchFeatures);
    createTrackbar("Contrast x100", windowName, &contrastThreshold, MAX_CONTRAST, matchFeatures);
    createTrackbar("Edge Threshold", windowName, &edgeThreshold, MAX_EDGE, matchFeatures);
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
            nFeatures = 0;
            nOctaveLayers = 3;
            contrastThreshold = 4;
            edgeThreshold = 10;
            matchRatio = 75;
            setTrackbarPos("Max Features", windowName, nFeatures);
            setTrackbarPos("Octave Layers", windowName, nOctaveLayers);
            setTrackbarPos("Contrast x100", windowName, contrastThreshold);
            setTrackbarPos("Edge Threshold", windowName, edgeThreshold);
            setTrackbarPos("Match Ratio x100", windowName, matchRatio);
            matchFeatures(0, 0);
        } else if (key == 's' || key == 'S') {
            if (!resultImage.empty()) {
                string outputPath = "dog_sift_matches.jpg";
                imwrite(outputPath, resultImage);
                cout << "Result saved to: " << outputPath << endl;
            }
        }
    }
    
    destroyAllWindows();
    cout << "Program terminated." << endl;
    
    return 0;
}

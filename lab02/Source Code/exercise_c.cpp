#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;

// Global variables for trackbars
int nFeatures = 0;        // 0 = unlimited
int nOctaveLayers = 3;
int contrastThreshold = 4; // x100 (actual 0.04)
int edgeThreshold = 10;
int sigma = 16;            // x10 (actual 1.6)

const int MAX_FEATURES = 5000;
const int MAX_OCTAVE_LAYERS = 8;
const int MAX_CONTRAST = 20;  // 0.00 to 0.20
const int MAX_EDGE = 20;
const int MAX_SIGMA = 30;     // 0.0 to 3.0

Mat srcImage, resultImage;
const string windowName = "DoG (SIFT) Detection";
bool fromCamera = false;

void showHelp() {
    cout << "\n===== DOG (SIFT) DETECTION - HELP =====" << endl;
    cout << "Usage: ./exercise_c [image_file]" << endl;
    cout << "\nDescription:" << endl;
    cout << "  Detects keypoints using DoG (Difference of Gaussians) algorithm" << endl;
    cout << "  DoG is the foundation of SIFT feature detection" << endl;
    cout << "  If no image file is provided, captures from camera" << endl;
    cout << "\nKeyboard Controls:" << endl;
    cout << "  'h' or 'H' - Show this help" << endl;
    cout << "  'r' or 'R' - Reset to original image" << endl;
    cout << "  's' or 'S' - Save current result" << endl;
    cout << "  'q' or 'Q' or ESC - Quit" << endl;
    cout << "\nTrackbar Parameters:" << endl;
    cout << "  Max Features    - Maximum number of keypoints (0=unlimited)" << endl;
    cout << "  Octave Layers   - Number of layers per octave (3-8)" << endl;
    cout << "  Contrast Thresh - Contrast threshold x100 (default 4 = 0.04)" << endl;
    cout << "  Edge Threshold  - Edge threshold (default 10)" << endl;
    cout << "  Sigma           - Gaussian sigma x10 (default 16 = 1.6)" << endl;
    cout << "\nAlgorithm:" << endl;
    cout << "  DoG (Difference of Gaussians) detects scale-invariant keypoints by:" << endl;
    cout << "  1. Creating scale-space pyramid with Gaussian blur" << endl;
    cout << "  2. Computing differences between adjacent scales" << endl;
    cout << "  3. Finding local extrema in DoG images" << endl;
    cout << "  4. Filtering low-contrast and edge responses" << endl;
    cout << "========================================\n" << endl;
}

void dogDetection(int, void*) {
    if (srcImage.empty()) return;
    
    // Convert contrast threshold (stored as int x100)
    double actualContrastThresh = contrastThreshold / 100.0;
    
    // Convert sigma (stored as int x10)
    double actualSigma = sigma / 10.0;
    
    // Ensure valid values
    if (actualSigma < 0.1) actualSigma = 0.1;
    if (actualContrastThresh < 0.01) actualContrastThresh = 0.01;
    if (edgeThreshold < 1) edgeThreshold = 1;
    if (nOctaveLayers < 1) nOctaveLayers = 1;
    
    try {
        // Create SIFT detector (SIFT uses DoG internally)
        Ptr<SIFT> detector = SIFT::create(
            nFeatures,              // nfeatures (0 = no limit)
            nOctaveLayers,          // nOctaveLayers
            actualContrastThresh,   // contrastThreshold
            edgeThreshold,          // edgeThreshold
            actualSigma            // sigma
        );
        
        // Detect keypoints (on grayscale srcImage)
        vector<KeyPoint> keypoints;
        detector->detect(srcImage, keypoints);
        
        // Draw detected keypoints on color image (resultImage already set in processImage)
        drawKeypoints(resultImage, keypoints, resultImage, 
                      Scalar(0, 0, 255), 
                      DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        
        // Display keypoint count and info
        string info = "DoG Keypoints: " + to_string(keypoints.size());
        putText(resultImage, info, Point(10, 30), FONT_HERSHEY_SIMPLEX, 
                0.7, Scalar(0, 255, 0), 2);
        
        // Display parameters
        string params_str = "Features:" + to_string(nFeatures) + 
                           " Octaves:" + to_string(nOctaveLayers) +
                           " Contrast:" + to_string(actualContrastThresh).substr(0, 4);
        putText(resultImage, params_str, Point(10, 60), FONT_HERSHEY_SIMPLEX, 
                0.5, Scalar(255, 255, 0), 1);
        
        string filters = "Edge:" + to_string(edgeThreshold) + " " +
                        "Sigma:" + to_string(actualSigma).substr(0, 3);
        putText(resultImage, filters, Point(10, 85), FONT_HERSHEY_SIMPLEX, 
                0.5, Scalar(255, 255, 0), 1);
        
        imshow(windowName, resultImage);
        
    } catch (const cv::Exception& e) {
        cerr << "OpenCV Error: " << e.what() << endl;
        cerr << "Note: SIFT requires opencv_contrib (xfeatures2d module)" << endl;
    }
}

void processImage(const Mat& img) {
    Mat colorImage = img.clone();  // Keep color version for display
    
    // Convert to grayscale for detection (SIFT works on grayscale)
    if (img.channels() == 3) {
        cvtColor(img, srcImage, COLOR_BGR2GRAY);
    } else {
        srcImage = img.clone();
        cvtColor(srcImage, colorImage, COLOR_GRAY2BGR);  // Convert to color for display
    }
    
    // Store color image globally for drawing
    resultImage = colorImage.clone();
    
    // Perform DoG detection
    dogDetection(0, 0);
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
        createTrackbar("Max Features", windowName, &nFeatures, MAX_FEATURES, dogDetection);
        createTrackbar("Octave Layers", windowName, &nOctaveLayers, MAX_OCTAVE_LAYERS, dogDetection);
        createTrackbar("Contrast x100", windowName, &contrastThreshold, MAX_CONTRAST, dogDetection);
        createTrackbar("Edge Threshold", windowName, &edgeThreshold, MAX_EDGE, dogDetection);
        createTrackbar("Sigma x10", windowName, &sigma, MAX_SIGMA, dogDetection);
        
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
                string outputPath = "dog_result.jpg";
                imwrite(outputPath, resultImage);
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
        createTrackbar("Max Features", windowName, &nFeatures, MAX_FEATURES, dogDetection);
        createTrackbar("Octave Layers", windowName, &nOctaveLayers, MAX_OCTAVE_LAYERS, dogDetection);
        createTrackbar("Contrast x100", windowName, &contrastThreshold, MAX_CONTRAST, dogDetection);
        createTrackbar("Edge Threshold", windowName, &edgeThreshold, MAX_EDGE, dogDetection);
        createTrackbar("Sigma x10", windowName, &sigma, MAX_SIGMA, dogDetection);
        
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
                string outputPath = "dog_result_camera.jpg";
                imwrite(outputPath, resultImage);
                cout << "Frame saved to: " << outputPath << endl;
            }
        }
        
        cap.release();
    }
    
    destroyAllWindows();
    cout << "Program terminated." << endl;
    
    return 0;
}

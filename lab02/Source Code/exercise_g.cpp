#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace cv;
using namespace std;

// Global variables  
int harrisBlockSize = 2;
int harrisApertureSize = 0;
int harrisK = 4;
int harrisThreshold = 200;
int matchRatio = 75;
int lbpRadius = 1;

const int MAX_BLOCK_SIZE = 10;
const int MAX_APERTURE = 7;
const int MAX_K = 10;
const int MAX_THRESHOLD = 255;
const int MAX_RATIO = 100;
const int MAX_LBP_RADIUS = 3;

Mat img1, img2, gray1, gray2;
Mat resultImage;
vector<KeyPoint> keypoints1, keypoints2;
const string windowName = "Harris + LBP Matching";

// Compute LBP descriptor for a keypoint region
Mat computeLBPDescriptor(const Mat& gray, Point2f center, int radius) {
    int patchSize = 40;  // Larger patch for better description
    int x = cvRound(center.x);
    int y = cvRound(center.y);
    
    // Extract patch
    int x1 = max(0, x - patchSize/2);
    int y1 = max(0, y - patchSize/2);
    int x2 = min(gray.cols, x + patchSize/2);
    int y2 = min(gray.rows, y + patchSize/2);
    
    if (x2 <= x1 + 5 || y2 <= y1 + 5) return Mat();
    
    Rect roi(x1, y1, x2-x1, y2-y1);
    Mat patch = gray(roi);
    
    // Compute LBP for the patch
    Mat lbp = Mat::zeros(patch.size(), CV_8U);
    for (int i = 1; i < patch.rows - 1; i++) {
        for (int j = 1; j < patch.cols - 1; j++) {
            uchar center = patch.at<uchar>(i, j);
            uchar code = 0;
            code |= (patch.at<uchar>(i-1, j) >= center) << 7;
            code |= (patch.at<uchar>(i-1, j+1) >= center) << 6;
            code |= (patch.at<uchar>(i, j+1) >= center) << 5;
            code |= (patch.at<uchar>(i+1, j+1) >= center) << 4;
            code |= (patch.at<uchar>(i+1, j) >= center) << 3;
            code |= (patch.at<uchar>(i+1, j-1) >= center) << 2;
            code |= (patch.at<uchar>(i, j-1) >= center) << 1;
            code |= (patch.at<uchar>(i-1, j-1) >= center) << 0;
            lbp.at<uchar>(i, j) = code;
        }
    }
    
    // Create histogram (256 bins for full LBP)
    int histSize = 256;
    float range[] = {0, 256};
    const float* histRange = {range};
    Mat hist;
    calcHist(&lbp, 1, 0, Mat(), hist, 1, &histSize, &histRange);
    
    // Add small value to avoid division by zero
    hist += 1e-7;
    
    // Normalize to sum = 1
    double sum = cv::sum(hist)[0];
    hist /= sum;
    
    return hist.reshape(1, 1);  // Row vector
}

void matchFeatures(int, void*) {
    if (gray1.empty() || gray2.empty()) return;
    
    try {
        int actualBlockSize = harrisBlockSize < 2 ? 2 : harrisBlockSize;
        int actualApertureSize = (harrisApertureSize * 2) + 3;
        if (actualApertureSize > 7) actualApertureSize = 7;
        double k = (harrisK + 1) / 100.0;
        
        // MANUAL HARRIS DETECTION
        Mat Ix1, Iy1, Ix2, Iy2;
        Sobel(gray1, Ix1, CV_32F, 1, 0, actualApertureSize);
        Sobel(gray1, Iy1, CV_32F, 0, 1, actualApertureSize);
        Sobel(gray2, Ix2, CV_32F, 1, 0, actualApertureSize);
        Sobel(gray2, Iy2, CV_32F, 0, 1, actualApertureSize);
        
        Mat Ixx1, Iyy1, Ixy1, Ixx2, Iyy2, Ixy2;
        multiply(Ix1, Ix1, Ixx1);
        multiply(Iy1, Iy1, Iyy1);
        multiply(Ix1, Iy1, Ixy1);
        multiply(Ix2, Ix2, Ixx2);
        multiply(Iy2, Iy2, Iyy2);
        multiply(Ix2, Iy2, Ixy2);
        
        int kernelSize = actualBlockSize * 2 + 1;
        Mat Sxx1, Syy1, Sxy1, Sxx2, Syy2, Sxy2;
        GaussianBlur(Ixx1, Sxx1, Size(kernelSize, kernelSize), 0);
        GaussianBlur(Iyy1, Syy1, Size(kernelSize, kernelSize), 0);
        GaussianBlur(Ixy1, Sxy1, Size(kernelSize, kernelSize), 0);
        GaussianBlur(Ixx2, Sxx2, Size(kernelSize, kernelSize), 0);
        GaussianBlur(Iyy2, Syy2, Size(kernelSize, kernelSize), 0);
        GaussianBlur(Ixy2, Sxy2, Size(kernelSize, kernelSize), 0);
        
        Mat harris1(gray1.size(), CV_32F), harris2(gray2.size(), CV_32F);
        for (int i = 0; i < gray1.rows; i++) {
            for (int j = 0; j < gray1.cols; j++) {
                float sxx = Sxx1.at<float>(i, j), syy = Syy1.at<float>(i, j), sxy = Sxy1.at<float>(i, j);
                harris1.at<float>(i, j) = sxx * syy - sxy * sxy - k * (sxx + syy) * (sxx + syy);
            }
        }
        for (int i = 0; i < gray2.rows; i++) {
            for (int j = 0; j < gray2.cols; j++) {
                float sxx = Sxx2.at<float>(i, j), syy = Syy2.at<float>(i, j), sxy = Sxy2.at<float>(i, j);
                harris2.at<float>(i, j) = sxx * syy - sxy * sxy - k * (sxx + syy) * (sxx + syy);
            }
        }
        
        Mat harrisNorm1, harrisNorm2;
        normalize(harris1, harrisNorm1, 0, 255, NORM_MINMAX);
        normalize(harris2, harrisNorm2, 0, 255, NORM_MINMAX);
        
        // Extract keypoints
        keypoints1.clear(); keypoints2.clear();
        Mat mask1, mask2;
        threshold(harrisNorm1, mask1, harrisThreshold, 255, THRESH_BINARY);
        threshold(harrisNorm2, mask2, harrisThreshold, 255, THRESH_BINARY);
        mask1.convertTo(mask1, CV_8U);
        mask2.convertTo(mask2, CV_8U);
        
        vector<Point> corners1, corners2;
        findNonZero(mask1, corners1);
        findNonZero(mask2, corners2);
        
        // Limit to top 500 corners
        int maxCorners = 500;
        for (size_t i = 0; i < min((size_t)maxCorners, corners1.size()); i++)
            keypoints1.push_back(KeyPoint(corners1[i].x, corners1[i].y, 1));
        for (size_t i = 0; i < min((size_t)maxCorners, corners2.size()); i++)
            keypoints2.push_back(KeyPoint(corners2[i].x, corners2[i].y, 1));
        
        cout << "Harris corners: " << keypoints1.size() << " (img1), " << keypoints2.size() << " (img2)" << endl;
        
        if (keypoints1.empty() || keypoints2.empty()) return;
        
        // Compute LBP descriptors
        Mat descriptors1(keypoints1.size(), 256, CV_32F);
        Mat descriptors2(keypoints2.size(), 256, CV_32F);
        
        for (size_t i = 0; i < keypoints1.size(); i++) {
            Mat desc = computeLBPDescriptor(gray1, keypoints1[i].pt, lbpRadius + 1);
            if (!desc.empty()) desc.copyTo(descriptors1.row(i));
        }
        for (size_t i = 0; i < keypoints2.size(); i++) {
            Mat desc = computeLBPDescriptor(gray2, keypoints2[i].pt, lbpRadius + 1);
            if (!desc.empty()) desc.copyTo(descriptors2.row(i));
        }
        
        // Match using histogram intersection (better for LBP histograms)
        vector<DMatch> goodMatches;
        for (size_t i = 0; i < keypoints1.size(); i++) {
            double bestDist = 1e9;
            double secondBestDist = 1e9;
            int bestMatch = -1;
            
            for (size_t j = 0; j < keypoints2.size(); j++) {
                // Chi-square distance for histograms
                double dist = compareHist(descriptors1.row(i), descriptors2.row(j), HISTCMP_CHISQR);
                
                if (dist < bestDist) {
                    secondBestDist = bestDist;
                    bestDist = dist;
                    bestMatch = j;
                } else if (dist < secondBestDist) {
                    secondBestDist = dist;
                }
            }
            
            if (bestMatch >= 0) {
                // Ratio test
                float ratio = matchRatio / 100.0f;
                if (secondBestDist > 0 && bestDist < ratio * secondBestDist) {
                    goodMatches.push_back(DMatch(i, bestMatch, bestDist));
                }
            }
        }
        
        cout << "LBP matches: " << goodMatches.size() << endl;
        
        drawMatches(img1, keypoints1, img2, keypoints2, goodMatches, resultImage);
        
        string info = "Matches: " + to_string(goodMatches.size()) + 
                     " | Harris KP1: " + to_string(keypoints1.size()) +
                     " | Harris KP2: " + to_string(keypoints2.size());
        putText(resultImage, info, Point(10, 30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 0), 2);
        
        imshow(windowName, resultImage);
        
    } catch (const cv::Exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}

int main(int argc, char** argv) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " image1.jpg image2.jpg" << endl;
        return -1;
    }
    
    img1 = imread(argv[1], IMREAD_COLOR);
    img2 = imread(argv[2], IMREAD_COLOR);
    
    if (img1.empty() || img2.empty()) {
        cerr << "Error loading images" << endl;
        return -1;
    }
    
    // Resize for performance
    int targetHeight = 600;
    if (img1.rows > targetHeight) resize(img1, img1, Size(), (double)targetHeight/img1.rows, (double)targetHeight/img1.rows);
    if (img2.rows > targetHeight) resize(img2, img2, Size(), (double)targetHeight/img2.rows, (double)targetHeight/img2.rows);
    
    cvtColor(img1, gray1, COLOR_BGR2GRAY);
    cvtColor(img2, gray2, COLOR_BGR2GRAY);
    
    namedWindow(windowName, WINDOW_NORMAL);
    createTrackbar("Block Size", windowName, &harrisBlockSize, MAX_BLOCK_SIZE, matchFeatures);
    createTrackbar("Aperture", windowName, &harrisApertureSize, MAX_APERTURE, matchFeatures);
    createTrackbar("K x100", windowName, &harrisK, MAX_K, matchFeatures);
    createTrackbar("Threshold", windowName, &harrisThreshold, MAX_THRESHOLD, matchFeatures);
    createTrackbar("LBP Radius", windowName, &lbpRadius, MAX_LBP_RADIUS, matchFeatures);
    createTrackbar("Match Ratio x100", windowName, &matchRatio, MAX_RATIO, matchFeatures);
    
    matchFeatures(0, 0);
    
    while (true) {
        int key = waitKey(30);
        if (key == 27 || key == 'q') break;
        else if (key == 's' && !resultImage.empty()) {
            imwrite("harris_lbp_matches.jpg", resultImage);
            cout << "Saved!" << endl;
        }
    }
    
    return 0;
}

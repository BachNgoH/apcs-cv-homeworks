#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;

int nFeatures = 500;
int nOctaveLayers = 3;
int contrastThreshold = 4;
int edgeThreshold = 10;
int matchRatio = 75;
int lbpRadius = 1;

Mat img1, img2, gray1, gray2, resultImage;
vector<KeyPoint> keypoints1, keypoints2;
const string windowName = "DoG + LBP Matching";

Mat computeLBPDescriptor(const Mat& gray, Point2f center, int radius) {
    int patchSize = 40;
    int x = cvRound(center.x), y = cvRound(center.y);
    int x1 = max(0, x - patchSize/2), y1 = max(0, y - patchSize/2);
    int x2 = min(gray.cols, x + patchSize/2), y2 = min(gray.rows, y + patchSize/2);
    if (x2 <= x1 + 5 || y2 <= y1 + 5) return Mat();
    
    Mat patch = gray(Rect(x1, y1, x2-x1, y2-y1));
    Mat lbp = Mat::zeros(patch.size(), CV_8U);
    
    for (int i = 1; i < patch.rows - 1; i++) {
        for (int j = 1; j < patch.cols - 1; j++) {
            uchar c = patch.at<uchar>(i, j), code = 0;
            code |= (patch.at<uchar>(i-1,j) >= c) << 7;
            code |= (patch.at<uchar>(i-1,j+1) >= c) << 6;
            code |= (patch.at<uchar>(i,j+1) >= c) << 5;
            code |= (patch.at<uchar>(i+1,j+1) >= c) << 4;
            code |= (patch.at<uchar>(i+1,j) >= c) << 3;
            code |= (patch.at<uchar>(i+1,j-1) >= c) << 2;
            code |= (patch.at<uchar>(i,j-1) >= c) << 1;
            code |= (patch.at<uchar>(i-1,j-1) >= c) << 0;
            lbp.at<uchar>(i,j) = code;
        }
    }
    
    int histSize = 256;
    float range[] = {0, 256};
    const float* histRange = {range};
    Mat hist;
    calcHist(&lbp, 1, 0, Mat(), hist, 1, &histSize, &histRange);
    hist += 1e-7;
    hist /= cv::sum(hist)[0];
    return hist.reshape(1, 1);
}

void matchFeatures(int, void*) {
    if (gray1.empty() || gray2.empty()) return;
    
    try {
        // DoG detection using SIFT
        Ptr<SIFT> sift = SIFT::create(nFeatures, nOctaveLayers, contrastThreshold/100.0, edgeThreshold, 1.6);
        sift->detect(gray1, keypoints1);
        sift->detect(gray2, keypoints2);
        
        cout << "DoG keypoints: " << keypoints1.size() << " (img1), " << keypoints2.size() << " (img2)" << endl;
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
        
        // Match using chi-square distance
        vector<DMatch> goodMatches;
        for (size_t i = 0; i < keypoints1.size(); i++) {
            double bestDist = 1e9, secondBestDist = 1e9;
            int bestMatch = -1;
            
            for (size_t j = 0; j < keypoints2.size(); j++) {
                double dist = compareHist(descriptors1.row(i), descriptors2.row(j), HISTCMP_CHISQR);
                if (dist < bestDist) {
                    secondBestDist = bestDist;
                    bestDist = dist;
                    bestMatch = j;
                } else if (dist < secondBestDist) {
                    secondBestDist = dist;
                }
            }
            
            if (bestMatch >= 0 && secondBestDist > 0 && bestDist < (matchRatio/100.0f) * secondBestDist) {
                goodMatches.push_back(DMatch(i, bestMatch, bestDist));
            }
        }
        
        cout << "LBP matches: " << goodMatches.size() << endl;
        
        drawMatches(img1, keypoints1, img2, keypoints2, goodMatches, resultImage);
        string info = "Matches: " + to_string(goodMatches.size()) + " | DoG KP1: " + to_string(keypoints1.size()) + " | DoG KP2: " + to_string(keypoints2.size());
        putText(resultImage, info, Point(10, 30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0,255,0), 2);
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
    if (img1.empty() || img2.empty()) return -1;
    
    int targetHeight = 600;
    if (img1.rows > targetHeight) resize(img1, img1, Size(), (double)targetHeight/img1.rows, (double)targetHeight/img1.rows);
    if (img2.rows > targetHeight) resize(img2, img2, Size(), (double)targetHeight/img2.rows, (double)targetHeight/img2.rows);
    
    cvtColor(img1, gray1, COLOR_BGR2GRAY);
    cvtColor(img2, gray2, COLOR_BGR2GRAY);
    
    namedWindow(windowName, WINDOW_NORMAL);
    createTrackbar("Max Features", windowName, &nFeatures, 2000, matchFeatures);
    createTrackbar("Octave Layers", windowName, &nOctaveLayers, 8, matchFeatures);
    createTrackbar("Contrast x100", windowName, &contrastThreshold, 20, matchFeatures);
    createTrackbar("Edge Threshold", windowName, &edgeThreshold, 20, matchFeatures);
    createTrackbar("LBP Radius", windowName, &lbpRadius, 3, matchFeatures);
    createTrackbar("Match Ratio x100", windowName, &matchRatio, 100, matchFeatures);
    
    matchFeatures(0, 0);
    
    while (waitKey(30) != 27);
    return 0;
}

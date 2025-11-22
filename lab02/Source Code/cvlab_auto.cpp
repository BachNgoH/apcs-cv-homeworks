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

// Function prototypes
void detectHarrisAuto(const string& imagePath, const string& outputPath);
void detectBlobAuto(const string& imagePath, const string& outputPath);
void detectDoGAuto(const string& imagePath, const string& outputPath);
void matchHarrisSIFTAuto(const string& img1Path, const string& img2Path, const string& outputPath);
void matchDoGSIFTAuto(const string& img1Path, const string& img2Path, const string& outputPath);
void matchBlobSIFTAuto(const string& img1Path, const string& img2Path, const string& outputPath);
void matchHarrisLBPAuto(const string& img1Path, const string& img2Path, const string& outputPath);
void matchDoGLBPAuto(const string& img1Path, const string& img2Path, const string& outputPath);
void matchBlobLBPAuto(const string& img1Path, const string& img2Path, const string& outputPath);

// Shared LBP computation
Mat computeLBPDescriptor(const Mat& gray, Point2f center) {
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

// Manual Harris detection
vector<KeyPoint> detectHarrisKeypoints(const Mat& gray) {
    int blockSize = 2, apertureSize = 3;
    double k = 0.04;
    Mat Ix, Iy;
    Sobel(gray, Ix, CV_32F, 1, 0, apertureSize);
    Sobel(gray, Iy, CV_32F, 0, 1, apertureSize);
    Mat Ixx, Iyy, Ixy;
    multiply(Ix, Ix, Ixx); multiply(Iy, Iy, Iyy); multiply(Ix, Iy, Ixy);
    Mat Sxx, Syy, Sxy;
    GaussianBlur(Ixx, Sxx, Size(3,3), 0); GaussianBlur(Iyy, Syy, Size(3,3), 0); GaussianBlur(Ixy, Sxy, Size(3,3), 0);
    Mat harris(gray.size(), CV_32F);
    for(int i=0; i<gray.rows; i++) {
        for(int j=0; j<gray.cols; j++) {
            float sxx = Sxx.at<float>(i,j), syy = Syy.at<float>(i,j), sxy = Sxy.at<float>(i,j);
            harris.at<float>(i,j) = sxx*syy - sxy*sxy - k*(sxx+syy)*(sxx+syy);
        }
    }
    Mat harrisNorm, mask;
    normalize(harris, harrisNorm, 0, 255, NORM_MINMAX);
    threshold(harrisNorm, mask, 200, 255, THRESH_BINARY);
    mask.convertTo(mask, CV_8U);
    vector<Point> corners;
    findNonZero(mask, corners);
    vector<KeyPoint> kps;
    for(size_t i=0; i<min((size_t)500, corners.size()); i++) kps.push_back(KeyPoint(corners[i].x, corners[i].y, 1));
    return kps;
}

int main(int argc, char** argv) {
    if (argc < 4) {
        cerr << "Usage: ./cvlab_auto <command> <input> [input2] <output>" << endl;
        return -1;
    }
    
    string command = argv[1];
    
    if (command == "harris") detectHarrisAuto(argv[2], argv[3]);
    else if (command == "blob") detectBlobAuto(argv[2], argv[3]);
    else if (command == "dog") detectDoGAuto(argv[2], argv[3]);
    else if (command == "m") {
        if (argc < 6) return -1;
        string detector = argv[2];
        string descriptor = argv[3];
        string img1 = argv[4];
        string img2 = argv[5];
        string out = argv[6];
        
        if (detector == "harris" && descriptor == "sift") matchHarrisSIFTAuto(img1, img2, out);
        else if (detector == "dog" && descriptor == "sift") matchDoGSIFTAuto(img1, img2, out);
        else if (detector == "blob" && descriptor == "sift") matchBlobSIFTAuto(img1, img2, out);
        else if (detector == "harris" && descriptor == "lbp") matchHarrisLBPAuto(img1, img2, out);
        else if (detector == "dog" && descriptor == "lbp") matchDoGLBPAuto(img1, img2, out);
        else if (detector == "blob" && descriptor == "lbp") matchBlobLBPAuto(img1, img2, out);
    }
    return 0;
}

void detectHarrisAuto(const string& imagePath, const string& outputPath) {
    Mat img = imread(imagePath);
    if(img.empty()) return;
    Mat gray; cvtColor(img, gray, COLOR_BGR2GRAY);
    vector<KeyPoint> kps = detectHarrisKeypoints(gray);
    Mat res = img.clone();
    for(auto& kp : kps) circle(res, kp.pt, 3, Scalar(0,0,255), 2);
    putText(res, "Harris: " + to_string(kps.size()), Point(10,30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0,255,0), 2);
    imwrite(outputPath, res);
    cout << "Saved: " << outputPath << endl;
}

void detectBlobAuto(const string& imagePath, const string& outputPath) {
    Mat img = imread(imagePath);
    if(img.empty()) return;
    Mat gray; cvtColor(img, gray, COLOR_BGR2GRAY);
    SimpleBlobDetector::Params params;
    params.minThreshold = 10; params.maxThreshold = 220;
    params.filterByArea = true; params.minArea = 100;
    params.filterByCircularity = true; params.minCircularity = 0.04f;
    params.filterByConvexity = true; params.minConvexity = 0.58f;
    params.filterByInertia = true; params.minInertiaRatio = 0.1f;
    Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
    vector<KeyPoint> kps; detector->detect(gray, kps);
    Mat res; drawKeypoints(img, kps, res, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    putText(res, "Blobs: " + to_string(kps.size()), Point(10,30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0,255,0), 2);
    imwrite(outputPath, res);
    cout << "Saved: " << outputPath << endl;
}

void detectDoGAuto(const string& imagePath, const string& outputPath) {
    Mat img = imread(imagePath);
    if(img.empty()) return;
    Mat gray; cvtColor(img, gray, COLOR_BGR2GRAY);
    Ptr<SIFT> sift = SIFT::create(500);
    vector<KeyPoint> kps; sift->detect(gray, kps);
    Mat res; drawKeypoints(img, kps, res, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    putText(res, "DoG: " + to_string(kps.size()), Point(10,30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0,255,0), 2);
    imwrite(outputPath, res);
    cout << "Saved: " << outputPath << endl;
}

void matchHarrisSIFTAuto(const string& img1Path, const string& img2Path, const string& outputPath) {
    Mat img1 = imread(img1Path), img2 = imread(img2Path);
    if(img1.empty() || img2.empty()) return;
    Mat gray1, gray2; cvtColor(img1, gray1, COLOR_BGR2GRAY); cvtColor(img2, gray2, COLOR_BGR2GRAY);
    vector<KeyPoint> kp1 = detectHarrisKeypoints(gray1), kp2 = detectHarrisKeypoints(gray2);
    Ptr<SIFT> sift = SIFT::create();
    Mat d1, d2; sift->compute(gray1, kp1, d1); sift->compute(gray2, kp2, d2);
    BFMatcher matcher(NORM_L2);
    vector<vector<DMatch>> knn; matcher.knnMatch(d1, d2, knn, 2);
    vector<DMatch> good;
    for(auto& m : knn) if(m.size() >= 2 && m[0].distance < 0.75 * m[1].distance) good.push_back(m[0]);
    Mat res; drawMatches(img1, kp1, img2, kp2, good, res);
    putText(res, "Matches: " + to_string(good.size()), Point(10,30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0,255,0), 2);
    imwrite(outputPath, res);
    cout << "Saved: " << outputPath << endl;
}

void matchDoGSIFTAuto(const string& img1Path, const string& img2Path, const string& outputPath) {
    Mat img1 = imread(img1Path), img2 = imread(img2Path);
    if(img1.empty() || img2.empty()) return;
    Mat gray1, gray2; cvtColor(img1, gray1, COLOR_BGR2GRAY); cvtColor(img2, gray2, COLOR_BGR2GRAY);
    Ptr<SIFT> sift = SIFT::create();
    vector<KeyPoint> kp1, kp2; Mat d1, d2;
    sift->detectAndCompute(gray1, Mat(), kp1, d1); sift->detectAndCompute(gray2, Mat(), kp2, d2);
    BFMatcher matcher(NORM_L2);
    vector<vector<DMatch>> knn; matcher.knnMatch(d1, d2, knn, 2);
    vector<DMatch> good;
    for(auto& m : knn) if(m.size() >= 2 && m[0].distance < 0.75 * m[1].distance) good.push_back(m[0]);
    Mat res; drawMatches(img1, kp1, img2, kp2, good, res);
    putText(res, "Matches: " + to_string(good.size()), Point(10,30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0,255,0), 2);
    imwrite(outputPath, res);
    cout << "Saved: " << outputPath << endl;
}

void matchBlobSIFTAuto(const string& img1Path, const string& img2Path, const string& outputPath) {
    Mat img1 = imread(img1Path), img2 = imread(img2Path);
    if(img1.empty() || img2.empty()) return;
    Mat gray1, gray2; cvtColor(img1, gray1, COLOR_BGR2GRAY); cvtColor(img2, gray2, COLOR_BGR2GRAY);
    SimpleBlobDetector::Params params;
    params.minThreshold = 10; params.maxThreshold = 220;
    params.filterByArea = true; params.minArea = 100;
    params.filterByCircularity = true; params.minCircularity = 0.04f;
    params.filterByConvexity = true; params.minConvexity = 0.58f;
    Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
    vector<KeyPoint> kp1, kp2; detector->detect(gray1, kp1); detector->detect(gray2, kp2);
    Ptr<SIFT> sift = SIFT::create();
    Mat d1, d2; sift->compute(gray1, kp1, d1); sift->compute(gray2, kp2, d2);
    BFMatcher matcher(NORM_L2);
    vector<vector<DMatch>> knn; matcher.knnMatch(d1, d2, knn, 2);
    vector<DMatch> good;
    for(auto& m : knn) if(m.size() >= 2 && m[0].distance < 0.75 * m[1].distance) good.push_back(m[0]);
    Mat res; drawMatches(img1, kp1, img2, kp2, good, res);
    putText(res, "Matches: " + to_string(good.size()), Point(10,30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0,255,0), 2);
    imwrite(outputPath, res);
    cout << "Saved: " << outputPath << endl;
}

void matchHarrisLBPAuto(const string& img1Path, const string& img2Path, const string& outputPath) {
    Mat img1 = imread(img1Path), img2 = imread(img2Path);
    if(img1.empty() || img2.empty()) return;
    Mat gray1, gray2; cvtColor(img1, gray1, COLOR_BGR2GRAY); cvtColor(img2, gray2, COLOR_BGR2GRAY);
    vector<KeyPoint> kp1 = detectHarrisKeypoints(gray1), kp2 = detectHarrisKeypoints(gray2);
    Mat d1(kp1.size(), 256, CV_32F), d2(kp2.size(), 256, CV_32F);
    for(size_t i=0; i<kp1.size(); i++) computeLBPDescriptor(gray1, kp1[i].pt).copyTo(d1.row(i));
    for(size_t i=0; i<kp2.size(); i++) computeLBPDescriptor(gray2, kp2[i].pt).copyTo(d2.row(i));
    vector<DMatch> good;
    for(size_t i=0; i<kp1.size(); i++) {
        double best=1e9, second=1e9; int idx=-1;
        for(size_t j=0; j<kp2.size(); j++) {
            double dist = compareHist(d1.row(i), d2.row(j), HISTCMP_CHISQR);
            if(dist < best) { second=best; best=dist; idx=j; }
            else if(dist < second) second=dist;
        }
        if(idx>=0 && second>0 && best < 0.75*second) good.push_back(DMatch(i, idx, best));
    }
    Mat res; drawMatches(img1, kp1, img2, kp2, good, res);
    putText(res, "Matches: " + to_string(good.size()), Point(10,30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0,255,0), 2);
    imwrite(outputPath, res);
    cout << "Saved: " << outputPath << endl;
}

void matchDoGLBPAuto(const string& img1Path, const string& img2Path, const string& outputPath) {
    Mat img1 = imread(img1Path), img2 = imread(img2Path);
    if(img1.empty() || img2.empty()) return;
    Mat gray1, gray2; cvtColor(img1, gray1, COLOR_BGR2GRAY); cvtColor(img2, gray2, COLOR_BGR2GRAY);
    Ptr<SIFT> sift = SIFT::create(500);
    vector<KeyPoint> kp1, kp2; sift->detect(gray1, kp1); sift->detect(gray2, kp2);
    Mat d1(kp1.size(), 256, CV_32F), d2(kp2.size(), 256, CV_32F);
    for(size_t i=0; i<kp1.size(); i++) computeLBPDescriptor(gray1, kp1[i].pt).copyTo(d1.row(i));
    for(size_t i=0; i<kp2.size(); i++) computeLBPDescriptor(gray2, kp2[i].pt).copyTo(d2.row(i));
    vector<DMatch> good;
    for(size_t i=0; i<kp1.size(); i++) {
        double best=1e9, second=1e9; int idx=-1;
        for(size_t j=0; j<kp2.size(); j++) {
            double dist = compareHist(d1.row(i), d2.row(j), HISTCMP_CHISQR);
            if(dist < best) { second=best; best=dist; idx=j; }
            else if(dist < second) second=dist;
        }
        if(idx>=0 && second>0 && best < 0.75*second) good.push_back(DMatch(i, idx, best));
    }
    Mat res; drawMatches(img1, kp1, img2, kp2, good, res);
    putText(res, "Matches: " + to_string(good.size()), Point(10,30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0,255,0), 2);
    imwrite(outputPath, res);
    cout << "Saved: " << outputPath << endl;
}

void matchBlobLBPAuto(const string& img1Path, const string& img2Path, const string& outputPath) {
    Mat img1 = imread(img1Path), img2 = imread(img2Path);
    if(img1.empty() || img2.empty()) return;
    Mat gray1, gray2; cvtColor(img1, gray1, COLOR_BGR2GRAY); cvtColor(img2, gray2, COLOR_BGR2GRAY);
    SimpleBlobDetector::Params params;
    params.minThreshold = 10; params.maxThreshold = 220;
    params.filterByArea = true; params.minArea = 100;
    params.filterByCircularity = true; params.minCircularity = 0.04f;
    params.filterByConvexity = true; params.minConvexity = 0.58f;
    Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
    vector<KeyPoint> kp1, kp2; detector->detect(gray1, kp1); detector->detect(gray2, kp2);
    Mat d1(kp1.size(), 256, CV_32F), d2(kp2.size(), 256, CV_32F);
    for(size_t i=0; i<kp1.size(); i++) computeLBPDescriptor(gray1, kp1[i].pt).copyTo(d1.row(i));
    for(size_t i=0; i<kp2.size(); i++) computeLBPDescriptor(gray2, kp2[i].pt).copyTo(d2.row(i));
    vector<DMatch> good;
    for(size_t i=0; i<kp1.size(); i++) {
        double best=1e9, second=1e9; int idx=-1;
        for(size_t j=0; j<kp2.size(); j++) {
            double dist = compareHist(d1.row(i), d2.row(j), HISTCMP_CHISQR);
            if(dist < best) { second=best; best=dist; idx=j; }
            else if(dist < second) second=dist;
        }
        if(idx>=0 && second>0 && best < 0.75*second) good.push_back(DMatch(i, idx, best));
    }
    Mat res; drawMatches(img1, kp1, img2, kp2, good, res);
    putText(res, "Matches: " + to_string(good.size()), Point(10,30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0,255,0), 2);
    imwrite(outputPath, res);
    cout << "Saved: " << outputPath << endl;
}

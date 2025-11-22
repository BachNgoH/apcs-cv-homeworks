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

// Forward declarations
void showHelp();
void detectHarris(const string& imagePath);
void detectBlob(const string& imagePath);
void detectDoG(const string& imagePath);
void matchHarrisSIFT(const string& img1Path, const string& img2Path);
void matchDoGSIFT(const string& img1Path, const string& img2Path);
void matchBlobSIFT(const string& img1Path, const string& img2Path);
void matchHarrisLBP(const string& img1Path, const string& img2Path);
void matchDoGLBP(const string& img1Path, const string& img2Path);
void matchBlobLBP(const string& img1Path, const string& img2Path);

// LBP descriptor computation (shared)
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

// Manual Harris detection (from exercise_a)
vector<KeyPoint> detectHarrisKeypoints(const Mat& gray) {
    int blockSize = 2, apertureSize = 3;
    double k = 0.04;
    
    Mat Ix, Iy;
    Sobel(gray, Ix, CV_32F, 1, 0, apertureSize);
    Sobel(gray, Iy, CV_32F, 0, 1, apertureSize);
    
    Mat Ixx, Iyy, Ixy;
    multiply(Ix, Ix, Ixx);
    multiply(Iy, Iy, Iyy);
    multiply(Ix, Iy, Ixy);
    
    int kernelSize = blockSize * 2 + 1;
    Mat Sxx, Syy, Sxy;
    GaussianBlur(Ixx, Sxx, Size(kernelSize, kernelSize), 0);
    GaussianBlur(Iyy, Syy, Size(kernelSize, kernelSize), 0);
    GaussianBlur(Ixy, Sxy, Size(kernelSize, kernelSize), 0);
    
    Mat harris(gray.size(), CV_32F);
    for (int i = 0; i < gray.rows; i++) {
        for (int j = 0; j < gray.cols; j++) {
            float sxx = Sxx.at<float>(i,j), syy = Syy.at<float>(i,j), sxy = Sxy.at<float>(i,j);
            harris.at<float>(i,j) = sxx * syy - sxy * sxy - k * (sxx + syy) * (sxx + syy);
        }
    }
    
    Mat harrisNorm;
    normalize(harris, harrisNorm, 0, 255, NORM_MINMAX);
    Mat mask;
    threshold(harrisNorm, mask, 200, 255, THRESH_BINARY);
    mask.convertTo(mask, CV_8U);
    
    vector<Point> corners;
    findNonZero(mask, corners);
    
    vector<KeyPoint> keypoints;
    int limit = min(500, (int)corners.size());
    for (int i = 0; i < limit; i++) {
        keypoints.push_back(KeyPoint(corners[i].x, corners[i].y, 1));
    }
    return keypoints;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        showHelp();
        return -1;
    }
    
    string command = argv[1];
    
    if (command == "h") {
        showHelp();
    }
    else if (command == "harris" && argc == 3) {
        detectHarris(argv[2]);
    }
    else if (command == "blob" && argc == 3) {
        detectBlob(argv[2]);
    }
    else if (command == "dog" && argc == 3) {
        detectDoG(argv[2]);
    }
    else if (command == "m" && argc >= 5) {
        string detector = argv[2];
        string descriptor = argv[3];
        string img1 = argv[4];
        string img2 = (argc >= 6) ? argv[5] : "";
        
        if (img2.empty()) {
            cerr << "Error: Two images required for matching" << endl;
            return -1;
        }
        
        if (detector == "harris" && descriptor == "sift") {
            matchHarrisSIFT(img1, img2);
        }
        else if (detector == "dog" && descriptor == "sift") {
            matchDoGSIFT(img1, img2);
        }
        else if (detector == "blob" && descriptor == "sift") {
            matchBlobSIFT(img1, img2);
        }
        else if (detector == "harris" && descriptor == "lbp") {
            matchHarrisLBP(img1, img2);
        }
        else if (detector == "dog" && descriptor == "lbp") {
            matchDoGLBP(img1, img2);
        }
        else if (detector == "blob" && descriptor == "lbp") {
            matchBlobLBP(img1, img2);
        }
        else {
            cerr << "Unknown detector/descriptor combination" << endl;
            showHelp();
            return -1;
        }
    }
    else {
        cerr << "Invalid command" << endl;
        showHelp();
        return -1;
    }
    
    return 0;
}

void showHelp() {
    cout << "\n========== CV Feature Detection & Matching ==========" << endl;
    cout << "Usage: ./cvlab <command> [arguments]" << endl;
    cout << "\nDETECTION COMMANDS:" << endl;
    cout << "  harris <image.jpg>              - Detect Harris corners" << endl;
    cout << "  blob <image.jpg>                - Detect blobs" << endl;
    cout << "  dog <image.jpg>                 - Detect DoG keypoints" << endl;
    cout << "\nMATCHING COMMANDS:" << endl;
    cout << "  m harris sift <img1> <img2>     - Harris + SIFT matching" << endl;
    cout << "  m dog sift <img1> <img2>        - DoG + SIFT matching" << endl;
    cout << "  m blob sift <img1> <img2>       - Blob + SIFT matching" << endl;
    cout << "  m harris lbp <img1> <img2>      - Harris + LBP matching" << endl;
    cout << "  m dog lbp <img1> <img2>         - DoG + LBP matching" << endl;
    cout << "  m blob lbp <img1> <img2>        - Blob + LBP matching" << endl;
    cout << "\nOTHER:" << endl;
    cout << "  h                               - Show this help" << endl;
    cout << "\nKEYBOARD CONTROLS (in window):" << endl;
    cout << "  ESC/q - Quit" << endl;
    cout << "  s     - Save result" << endl;
    cout << "====================================================\n" << endl;
}

void detectHarris(const string& imagePath) {
    Mat img = imread(imagePath, IMREAD_COLOR);
    if (img.empty()) {
        cerr << "Error: Cannot open image " << imagePath << endl;
        return;
    }
    
    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);
    
    // Interactive parameters
    int blockSize = 2;
    int apertureSize = 1; // Will be 3, 5, 7
    int kValue = 4; // k * 0.01
    int threshold = 200;
    
    namedWindow("Harris Detection", WINDOW_AUTOSIZE);
    createTrackbar("Block Size", "Harris Detection", &blockSize, 10);
    createTrackbar("Aperture", "Harris Detection", &apertureSize, 3);
    createTrackbar("K x100", "Harris Detection", &kValue, 10);
    createTrackbar("Threshold", "Harris Detection", &threshold, 255);
    
    while (true) {
        int actualAperture = apertureSize * 2 + 3;
        if (actualAperture > 7) actualAperture = 7;
        if (blockSize < 2) blockSize = 2;
        double k = (kValue + 1) / 100.0;
        
        Mat Ix, Iy;
        Sobel(gray, Ix, CV_32F, 1, 0, actualAperture);
        Sobel(gray, Iy, CV_32F, 0, 1, actualAperture);
        
        Mat Ixx, Iyy, Ixy;
        multiply(Ix, Ix, Ixx);
        multiply(Iy, Iy, Iyy);
        multiply(Ix, Iy, Ixy);
        
        int kernelSize = blockSize * 2 + 1;
        Mat Sxx, Syy, Sxy;
        GaussianBlur(Ixx, Sxx, Size(kernelSize, kernelSize), 0);
        GaussianBlur(Iyy, Syy, Size(kernelSize, kernelSize), 0);
        GaussianBlur(Ixy, Sxy, Size(kernelSize, kernelSize), 0);
        
        Mat harris(gray.size(), CV_32F);
        for (int i = 0; i < gray.rows; i++) {
            for (int j = 0; j < gray.cols; j++) {
                float sxx = Sxx.at<float>(i,j), syy = Syy.at<float>(i,j), sxy = Sxy.at<float>(i,j);
                harris.at<float>(i,j) = sxx * syy - sxy * sxy - k * (sxx + syy) * (sxx + syy);
            }
        }
        
        Mat harrisNorm;
        normalize(harris, harrisNorm, 0, 255, NORM_MINMAX);
        Mat mask;
        cv::threshold(harrisNorm, mask, threshold, 255, THRESH_BINARY);
        mask.convertTo(mask, CV_8U);
        
        vector<Point> corners;
        findNonZero(mask, corners);
        
        Mat result = img.clone();
        for (const auto& corner : corners) {
            circle(result, corner, 3, Scalar(0, 0, 255), 2);
        }
        
        string info = "Corners: " + to_string(corners.size());
        putText(result, info, Point(10, 30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 0), 2);
        
        imshow("Harris Detection", result);
        
        int key = waitKey(30);
        if (key == 27 || key == 'q') break;
        else if (key == 's') {
            imwrite("harris_result.jpg", result);
            cout << "Saved harris_result.jpg" << endl;
        }
    }
}

void detectBlob(const string& imagePath) {
    Mat img = imread(imagePath, IMREAD_COLOR);
    if (img.empty()) return;
    
    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);
    
    int minThreshold = 10;
    int maxThreshold = 220;
    int minArea = 100;
    int minCircularity = 4;
    int minConvexity = 58;
    int minInertia = 10;
    
    namedWindow("Blob Detection", WINDOW_AUTOSIZE);
    createTrackbar("Min Threshold", "Blob Detection", &minThreshold, 255);
    createTrackbar("Max Threshold", "Blob Detection", &maxThreshold, 255);
    createTrackbar("Min Area", "Blob Detection", &minArea, 5000);
    createTrackbar("Circularity %", "Blob Detection", &minCircularity, 100);
    createTrackbar("Convexity %", "Blob Detection", &minConvexity, 100);
    createTrackbar("Inertia %", "Blob Detection", &minInertia, 100);
    
    while (true) {
        SimpleBlobDetector::Params params;
        params.minThreshold = minThreshold;
        params.maxThreshold = max(minThreshold + 10, maxThreshold);
        params.filterByArea = true;
        params.minArea = minArea;
        params.filterByCircularity = true;
        params.minCircularity = minCircularity / 100.0f;
        params.filterByConvexity = true;
        params.minConvexity = minConvexity / 100.0f;
        params.filterByInertia = true;
        params.minInertiaRatio = minInertia / 100.0f;
        
        Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
        vector<KeyPoint> keypoints;
        detector->detect(gray, keypoints);
        
        Mat result;
        drawKeypoints(img, keypoints, result, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        
        string info = "Blobs: " + to_string(keypoints.size());
        putText(result, info, Point(10, 30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0,255,0), 2);
        
        imshow("Blob Detection", result);
        
        int key = waitKey(30);
        if (key == 27 || key == 'q') break;
        else if (key == 's') {
            imwrite("blob_result.jpg", result);
            cout << "Saved blob_result.jpg" << endl;
        }
    }
}

void detectDoG(const string& imagePath) {
    Mat img = imread(imagePath, IMREAD_COLOR);
    if (img.empty()) return;
    
    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);
    
    int nFeatures = 500;
    int nOctaveLayers = 3;
    int contrastThreshold = 4;
    int edgeThreshold = 10;
    
    namedWindow("DoG Detection", WINDOW_AUTOSIZE);
    createTrackbar("Max Features", "DoG Detection", &nFeatures, 2000);
    createTrackbar("Octave Layers", "DoG Detection", &nOctaveLayers, 8);
    createTrackbar("Contrast x100", "DoG Detection", &contrastThreshold, 20);
    createTrackbar("Edge Threshold", "DoG Detection", &edgeThreshold, 20);
    
    while (true) {
        Ptr<SIFT> sift = SIFT::create(nFeatures, nOctaveLayers, contrastThreshold/100.0, edgeThreshold);
        vector<KeyPoint> keypoints;
        sift->detect(gray, keypoints);
        
        Mat result;
        drawKeypoints(img, keypoints, result, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        
        string info = "DoG Keypoints: " + to_string(keypoints.size());
        putText(result, info, Point(10, 30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0,255,0), 2);
        
        imshow("DoG Detection", result);
        
        int key = waitKey(30);
        if (key == 27 || key == 'q') break;
        else if (key == 's') {
            imwrite("dog_result.jpg", result);
            cout << "Saved dog_result.jpg" << endl;
        }
    }
}

void matchHarrisSIFT(const string& img1Path, const string& img2Path) {
    Mat img1 = imread(img1Path), img2 = imread(img2Path);
    if (img1.empty() || img2.empty()) return;
    
    Mat gray1, gray2;
    cvtColor(img1, gray1, COLOR_BGR2GRAY);
    cvtColor(img2, gray2, COLOR_BGR2GRAY);
    
    vector<KeyPoint> kp1 = detectHarrisKeypoints(gray1);
    vector<KeyPoint> kp2 = detectHarrisKeypoints(gray2);
    
    Ptr<SIFT> sift = SIFT::create();
    Mat desc1, desc2;
    sift->compute(gray1, kp1, desc1);
    sift->compute(gray2, kp2, desc2);
    
    BFMatcher matcher(NORM_L2);
    vector<vector<DMatch>> knnMatches;
    matcher.knnMatch(desc1, desc2, knnMatches, 2);
    
    vector<DMatch> good;
    for (auto& m : knnMatches) {
        if (m.size() >= 2 && m[0].distance < 0.75f * m[1].distance) {
            good.push_back(m[0]);
        }
    }
    
    Mat result;
    drawMatches(img1, kp1, img2, kp2, good, result);
    putText(result, "Matches: " + to_string(good.size()), Point(10,30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0,255,0), 2);
    
    namedWindow("Harris+SIFT", WINDOW_NORMAL);
    imshow("Harris+SIFT", result);
    while (waitKey(30) != 27);
}

void matchDoGSIFT(const string& img1Path, const string& img2Path) {
    Mat img1 = imread(img1Path), img2 = imread(img2Path);
    if (img1.empty() || img2.empty()) return;
    
    Mat gray1, gray2;
    cvtColor(img1, gray1, COLOR_BGR2GRAY);
    cvtColor(img2, gray2, COLOR_BGR2GRAY);
    
    Ptr<SIFT> sift = SIFT::create();
    vector<KeyPoint> kp1, kp2;
    Mat desc1, desc2;
    sift->detectAndCompute(gray1, Mat(), kp1, desc1);
    sift->detectAndCompute(gray2, Mat(), kp2, desc2);
    
    BFMatcher matcher(NORM_L2);
    vector<vector<DMatch>> knnMatches;
    matcher.knnMatch(desc1, desc2, knnMatches, 2);
    
    vector<DMatch> good;
    for (auto& m : knnMatches) {
        if (m.size() >= 2 && m[0].distance < 0.75f * m[1].distance) {
            good.push_back(m[0]);
        }
    }
    
    Mat result;
    drawMatches(img1, kp1, img2, kp2, good, result);
    putText(result, "Matches: " + to_string(good.size()), Point(10,30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0,255,0), 2);
    
    namedWindow("DoG+SIFT", WINDOW_NORMAL);
    imshow("DoG+SIFT", result);
    while (waitKey(30) != 27);
}

void matchBlobSIFT(const string& img1Path, const string& img2Path) {
    Mat img1 = imread(img1Path), img2 = imread(img2Path);
    if (img1.empty() || img2.empty()) return;
    
    Mat gray1, gray2;
    cvtColor(img1, gray1, COLOR_BGR2GRAY);
    cvtColor(img2, gray2, COLOR_BGR2GRAY);
    
    SimpleBlobDetector::Params params;
    params.minThreshold = 10;
    params.maxThreshold = 220;
    params.filterByArea = true;
    params.minArea = 100;
    params.filterByCircularity = true;
    params.minCircularity = 0.04f;
    params.filterByConvexity = true;
    params.minConvexity = 0.58f;
    
    Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
    vector<KeyPoint> kp1, kp2;
    detector->detect(gray1, kp1);
    detector->detect(gray2, kp2);
    
    Ptr<SIFT> sift = SIFT::create();
    Mat desc1, desc2;
    sift->compute(gray1, kp1, desc1);
    sift->compute(gray2, kp2, desc2);
    
    BFMatcher matcher(NORM_L2);
    vector<vector<DMatch>> knnMatches;
    matcher.knnMatch(desc1, desc2, knnMatches, 2);
    
    vector<DMatch> good;
    for (auto& m : knnMatches) {
        if (m.size() >= 2 && m[0].distance < 0.75f * m[1].distance) {
            good.push_back(m[0]);
        }
    }
    
    Mat result;
    drawMatches(img1, kp1, img2, kp2, good, result);
    putText(result, "Matches: " + to_string(good.size()), Point(10,30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0,255,0), 2);
    
    namedWindow("Blob+SIFT", WINDOW_NORMAL);
    imshow("Blob+SIFT", result);
    while (waitKey(30) != 27);
}

void matchHarrisLBP(const string& img1Path, const string& img2Path) {
    Mat img1 = imread(img1Path), img2 = imread(img2Path);
    if (img1.empty() || img2.empty()) return;
    
    Mat gray1, gray2;
    cvtColor(img1, gray1, COLOR_BGR2GRAY);
    cvtColor(img2, gray2, COLOR_BGR2GRAY);
    
    vector<KeyPoint> kp1 = detectHarrisKeypoints(gray1);
    vector<KeyPoint> kp2 = detectHarrisKeypoints(gray2);
    
    Mat desc1(kp1.size(), 256, CV_32F), desc2(kp2.size(), 256, CV_32F);
    for (size_t i = 0; i < kp1.size(); i++) {
        Mat d = computeLBPDescriptor(gray1, kp1[i].pt);
        if (!d.empty()) d.copyTo(desc1.row(i));
    }
    for (size_t i = 0; i < kp2.size(); i++) {
        Mat d = computeLBPDescriptor(gray2, kp2[i].pt);
        if (!d.empty()) d.copyTo(desc2.row(i));
    }
    
    vector<DMatch> good;
    for (size_t i = 0; i < kp1.size(); i++) {
        double best = 1e9, second = 1e9;
        int idx = -1;
        for (size_t j = 0; j < kp2.size(); j++) {
            double dist = compareHist(desc1.row(i), desc2.row(j), HISTCMP_CHISQR);
            if (dist < best) { second = best; best = dist; idx = j; }
            else if (dist < second) second = dist;
        }
        if (idx >= 0 && second > 0 && best < 0.75 * second) {
            good.push_back(DMatch(i, idx, best));
        }
    }
    
    Mat result;
    drawMatches(img1, kp1, img2, kp2, good, result);
    putText(result, "Matches: " + to_string(good.size()), Point(10,30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0,255,0), 2);
    
    namedWindow("Harris+LBP", WINDOW_NORMAL);
    imshow("Harris+LBP", result);
    while (waitKey(30) != 27);
}

void matchDoGLBP(const string& img1Path, const string& img2Path) {
    Mat img1 = imread(img1Path), img2 = imread(img2Path);
    if (img1.empty() || img2.empty()) return;
    
    Mat gray1, gray2;
    cvtColor(img1, gray1, COLOR_BGR2GRAY);
    cvtColor(img2, gray2, COLOR_BGR2GRAY);
    
    Ptr<SIFT> sift = SIFT::create(500);
    vector<KeyPoint> kp1, kp2;
    sift->detect(gray1, kp1);
    sift->detect(gray2, kp2);
    
    Mat desc1(kp1.size(), 256, CV_32F), desc2(kp2.size(), 256, CV_32F);
    for (size_t i = 0; i < kp1.size(); i++) {
        Mat d = computeLBPDescriptor(gray1, kp1[i].pt);
        if (!d.empty()) d.copyTo(desc1.row(i));
    }
    for (size_t i = 0; i < kp2.size(); i++) {
        Mat d = computeLBPDescriptor(gray2, kp2[i].pt);
        if (!d.empty()) d.copyTo(desc2.row(i));
    }
    
    vector<DMatch> good;
    for (size_t i = 0; i < kp1.size(); i++) {
        double best = 1e9, second = 1e9;
        int idx = -1;
        for (size_t j = 0; j < kp2.size(); j++) {
            double dist = compareHist(desc1.row(i), desc2.row(j), HISTCMP_CHISQR);
            if (dist < best) { second = best; best = dist; idx = j; }
            else if (dist < second) second = dist;
        }
        if (idx >= 0 && second > 0 && best < 0.75 * second) {
            good.push_back(DMatch(i, idx, best));
        }
    }
    
    Mat result;
    drawMatches(img1, kp1, img2, kp2, good, result);
    putText(result, "Matches: " + to_string(good.size()), Point(10,30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0,255,0), 2);
    
    namedWindow("DoG+LBP", WINDOW_NORMAL);
    imshow("DoG+LBP", result);
    while (waitKey(30) != 27);
}

void matchBlobLBP(const string& img1Path, const string& img2Path) {
    Mat img1 = imread(img1Path), img2 = imread(img2Path);
    if (img1.empty() || img2.empty()) return;
    
    Mat gray1, gray2;
    cvtColor(img1, gray1, COLOR_BGR2GRAY);
    cvtColor(img2,gray2, COLOR_BGR2GRAY);
    
    SimpleBlobDetector::Params params;
    params.minThreshold = 10;
    params.maxThreshold = 220;
    params.filterByArea = true;
    params.minArea = 100;
    params.filterByCircularity = true;
    params.minCircularity = 0.04f;
    params.filterByConvexity = true;
    params.minConvexity = 0.58f;
    
    Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
    vector<KeyPoint> kp1, kp2;
    detector->detect(gray1, kp1);
    detector->detect(gray2, kp2);
    
    Mat desc1(kp1.size(), 256, CV_32F), desc2(kp2.size(), 256, CV_32F);
    for (size_t i = 0; i < kp1.size(); i++) {
        Mat d = computeLBPDescriptor(gray1, kp1[i].pt);
        if (!d.empty()) d.copyTo(desc1.row(i));
    }
    for (size_t i = 0; i < kp2.size(); i++) {
        Mat d = computeLBPDescriptor(gray2, kp2[i].pt);
        if (!d.empty()) d.copyTo(desc2.row(i));
    }
    
    vector<DMatch> good;
    for (size_t i = 0; i < kp1.size(); i++) {
        double best = 1e9, second = 1e9;
        int idx = -1;
        for (size_t j = 0; j < kp2.size(); j++) {
            double dist = compareHist(desc1.row(i), desc2.row(j), HISTCMP_CHISQR);
            if (dist < best) { second = best; best = dist; idx = j; }
            else if (dist < second) second = dist;
        }
        if (idx >= 0 && second > 0 && best < 0.75 * second) {
            good.push_back(DMatch(i, idx, best));
        }
    }
    
    Mat result;
    drawMatches(img1, kp1, img2, kp2, good, result);
    putText(result, "Matches: " + to_string(good.size()), Point(10,30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0,255,0), 2);
    
    namedWindow("Blob+LBP", WINDOW_NORMAL);
    imshow("Blob+LBP", result);
    while (waitKey(30) != 27);
}

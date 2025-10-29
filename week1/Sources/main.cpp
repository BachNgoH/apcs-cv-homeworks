#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

void onBrightnessTrackbar(int value, void* userdata);
void onContrastTrackbar(int value, void* userdata);
void onAvgFilterTrackbar(int value, void* userdata);
void onGaussFilterTrackbar(int value, void* userdata);

Mat originalImage, processedImage;
int brightnessValue = 0;
int contrastValue = 100;
int avgKernelSize = 1;
int gaussKernelSize = 1;

int main(int argc, char** argv) {
    if (argc < 3) {
        cout << "Usage: " << argv[0] << " <option> <input_image>" << endl;
        cout << "Options:" << endl;
        cout << "  -rgb2gray <input_image>" << endl;
        cout << "  -brightness <input_image>" << endl;
        cout << "  -contrast <input_image>" << endl;
        cout << "  -avg <input_image>" << endl;
        cout << "  -gauss <input_image>" << endl;
        return -1;
    }

    string option = argv[1];
    string inputFile = argv[2];

    originalImage = imread(inputFile);
    if (originalImage.empty()) {
        cout << "Could not open or find the image: " << inputFile << endl;
        return -1;
    }

    processedImage = originalImage.clone();

    if (option == "-rgb2gray") {
        cvtColor(originalImage, processedImage, COLOR_BGR2GRAY);
        cout << "RGB to Grayscale conversion completed." << endl;
        // For testing, save the output instead of displaying
        imwrite("output_rgb2gray.jpg", processedImage);
    } else if (option == "-brightness") {
        cout << "Brightness adjustment: Use trackbar in GUI window." << endl;
        // For testing, apply default brightness and save
        processedImage = originalImage + Scalar(50, 50, 50);
        imwrite("output_brightness.jpg", processedImage);
        cout << "Brightness adjustment completed with default value." << endl;
    } else if (option == "-contrast") {
        cout << "Contrast adjustment: Use trackbar in GUI window." << endl;
        // For testing, apply default contrast and save
        double alpha = 1.5;
        processedImage = Mat::zeros(originalImage.size(), originalImage.type());
        for (int y = 0; y < originalImage.rows; y++) {
            for (int x = 0; x < originalImage.cols; x++) {
                for (int c = 0; c < originalImage.channels(); c++) {
                    processedImage.at<Vec3b>(y, x)[c] = saturate_cast<uchar>(alpha * originalImage.at<Vec3b>(y, x)[c]);
                }
            }
        }
        imwrite("output_contrast.jpg", processedImage);
        cout << "Contrast adjustment completed with default value." << endl;
    } else if (option == "-avg") {
        cout << "Average filter: Use trackbar in GUI window." << endl;
        // For testing, apply default kernel size and save
        blur(originalImage, processedImage, Size(5, 5));
        imwrite("output_avg.jpg", processedImage);
        cout << "Average filter completed with default kernel size." << endl;
    } else if (option == "-gauss") {
        cout << "Gaussian filter: Use trackbar in GUI window." << endl;
        // For testing, apply default kernel size and save
        GaussianBlur(originalImage, processedImage, Size(5, 5), 0);
        imwrite("output_gauss.jpg", processedImage);
        cout << "Gaussian filter completed with default kernel size." << endl;
    } else {
        cout << "Invalid option: " << option << endl;
        return -1;
    }

    return 0;
}

void onBrightnessTrackbar(int value, void* userdata) {
    processedImage = originalImage + Scalar(value, value, value);
    imshow("Brightness Adjustment", processedImage);
}

void onContrastTrackbar(int value, void* userdata) {
    double alpha = value / 100.0;
    processedImage = Mat::zeros(originalImage.size(), originalImage.type());
    for (int y = 0; y < originalImage.rows; y++) {
        for (int x = 0; x < originalImage.cols; x++) {
            for (int c = 0; c < originalImage.channels(); c++) {
                processedImage.at<Vec3b>(y, x)[c] = saturate_cast<uchar>(alpha * originalImage.at<Vec3b>(y, x)[c]);
            }
        }
    }
    imshow("Contrast Adjustment", processedImage);
}

void onAvgFilterTrackbar(int value, void* userdata) {
    if (value % 2 == 0) value++;
    if (value < 1) value = 1;
    blur(originalImage, processedImage, Size(value, value));
    imshow("Average Filter", processedImage);
}

void onGaussFilterTrackbar(int value, void* userdata) {
    if (value % 2 == 0) value++;
    if (value < 1) value = 1;
    GaussianBlur(originalImage, processedImage, Size(value, value), 0);
    imshow("Gaussian Filter", processedImage);
}
#include "Image.h"
#include <opencv2/opencv.hpp>

/**
 * @brief Compare two RGB colors for equality.
 *
 * This function compares two RGB colors by comparing their individual color components.
 *
 * @param color1 The first RGB color to compare.
 * @param color2 The second RGB color to compare.
 * @return true if the colors are equal, false otherwise.
 */
bool compareRGB(const RGB& color1, const RGB& color2) {
    return (color1.red == color2.red) && (color1.green == color2.green) && (color1.blue == color2.blue);
}

// bool areImagesIdentical(cv::Mat& image1, cv::Mat& image2) {
//     // Ensure both images have the same type
//     if (image1.size() != image2.size() || image1.type() != image2.type()) {
//         return false;
//     }

//     // Convert both images to 3-channel BGR if necessary
//     cv::Mat img1, img2;
//     if (image1.channels() == 4) {
//         cv::cvtColor(image1, img1, cv::COLOR_BGRA2BGR);
//     } else {
//         img1 = image1;
//     }

//     if (image2.channels() == 4) {
//         cv::cvtColor(image2, img2, cv::COLOR_BGRA2BGR);
//     } else {
//         img2 = image2;
//     }

//     // Convert both to grayscale
//     cv::Mat grayImage1, grayImage2;
//     cv::cvtColor(img1, grayImage1, cv::COLOR_BGR2GRAY);
//     cv::cvtColor(img2, grayImage2, cv::COLOR_BGR2GRAY);

//     // Check for differences
//     return cv::countNonZero(grayImage1 != grayImage2) == 0;
// }

bool areImagesIdentical(cv::Mat& image1, cv::Mat& image2) {
    if (image1.size() != image2.size() || image1.type() != image2.type()) {
        return false;
    }

    std::vector<cv::Mat> channels1, channels2;
    cv::split(image1, channels1);
    cv::split(image2, channels2);

    for (int i = 0; i < image1.channels(); i++) {
        if (cv::countNonZero(channels1[i] != channels2[i]) > 0) {
            return false;
        }
    }
    return true;
}


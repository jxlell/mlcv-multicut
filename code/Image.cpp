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

// bool areImagesIdentical(cv::Mat& image1, cv::Mat& image2) {
//     if (image1.size() != image2.size()) {
//         std::cout << "Image sizes do not match." << std::endl;
//         return false;
//     }

//     int minChannels = std::min(image1.channels(), image2.channels()); // Compare only common channels

//     std::vector<cv::Mat> channels1, channels2;
//     cv::split(image1, channels1);
//     cv::split(image2, channels2);

//     bool identical = true;

//     for (int i = 0; i < minChannels; i++) {  
//         bool sizeMismatch = channels1[i].size() != channels2[i].size();
//         bool typeMismatch = channels1[i].type() != channels2[i].type();

//         if (sizeMismatch || typeMismatch) {
//             std::cout << "Channel " << i << " mismatch: ";
//             if (sizeMismatch) {
//                 std::cout << "size mismatch ("
//                           << channels1[i].size() << " vs " << channels2[i].size() << ")";
//             }
//             if (sizeMismatch && typeMismatch) {
//                 std::cout << ", ";
//             }
//             if (typeMismatch) {
//                 std::cout << "type mismatch ("
//                           << channels1[i].type() << " vs " << channels2[i].type() << ")";
//             }
//             std::cout << std::endl;

//             identical = false;
//             continue;  
//         }

//         cv::Mat diff;
//         cv::compare(channels1[i], channels2[i], diff, cv::CMP_NE);

//         if (cv::countNonZero(diff) > 0) {
//             std::cout << "Difference found in channel " << i << std::endl;
//             identical = false;

//             // Print up to 10 different pixel values
//             int diffCount = 0;
//             for (int row = 0; row < diff.rows; row++) {
//                 for (int col = 0; col < diff.cols; col++) {
//                     if (diff.at<uchar>(row, col) > 0) {  // Found a mismatch
//                         int pixelIndex = row * diff.cols + col; // Calculate pixel index
//                         std::cout << "Pixel (" << row << ", " << col << ") [Index: " << pixelIndex << "] - "
//                                   << "Img1: " << static_cast<int>(channels1[i].at<uchar>(row, col)) << " | "
//                                   << "Img2: " << static_cast<int>(channels2[i].at<uchar>(row, col)) 
//                                   << std::endl;
//                         diffCount++;
//                         if (diffCount >= 10) break; // Limit to 10 differences
//                     }
//                 }
//                 if (diffCount >= 10) break;
//             }
//         }
//     }

//     return identical;
// }
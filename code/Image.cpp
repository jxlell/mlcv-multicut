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

bool areImagesIdentical(cv::Mat& image1, cv::Mat& image2) {
    // Check if the images have the same size and type
    if (image1.size() != image2.size() || image1.type() != image2.type()) {
        return false;
    }

    // Convert the images to grayscale
    cv::Mat grayImage1, grayImage2;
    cv::cvtColor(image1, grayImage1, cv::COLOR_BGR2GRAY);
    cv::cvtColor(image2, grayImage2, cv::COLOR_BGR2GRAY);

    // Check if there are any non-zero elements (differences) between the images
    return cv::countNonZero(grayImage1 != grayImage2) == 0;
}
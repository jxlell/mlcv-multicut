// quantization.h

#ifndef QUANTIZATION_H
#define QUANTIZATION_H

#include <opencv2/opencv.hpp>
#include <map>
#include <algorithm>
#include <iostream>

// Custom comparator for Vec<unsigned char, 3>
struct VecComparator {
    bool operator()(const cv::Vec<unsigned char, 3>& a, const cv::Vec<unsigned char, 3>& b) const {
        return std::lexicographical_compare(a.val, a.val + 3, b.val, b.val + 3);
    }
};


class Quantizer {
public:
    Quantizer(const std::string& imagePath);
    std::map<cv::Vec<unsigned char, 3>, cv::Vec<unsigned char, 3>, VecComparator> performQuantization();
    void displayResults();
    // Add any other necessary functions or member variables
private:
    cv::Mat image;
    cv::Mat quantizedImage;
    std::map<cv::Vec<unsigned char, 3>, cv::Vec<unsigned char, 3>, VecComparator> colorMapping;
};

#endif // QUANTIZATION_H

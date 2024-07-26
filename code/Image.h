#ifndef IMAGE_H
#define IMAGE_H

#include <cstdint>
#include <opencv2/opencv.hpp>

struct RGB {

   
    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;
    
};

bool compareRGB(const RGB& color1, const RGB& color2);
bool areImagesIdentical(cv::Mat& image1, cv::Mat& image2);

#endif // IMAGE_H
#ifndef IMAGE_H
#define IMAGE_H

#include <cstdint>
#include <opencv2/opencv.hpp>

struct RGB {

   
    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;
    
};

struct RGBComparator {
    bool operator()(const RGB& color1, const RGB& color2) const {
        if (color1.red != color2.red) {
            return color1.red < color2.red;
        }
        if (color1.green != color2.green) {
            return color1.green < color2.green;
        }
        return color1.blue < color2.blue;
    }
};

bool compareRGB(const RGB& color1, const RGB& color2);
bool areImagesIdentical(cv::Mat& image1, cv::Mat& image2);

#endif // IMAGE_H
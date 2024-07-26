#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <iostream>
#include "Multicut.h"
#include <opencv2/opencv.hpp>
#include "DirectionPath.h"
#include "Image.h"


class Compressor {
    public:
        Compressor(const std::string& imagePath);
        std::pair<std::vector<RGB>, PathInfoVector> compressImage();
        void setVertexColors();
        void setEdgeBits();
        void setRegions();
        void setPaths();
        Multicut getMulticut();
    private:
        std::string imagePath;
        std::vector<int> neighborsOffsets;
        cv::Mat img;
        Multicut multicut;
        int vertices;
};

#endif // COMPRESSOR_H
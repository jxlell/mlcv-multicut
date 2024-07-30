#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <iostream>
#include "Multicut.h"
#include <opencv2/opencv.hpp>
#include "DirectionPath.h"
#include "Image.h"


class Compressor {
    public:
        Compressor(const std::string& imagePath, const std::string& volumePath = "");
        std::pair<std::vector<RGB>, PathInfoVector> compressImage();
        void setVertexColors();
        void setEdgeBits();
        void setRegions();
        void setPaths();
        andres::Partition<int> getRegionsFromVolume(std::vector<Multicut> volume);
        Multicut getMulticut();
        void compressVolume();
    private:
        std::string imagePath;
        std::vector<int> neighborsOffsets;
        cv::Mat img;
        std::vector<cv::Mat> volume; 
        Multicut multicut;
        int vertices;
        std::string volumePath;
};

#endif // COMPRESSOR_H
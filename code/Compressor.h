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
        std::tuple<std::vector<RGB>, PathInfoVector, cv::Mat, PathInfoVector, RLEVector, Straights, std::vector<bool>> compressImage();
        void setVertexColors();
        void setEdgeBits();
        void setRegions();
        void setPaths();
        void set2BitPaths();
        Straights setStraights();
        andres::Partition<int> getRegionsFromVolume(std::vector<Multicut> volume);
        Multicut getMulticut();
        void compressVolume();
        long long getCompressionTime();
        double getCompressionRate();
        double getOldCompressionRates();
        double getRLECompressionRate();
        double getStraightsCompressionRate();
        double getMulticutPercentage();
        double getDisconnectedComponents();
        int getImgSize();
        int getkBSize();
        void print2bitpaths(PathInfoVector paths);
    private:
        std::string imagePath;
        std::vector<int> neighborsOffsets;
        cv::Mat img;
        std::vector<cv::Mat> volume; 
        Multicut multicut;
        int vertices;
        std::string volumePath;
        long long compressionTime;
        int imgSize;
        RLEVector rle_paths;
        Straights straights;
};

#endif // COMPRESSOR_H
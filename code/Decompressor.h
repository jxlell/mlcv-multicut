#ifndef DECOMPRESSOR_H
#define DECOMPRESSOR_H

#include <iostream>
#include "DirectionPath.h"
#include "Image.h"
#include <opencv2/opencv.hpp>


class Decompressor {
    public:
        Decompressor(std::vector<RGB> regionColors, PathInfoVector paths, int edgeBitsSize, int rows, int cols, cv::Mat img, PathInfoVector paths_2bit, RLEVector rle_paths);
        void reconstructImage();
        void reconstruct_edgeBits_iterative(int startEdge, Direction currentDir, std::vector<bool>& directionVector, std::vector<bool>& reconstructedEdgeBits);
        //void reconstructMulticut();
        long long getDecompressionTime() const;
        std::vector<bool> reconstruct_edgeBits2bits(PathInfoVector paths);
    private:
        std::string imagePath;
        std::vector<int> neighborsOffsets;
        std::vector<RGB> regionColors;
        PathInfoVector paths;
        PathInfoVector paths_2bit;
        RLEVector rle_paths;
        std::vector<bool> reconstructed_edgeBits;
        int edgeBitsSize;
        int rows;
        int cols;
        cv::Mat img;
        long long decompressionTime;
};

#endif // DECOMPRESSOR_H
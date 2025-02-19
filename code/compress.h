#ifndef COMPRESS_H
#define COMPRESS_H

#include <vector>
#include "Util.h"
#include "DirectionPath.h"
#include <opencv2/opencv.hpp>
#include "Image.h"
#include "partition.hxx"


struct CompressedImage{
    std::vector<RGB> colorVector;
    PathInfoVector paths;
    cv::Mat originalImage;
    PathInfoVector pathInfoVector2bit;
    RLEVector rleVector;
    Straights straights;
    std::vector<bool> regionColorBitString;
};

//std::tuple<std::vector<RGB>, PathInfoVector, cv::Mat, PathInfoVector, RLEVector, Straights, std::vector<bool>> compress(const std::string& imagePath);
CompressedImage compress(const std::string& imagePath);
std::vector<bool> setEdgeBits(cv::Mat img, std::vector<bool> edgeBits01, std::vector<int> neighborsOffsets);
PathInfoVector setPaths(std::vector<bool> edgeBits01, cv::Mat img);
std::tuple<PathInfoVector, RLEVector> set2BitPaths(std::vector<bool> edgeBits01, cv::Mat img);
Straights setStraights(std::vector<bool> edgeBits01, cv::Mat img);
std::vector<RGB> setRegions(cv::Mat img, std::vector<int> neighborsOffsets, int vertices);
double getCompressionRate(std::vector<RGB> regionColors, PathInfoVector paths, cv::Mat img);
double getRLECompressionRate(std::vector<RGB> regionColors, RLEVector rle_paths, cv::Mat img);
double getOldCompressionRate(cv::Mat img, std::vector<RGB> regionColors);
double getStraightsCompressionRate(std::vector<RGB> regionColors, Straights straights, cv::Mat img);
double getMulticutPercentage(std::vector<bool> edgeBits01);



#endif // COMPRESS_H
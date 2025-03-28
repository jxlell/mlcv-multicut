#ifndef COMPRESS_H
#define COMPRESS_H

#include <vector>
#include "Util.h"
#include "DirectionPath.h"
#include <opencv2/opencv.hpp>
#include "Image.h"
#include "partition.hxx"
#include "huffman.h"


struct CompressedImage{
    std::vector<RGB> colorVector;
    std::vector<bool> edgeBits01;
    PathInfoVector paths;
    std::vector<bool> pathsBitString;
    cv::Mat originalImage;
    PathInfoVector pathInfoVector2bit;
    RLEVector rleVector;
    std::vector<bool> rleBitString;
    Straights straights;
    std::vector<bool> regionColorBitString;

    std::vector<bool> straightsHuffmanCodesBitString;
    std::vector<uint32_t> straightsHuffmanCodesStartPoints;
    HuffmanNode* root; 
    std::vector<uint16_t> straightLengthsList;
    std::vector<uint32_t> straightLengthFrequencies;
    std::vector<bool> straightsBitString;

    std::vector<bool> horizontalBits;
    std::vector<bool> reducedVerticalBits;

    double pathCompressionRate;
    double rleCompressionRate;
    double oldCompressionRate;
    double straightsCompressionRate;
    double straightsHuffmanCompressionRate; 
    double newEdgeBitsCompressionRate;
};

//std::tuple<std::vector<RGB>, PathInfoVector, cv::Mat, PathInfoVector, RLEVector, Straights, std::vector<bool>> compress(const std::string& imagePath);
CompressedImage compress(const std::string& imagePath);
std::vector<bool> setEdgeBits(cv::Mat img, std::vector<bool> edgeBits01, std::vector<int> neighborsOffsets);
std::vector<bool> setVerticalBits(cv::Mat img);
std::vector<bool> reduceVerticalBits(cv::Mat img, std::vector<bool>& edgeBits01);
std::vector<bool> reduceHorizontalBits(cv::Mat img, std::vector<bool>& edgeBits01);
std::vector<bool> setHorizontalBits(cv::Mat img);
PathInfoVector setPaths(std::vector<bool> edgeBits01, cv::Mat img);
std::tuple<PathInfoVector, RLEVector> set2BitPaths(std::vector<bool> edgeBits01, cv::Mat img);
std::tuple<Straights, std::vector<bool>, std::vector<uint32_t>, HuffmanNode*, std::vector<uint16_t>, std::vector<uint32_t>> setStraights(std::vector<bool> edgeBits01, cv::Mat img);
std::vector<RGB> setRegions(cv::Mat img, std::vector<int> neighborsOffsets, int vertices);
double getCompressionRate(std::vector<RGB> regionColors, PathInfoVector paths, cv::Mat img);
double getRLECompressionRate(std::vector<RGB> regionColors, RLEVector rle_paths, cv::Mat img);
double getOldCompressionRate(cv::Mat img, std::vector<RGB> regionColors);
double getStraightsCompressionRate(std::vector<RGB> regionColors, Straights straights, cv::Mat img);
double getMulticutPercentage(std::vector<bool> edgeBits01);
void getAnomalies(std::vector<bool> edgeBits01, cv::Mat img);



#endif // COMPRESS_H
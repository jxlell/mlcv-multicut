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
    methods compressionMethods;
    std::vector<RGB> regionColors;
    std::vector<bool> edgeBits01;
    std::vector<bool> edgeBitsBitString;
    PathInfoVector paths;
    std::vector<bool> pathsBitString;
    cv::Mat originalImage;
    PathInfoVector pathInfoVector2bit;
    std::vector<bool> paths2bitBitString;
    std::vector<bool> paths2bitRLEBitString;
    RLEVector rleVector;
    // std::vector<bool> rleBitString;
    Straights straights;
    std::vector<bool> straightsNoHuffBitString;
    std::vector<bool> regionColorBitString;

    std::vector<bool> straightsHuffmanCodesBitString;
    std::vector<uint32_t> straightsHuffmanCodesStartPoints;
    HuffmanNode* root; 
    std::vector<uint16_t> straightLengthsList;
    std::vector<uint32_t> straightLengthFrequencies;
    std::vector<bool> straightsBitString;

    std::vector<bool> horizontalBits;
    std::vector<bool> reducedVerticalBits;
    std::vector<bool> reducedEdgeBitsBitString;

    double multicutPercentage;
    long long total_tree_bits;

    double treeCompressionRate;
    double rleCompressionRate;
    double oldCompressionRate;
    double straightsCompressionRate;
    double straightsHuffmanCompressionRate; 
    double reducedEdgeBitsCompressionRate;
    double paths2bit_compression_rate;
    std::vector<uint8_t> transparencyValues;

    long long tree_compression_time;
    long long old_compression_time;
    long long rle_compression_time;
    long long straights_compression_time;
    long long straights_huffman_compression_time;
    long long reduced_edgebits_compression_time;
    long long paths2bit_compression_time;

    int threeBitCount;
    int currentTreeDirectionBits;
    int tree_start_bits;
    int disconnectedComponents;
    int paths2bit_direction_bits;
    int paths2bit_start_bits;
    int paths2bit_components;
    int rle_direction_bits;
    int region_colors_bits;
    int dpcm_huffman_bits;
    int deflate_bits;
    int deflate_bits_unseparated;
    int new2bitDirectionBits;

    int bitsfortransferingcodes;
    int bitsfortransferingfrequencymap;

    long long read_img_time;
    long long setEdgeBitsTime;
    int region_color_dfs_time;
    int region_color_UF_time;
    int dpcm_huffman_time;
    int dpcm_huffman_bitstring_time;
    int tree_construction_time;
    int tree_bitstring_time;
};

//std::tuple<std::vector<RGB>, PathInfoVector, cv::Mat, PathInfoVector, RLEVector, Straights, std::vector<bool>> compress(const std::string& imagePath);
CompressedImage compress(const std::string& imagePath);
std::vector<bool> setEdgeBits(cv::Mat img, std::vector<bool> edgeBits01, std::vector<int> neighborsOffsets);
std::vector<bool> setVerticalBits(cv::Mat img);
std::vector<bool> reduceVerticalBits(cv::Mat img, std::vector<bool>& edgeBits01);
std::vector<bool> reduceHorizontalBits(cv::Mat img, std::vector<bool>& edgeBits01);
std::vector<bool> setHorizontalBits(cv::Mat img);
std::tuple<PathInfoVector,int> setPaths(std::vector<bool> edgeBits01, cv::Mat img);
std::tuple<PathInfoVector, RLEVector, int> set2BitPaths(std::vector<bool> edgeBits01, cv::Mat img);
std::tuple<Straights, std::vector<bool>, std::vector<uint32_t>, HuffmanNode*, std::vector<uint16_t>, std::vector<uint32_t>, map<int,string>> setStraights(std::vector<bool> edgeBits01, cv::Mat img);
std::vector<RGB> setRegions(cv::Mat img, std::vector<int> neighborsOffsets, int vertices);
double getCompressionRate(std::vector<RGB> regionColors, PathInfoVector paths, cv::Mat img);
double getRLECompressionRate(std::vector<RGB> regionColors, RLEVector rle_paths, cv::Mat img);
double getOldCompressionRate(cv::Mat img, std::vector<RGB> regionColors);
double getStraightsCompressionRate(std::vector<RGB> regionColors, Straights straights, cv::Mat img);
double getMulticutPercentage(std::vector<bool> edgeBits01);
void getAnomalies(std::vector<bool> edgeBits01, cv::Mat img);
std::vector<RGB> getRegionsFromImageSearch(cv::Mat img, std::vector<bool>& edgeBitsFromDFS);
std::vector<RGB> dpcm(std::vector<RGB> colors);
std::vector<RGB> dpcm_modified(std::vector<RGB> colors);
std::vector<uint8_t> flatten_colors(std::vector<RGB> differences);
std::map<uint8_t, int> createFrequencyMap(const std::vector<uint8_t>& differences);
void testZlib();
std::vector<uint8_t> ZlibDeflate(const std::vector<uint8_t>& input);

#endif // COMPRESS_H
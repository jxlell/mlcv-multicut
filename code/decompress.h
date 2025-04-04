#ifndef DECOMPRESS_H
#define DECOMPRESS_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include "DirectionPath.h"
#include "compress.h"

struct decompInfo{
    bool success;
    long long tree_decompression_time;
    long long old_decompression_time;
    long long rle_decompression_time;
    long long straights_huffman_decompression_time;
    long long reduced_edgebits_decompression_time;
    long long straights_decompression_time;
    long long path2bits_decompression_time;
};


decompInfo reconstructImage(CompressedImage compImg, bool showImg);
void reconstruct_edgeBits_iterative(int currentEdge, Direction currentDir, std::vector<bool>& reconstructedEdgeBits, int cols, int rows, std::vector<bool>& visited, std::queue<bool>& directionQueue);
std::vector<bool> reconstruct_edgeBits2bits(PathInfoVector paths, int edgeBitsSize, int cols, int rows);
std::vector<bool> reconstructStraights(Straights straights, int edgeBitsSize, int cols, int rows);
std::vector<bool> reconstruct_edgeBits_from_Horizontals(std::vector<bool>& horizontalBits, std::vector<bool>& reducedVerticals, int cols, int rows);
std::vector<std::vector<bool>> directionsVectorFromBitstring(std::string directionsBitstring, int directionBitsSize);
RLEVector parseRLEBitstring(std::vector<bool> rleBitString);
int boolVectorToIntSegment(const std::vector<bool>& bitstring, size_t startIndex, size_t length);
std::vector<bool> reconstruct_edgeBits2bits_from_bitstring(const std::vector<bool>& bitstring, int cols, int rows);
std::vector<bool> decodeRLEToNonRLE(const std::vector<bool>& rleBitString);
std::string boolVectorToString(const std::vector<bool>& bitVec, size_t startIndex, size_t length);
std::vector<RGB> decodeDifferences(const std::vector<uint8_t>& encodedDifferences);

#endif // DECOMPRESS_H
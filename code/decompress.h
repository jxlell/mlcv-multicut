#ifndef DECOMPRESS_H
#define DECOMPRESS_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include "DirectionPath.h"





void reconstructImage(int rows, int cols, RLEVector rle_paths, PathInfoVector paths, std::vector<bool> regionColorBitString, Straights straights, cv::Mat originalImg);
void reconstruct_edgeBits_iterative(int currentEdge, Direction currentDir, std::vector<bool>& directionVector, std::vector<bool>& reconstructedEdgeBits, int cols, int rows);
std::vector<bool> reconstruct_edgeBits2bits(PathInfoVector paths, int edgeBitsSize, int cols, int rows);
std::vector<bool> reconstructStraights(Straights straights, int edgeBitsSize, int cols, int rows);


#endif // DECOMPRESS_H
#ifndef DECOMPRESS_H
#define DECOMPRESS_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include "DirectionPath.h"
#include "compress.h"




void reconstructImage(CompressedImage compImg);
void reconstruct_edgeBits_iterative(int currentEdge, Direction currentDir, std::vector<bool>& directionVector, std::vector<bool>& reconstructedEdgeBits, int cols, int rows);
std::vector<bool> reconstruct_edgeBits2bits(PathInfoVector paths, int edgeBitsSize, int cols, int rows);
std::vector<bool> reconstructStraights(Straights straights, int edgeBitsSize, int cols, int rows);


#endif // DECOMPRESS_H
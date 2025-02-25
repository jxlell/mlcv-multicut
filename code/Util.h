#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include "partition.hxx"
#include "DirectionPath.h"
#include <filesystem>
#include <type_traits>
#include <fstream>
#include <iostream>
#include "Image.h"


andres::Partition<int> getRegions(std::vector<bool>& edgeBitsVector, int rows, int cols);
int getNeighbor(int currentEdge, Direction currentDirection, int neighborIndex, int cols, int rows);
Direction nextDirection(Direction dir);
Direction previousDirection(Direction dir);
int getEdgeBitFromList(int v, int w, std::vector<bool>& edgebitsvector, int rows, int cols);
void printProgressBar(int progress, int total);
Direction getDirectionFromIndex(int index, int row, int col);
std::string directionToString(Direction dir);
std::tuple<std::vector<bool>, std::vector<uint16_t>, bool> getRLE(std::vector<bool> bits);
std::vector<bool> reconstructRLE(std::vector<bool> zeros_rle, std::vector<uint16_t> ones_rle, bool start);
std::vector<bool> intToBool(int num);
int boolVectorToInt(const std::vector<bool>& binary);
std::vector<bool> boolVectorFromRGBVector(std::vector<RGB>& regionColors);
std::vector<RGB> colorBitStringToRGBVector(std::vector<bool>& bitString);
int calculateBoolVectorStorage(std::vector<bool>& boolVector);
#endif // UTIL_H
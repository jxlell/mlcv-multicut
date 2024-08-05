#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include "partition.hxx"
#include "DirectionPath.h"
#include <filesystem>
#include <type_traits>
#include <fstream>
#include <iostream>


andres::Partition<int> getRegions(std::vector<bool>& edgeBitsVector, int rows, int cols);
int getNeighbor(int currentEdge, Direction currentDirection, int neighborIndex, int cols, int rows);
Direction nextDirection(Direction dir);
Direction previousDirection(Direction dir);
int getEdgeBitFromList(int v, int w, std::vector<bool>& edgebitsvector, int rows, int cols);
void printProgressBar(int progress, int total);
#endif // UTIL_H
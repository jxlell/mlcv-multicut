#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include "partition.hxx"
#include "DirectionPath.h"


andres::Partition<int> getRegions(std::vector<bool>& edgeBitsVector, int rows, int cols);
int getNeighbor(int currentEdge, Direction currentDirection, int neighborIndex, int cols, int rows);
Direction nextDirection(Direction dir);
Direction previousDirection(Direction dir);
int getEdgeBitFromList(int v, int w, std::vector<bool>& edgebitsvector, int rows, int cols);
#endif // UTIL_H
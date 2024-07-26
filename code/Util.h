#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include "partition.hxx"
#include "DirectionPath.h"


andres::Partition<int> getRegions(std::vector<bool>& edgeBitsVector);
int getNeighbor(int currentEdge, Direction currentDirection, int neighborIndex, int cols, int rows);
Direction nextDirection(Direction dir);
Direction previousDirection(Direction dir);

#endif // UTIL_H
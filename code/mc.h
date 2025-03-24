#ifndef MC_H
#define MC_H

#include <vector>
#include <bitset>
#include "Image.h"
#include "DirectionPath.h"
#include "partition.hxx"

RGB getVertexColor(int v, int vertices, cv::Mat img, bool invert=false);
andres::Partition<int> getRegionsFromImage(cv::Mat img, std::vector<int> neighborsOffsets, int vertices);
std::vector<bool> dfs_paths_iterative(int currentEdge, Direction currentDir, std::vector<bool>& visited, cv::Mat img, std::vector<bool>& edgeBits01);


#endif // MC_H
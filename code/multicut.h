#ifndef MULTICUT_H
#define MULTICUT_H

#include <vector>
#include <bitset>
#include <opencv2/opencv.hpp> 
#include "Image.h"
#include "DirectionPath.h"
#include "partition.hxx"

class Multicut {
    public:
        Multicut(cv::Mat img);
        cv::Mat img;
        int getEdgeBitFromList(int v, int w, const std::vector<std::vector<bool>>& edgeBits);
        void findAllRegions();
        void assignRegions();
        void printRegionVector();
        int findVectorIndex(const std::vector<std::vector<int>>& regions, int targetNumber);
        void setVertexColor(int v, int red, int green, int blue);
        RGB getVertexColor(int v) const;
        int getVertices() const;
        std::vector<bool> getEdgeBits01() const;
        std::vector<bool> dfs_paths_iterative(int currentEdge, Direction currentDir, std::vector<bool>& visited);
        //Direction nextDirection(Direction dir);
        //Direction previousDirection(Direction dir);
        //int getNeighbor(int currentEdge, Direction currentDirection, int neighborIndex);
        andres::Partition<int> getRegionsFromImage();
        std::vector<bool> edgeBits01;
        std::vector<RGB> vertexColors;
        std::vector<int> vertexRegions;
        int vertices;
        std::vector<bool> visited; 
        PathInfoVector paths;
        int disconnectedComponents;
        int cols;
        int rows; 
        std::vector<RGB> regionColors;
        std::vector<int> neighborsOffsets;
};



#endif // MULTICUT_H

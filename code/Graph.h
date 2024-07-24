// Graph.h
#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <bitset>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "partition.hxx"


struct RGB {
    /*
    std::bitset<8> red;
    std::bitset<8> green;
    std::bitset<8> blue;
    */
   
    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;
    
};

enum class Direction { UP, RIGHT, DOWN, LEFT };

// Define the tuple type
// startEdge, startDirection, directionVector
using PathInfo = std::tuple<int, Direction, std::vector<bool>>;

// Define the vector of tuples
using PathInfoVector = std::vector<PathInfo>;


class Graph {
public:
    Graph(const std::string& imagePath);
    void addVertex();
    void printGraph();
    int getVertices() const;
    void setVertexColor(int v, int red, int green, int blue);
    RGB getVertexColor(int v) const;
    void setVertexValue(int v, int value);
    int getVertexValue(int v) const;
    bool compareRGB(const RGB& color1, const RGB& color2);
    int countEdges() const;
    void setEdgeBit(int v, int w, bool value);
    bool getEdgeBit(int v, int w) const;
    std::string getImagePath() const;
    void setMulticut();
    void printEdgeBits();
    void printColorRegions();
    std::vector<int> getConnectedPixels(int seed);
    void printConnectedPixels(int seed);
    void findAllRegions();
    void printSize();
    void reconstructImage(/*std::vector<std::vector<int>> regions, std::vector<RGB> regionColors*/);
    int findVectorIndex(const std::vector<std::vector<int>>& regions, int targetNumber);
    bool areImagesIdentical(const cv::Mat& image1, const cv::Mat& image2);
    cv::Mat generateRandomImage(int width, int height);
    void printProgressBar(int progress, int total);
    void printRegionVector();
    void assignRegions();
    std::vector<RGB> getRegionColors();
    std::vector<int> getVertexRegions();
    double reconstructMulticut();
    andres::Partition<int> getRegions(std::vector<bool>& edgeBitsVector);
    andres::Partition<int> getRegionsFromImage();
    int getEdgeBitFromList(int v, int w, std::vector<bool>& edgeBitsVector) const;
    void labelRegions();
    bool isValidNeighbor(int neighbor) const;
    std::vector<bool> dfs_paths_iterative(int currentEdge, Direction currentDir, std::vector<bool>& visited);
    std::string directionToString(Direction dir) const;
    void printPaths() const;
    Direction nextDirection(Direction dir);
    Direction previousDirection(Direction dir);
    void reconstruct_edgeBits_iterative(int startEdge, Direction currentDir, std::vector<bool>& directionVector, std::vector<bool>& reconstructedEdgeBits);

    int getNeighbor(int currentEdge, Direction currentDirection, int neighborIndex);
    
private:
    int vertices;
    int callstack;
    std::string imagePath;
    std::vector<std::vector<int>> adjList;
    std::vector<RGB> vertexColors;
    std::vector<int> vertexValues;
    std::vector<bool> edgeBits01;
    std::vector<int> regionRepresentatives;
    cv::Mat img;
    int cols;
    int rows;
    std::vector<int> neighborsOffsets;
    std::vector<RGB> regionColors;
    std::vector<std::vector<int>> regions;
    std::vector<int> vertexRegions;
    std::vector<bool> visited; 
    PathInfoVector paths;
    int directionIndex; 
    int disconnectedComponents;
};

#endif // GRAPH_H

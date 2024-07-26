#include "Decompressor.h"
#include <stack>
#include <queue>

Decompressor::Decompressor(std::vector<RGB> regionColors, PathInfoVector paths, int edgeBitsSize, int cols, int rows) {
    this->regionColors = regionColors;
    this->paths = paths; 
    this->edgeBitsSize = edgeBitsSize;
    this->reconstructed_edgeBits = std::vector<bool>(edgeBitsSize, false);
    this->cols = cols;
    this->rows = rows;
}

/*
void Decompressor::reconstruct_edgeBits_iterative(int currentEdge, Direction currentDir, std::vector<bool>& directionVector, std::vector<bool>& reconstructedEdgeBits){
    std::stack<std::pair<int, Direction>> pendingEdges;
    std::queue<bool> directionQueue;
    for (bool dir : directionVector){
        directionQueue.push(dir);
    }
    pendingEdges.push(std::make_pair(currentEdge, currentDir));
    
    while(!pendingEdges.empty()){
        std::tie(currentEdge, currentDir) = pendingEdges.top();
        pendingEdges.pop();
        reconstructedEdgeBits[currentEdge] = true;
        bool left = directionQueue.front();
        directionQueue.pop();
        bool forward = directionQueue.front();
        directionQueue.pop();
        bool right = directionQueue.front();
        directionQueue.pop();
        if(!left && !forward && !right){
            continue;
        }
        if(right){
            pendingEdges.push(std::make_pair(getNeighbor(currentEdge, currentDir, 2), nextDirection(currentDir)));
        }
        if(forward){
            pendingEdges.push(std::make_pair(getNeighbor(currentEdge, currentDir, 1), currentDir));
        }
        if(left){
            pendingEdges.push(std::make_pair(getNeighbor(currentEdge, currentDir, 0), previousDirection(currentDir)));
        }
    }
    return;
    //return reconstruction;
}
*/
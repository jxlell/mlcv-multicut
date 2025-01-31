#include "Util.h"
#include <iostream>
#include "DirectionPath.h"
#include <filesystem>
#include <fstream>
#include <numeric>
#include <type_traits>

int getNeighbor(int currentEdge, Direction currentDir, int neighborIndex, int cols, int rows){
    std::vector<int> edgeOffsets;

    int row = currentEdge/(2*cols-1) + 1; // 25 for 6409
    int column = (currentEdge % (2*cols-1)) / 2; // 0 for 6409
    //std::cout << "row: " << row << std::endl;
    //std::cout << "column: " << column << std::endl;

    switch (currentDir)
    {
    // offsets are in order of direction 3-bit-representation
    //TODO: row is never == 0?
    case Direction::UP:
        if(row == 0){
            return -1;
        }
        // last row case?
        else if(row==rows){
            // calculate column in last row differently
            column = currentEdge % (2*cols-1);
            edgeOffsets = {-(2*cols-1)+column, -(2*cols-1)+column+1, -(2*cols-1)+column+2};
        }else{
            edgeOffsets = {-2*(cols-1)-2, -2*(cols-1)-1, -2*(cols-1)};
        }
        break;
    case Direction::DOWN:
        if(row == rows){
            return -1;
        }
        // last row case?
        if(row==rows-1){
            //std::cout << "last row case for DOWN: " << currentEdge << std::endl;
            edgeOffsets = {1,2*cols-2-column, -1};
        }else{
            edgeOffsets = {1,2*cols-1, -1};
        }
        break;
    case Direction::LEFT:
        if(column == 0){
            return -1;
        }
        // last row case?
        if(row==rows-1){
            //std::cout << "last row case for LEFT: " << currentEdge << std::endl;
            edgeOffsets = {2*cols-2-column, -2, -1};
        }else{
            edgeOffsets = {2*(cols-1), -2,-1};
        }
        break;
    case Direction::RIGHT:
        if(column == cols-1){
            return -1;
        }
        // last row case?
        if(row==rows-1){
            //std::cout << "last row case for RIGHT: " << currentEdge << ", column " << column << std::endl;
            edgeOffsets = {1, 2, 2*cols-1-column};
        }
        else{
            edgeOffsets = {1, 2, 2*(cols-1)+2};
        }
        break;
    
    default:
        edgeOffsets = {1, 2, 2*(cols-1)+2};
        break;
    }
    
    //return edgeBits01[currentEdge + edgeOffsets[neighborIndex]];
    return currentEdge + edgeOffsets[neighborIndex];
}

Direction nextDirection(Direction dir) {
    return static_cast<Direction>((static_cast<int>(dir) + 1) % 4);
}

Direction previousDirection(Direction dir) {
    int newDir = (static_cast<int>(dir) - 1) % 4;
    if (newDir < 0) newDir += 4;  // Handle negative wrap-around
    return static_cast<Direction>(newDir);
}

andres::Partition<int> getRegions(std::vector<bool>& edgeBitsVector, int rows, int cols){
    andres::Partition<int> region(rows*cols);
    std::vector<int> neighborsOffsets = {cols, 1};
    for (int index = 0; index < rows * cols; ++index) {
        for (int offset : neighborsOffsets) {
            int neighbor = index + offset;
            if (neighbor >= 0 && neighbor < rows*cols && ((neighbor / cols == index / cols) || (abs(neighbor - index) > 1))) {
                // check if neighbours are separated by multicut 
                // if no: merge 

                if(getEdgeBitFromList(index, neighbor, edgeBitsVector, rows, cols) == 0){
                    region.merge(index, neighbor);
                    //std::cout << reconstruction.find(index) << ", ";
                    //std::cout << "merged " << index << " and " << neighbor << std::endl;
                }

                
            }
        }
    }
    return region;
}

int getEdgeBitFromList(int v, int w, std::vector<bool>& edgebitsvector, int rows, int cols) {
    int index = std::min(v,w);
    int row = index / cols + 1;
    int col = index % cols + 1;
    //std::cout << std::endl << "row: " << row << std::endl;
    //rechter nachbar?
    //TODO: berechnung nicht richtig für last row case!!
    if(abs(v-w) == 1 && row != rows){
        int rightBit = edgebitsvector[index * 2 - (row - 2)];
        //std::cout << "rightBitIndex: " << index * 2 - (row - 2) << std::endl;
        return rightBit;
    }
    if(abs(v-w) == 1 && row == rows){
        int rightBit = edgebitsvector[index * 2 - (row - 2) - col];
        //std::cout << "rightBitIndex: " << index * 2 - (row - 1) << std::endl;
        return rightBit;
    }
    //sonst unterer nachbar
    else{
        int edgeI = index * 2 - (row - 1);
        int downBit = edgebitsvector[edgeI];
        //std::cout << "downBitIndex: " << edgeI << std::endl;
        return downBit;
    }
    
}

void printProgressBar(int progress, int total) {
    float percentage = static_cast<float>(progress) / total;
    int width = 50;
    int barWidth = static_cast<int>(percentage * width);

    std::cout << "\r[";
    for (int i = 0; i < barWidth; ++i) {
        std::cout << "=";
    }
    for (int i = barWidth; i < width; ++i) {
        std::cout << " ";
    }
    std::cout << "] " << std::setprecision(3) << percentage * 100.0 << "%";
    std::cout.flush();
}

Direction getDirectionFromIndex(int index, int rows, int cols){
    Direction horizontalDir = Direction::DOWN;
    Direction verticalDir = Direction::RIGHT;

    int row = index / (2*cols-1);
    int col = index % (2*cols-1);

    if(index>2*cols*rows-cols-rows-cols){
        verticalDir = Direction::UP;
    }else{
        verticalDir = Direction::DOWN;
    }
    if((index+1) % (2*cols-1) == 0){
        horizontalDir = Direction::LEFT;
    }else{
        horizontalDir = Direction::RIGHT;
    }

    
    Direction currentDir = (row % 2 == 0) ? 
        ((index % 2 == 0) ? horizontalDir : verticalDir) : 
        ((index % 2 == 0) ? verticalDir : horizontalDir);

    return currentDir;
}

std::string directionToString(Direction dir){
    switch (dir)
    {
    case Direction::UP:
        return "UP";
        break;
    case Direction::DOWN:
        return "DOWN";
        break;
    case Direction::LEFT:
        return "LEFT";
        break;
    case Direction::RIGHT:
        return "RIGHT";
        break;
    default:
        return "UNKNOWN";
        break;
    }
}
#include "Util.h"
#include <iostream>

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

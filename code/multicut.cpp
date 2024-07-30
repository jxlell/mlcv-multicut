#include "Multicut.h"
#include <iostream>
#include <stack>
#include <unordered_set>
#include <cstdlib>
#include "DirectionPath.h"
#include "partition.hxx"
#include "Util.h"

Multicut::Multicut(cv::Mat img) {
    this->img = img;
    vertices = img.cols * img.rows;
    vertexColors.resize(vertices);    
    edgeBits01.resize((img.cols-1)*img.rows + img.cols*(img.rows-1));
    vertexRegions.resize(vertices, -1);
    visited.resize(edgeBits01.size(), false);
    cols = img.cols;
    rows = img.rows;
    neighborsOffsets = {cols, 1};
}

// Setter method to set the RGB value for a vertex
void Multicut::setVertexColor(int v, int red, int green, int blue) {
    if (v >= 0 && v < vertices) {
        vertexColors[v].red = static_cast<std::uint8_t>(red);
        vertexColors[v].green = static_cast<std::uint8_t>(green);
        vertexColors[v].blue = static_cast<std::uint8_t>(blue);
        //std::cout << static_cast<int>(vertexColors[v].red) << ", ";
    } else {
        std::cout << "Invalid vertex index." << std::endl;
    }
}

// Getter method to access the RGB value for a vertex
RGB Multicut::getVertexColor(int v) const {
    if (v >= 0 && v < vertices) {
        //return vertexColors[v];
        int x = v % cols;
        int y = v / cols;
        cv::Vec3b pixel = img.at<cv::Vec3b>(y, x);
        return {pixel[2], pixel[1], pixel[0]};
    } else {
        std::cout << "Invalid vertex index. Returning (0, 0, 0)." << std::endl;
        return {0, 0, 0};
    }
}

int Multicut::getVertices() const {
    return vertices;
}

std::vector<bool> Multicut::dfs_paths_iterative(int currentEdge, Direction currentDir, std::vector<bool>& visited){
    std::vector<bool> directionVector;
    //std::vector<bool> visited(edgeBits01.size(), false);
    std::stack<std::pair<int, Direction>> pendingEdges;
    int vecIndex = 0;
    pendingEdges.push(std::make_pair(currentEdge, currentDir));
    visited[currentEdge] = true;
    bool left, front, right;
    while(!pendingEdges.empty()){
        left = false;
        front = false;
        right = false;
        std::tie(currentEdge, currentDir) = pendingEdges.top();
        //visited[currentEdge] = true;
        pendingEdges.pop();
        //std::cout << "current edge: " << currentEdge << std::endl;

        int neighborLeft = getNeighbor(currentEdge, currentDir, 0, cols, rows);
        int neighborFront = getNeighbor(currentEdge, currentDir, 1, cols, rows);
        int neighborRight = getNeighbor(currentEdge, currentDir, 2, cols, rows);


        //TODO: push 000 und continue falls nächste edges nicht multicut oder schon visited 
        if(neighborLeft == -1 || neighborFront == -1 || neighborRight == -1 || neighborLeft >= edgeBits01.size() || neighborFront >= edgeBits01.size() || neighborRight >= edgeBits01.size()){
            directionVector.push_back(0);
            directionVector.push_back(0);
            directionVector.push_back(0);
            //directionVector[vecIndex++] = 0;
            //directionVector[vecIndex++] = 0;
            //directionVector[vecIndex++] = 0;
            //std::cout << "000" << std::endl;
            continue;
        }




        if(!visited[neighborLeft]){
            left = edgeBits01[neighborLeft];
            directionVector.push_back(left);
            //directionVector[vecIndex++] = left;
        }else{
            directionVector.push_back(false);
            //directionVector[vecIndex++] = false;
        }
        if(!visited[neighborFront]){
            front = edgeBits01[neighborFront];
            directionVector.push_back(front);
            //directionVector[vecIndex++] = front;
        }
        else{
            directionVector.push_back(false);
            //directionVector[vecIndex++] = false;

        }
        if(!visited[neighborRight]){
            right = edgeBits01[neighborRight];
            directionVector.push_back(right);
            //directionVector[vecIndex++] = right;

        }
        else{
            directionVector.push_back(false);
            //directionVector[vecIndex++] = false;
        }

        //std::cout << left << front << right << std::endl;

        
        bool lastThreeAllFalse = false;
        if (directionVector.size() >= 3) {
            lastThreeAllFalse = !directionVector[directionVector.size() - 1] &&
                                !directionVector[directionVector.size() - 2] &&
                                !directionVector[directionVector.size() - 3];
        }

        if(!left && !front && !right && !lastThreeAllFalse){
            directionVector.push_back(0);
            directionVector.push_back(0);
            directionVector.push_back(0);
            //directionVector[vecIndex++] = 0;
            //directionVector[vecIndex++] = 0;
            //directionVector[vecIndex++] = 0;
            continue;
        }
        
        

        if(right){
            if(!visited[neighborRight]){
                pendingEdges.push(std::make_pair(neighborRight, nextDirection(currentDir)));
                visited[neighborRight] = true;
            }
        }
        if(front){
            if(!visited[neighborFront]){
                pendingEdges.push(std::make_pair(neighborFront, currentDir));
                visited[neighborFront] = true;
            }
        }
        if(left){
            if(!visited[neighborLeft]){
                pendingEdges.push(std::make_pair(neighborLeft, previousDirection(currentDir)));
                visited[neighborLeft] = true;
            }
        }

    }
    //directionVector.resize(vecIndex);
    return directionVector;
}

andres::Partition<int> Multicut::getRegionsFromImage(){
    andres::Partition<int> region(rows*cols);
    for (int index = 0; index < rows * cols; ++index) {
        for (int offset : neighborsOffsets) {
            int neighbor = index + offset;
            if (neighbor >= 0 && neighbor < getVertices() && ((neighbor / cols == index / cols) || (abs(neighbor - index) > 1))) {
                // check if neighbours are separated by multicut 
                // if no: merge 
                if(compareRGB(getVertexColor(index), getVertexColor(neighbor))){
                    region.merge(index, neighbor);
                    //std::cout << reconstruction.find(index) << ", ";
                }

                
            }
        }
    }
    return region;
}
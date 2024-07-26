#include "Compressor.h"
#include "Multicut.h"
#include "Image.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
#include "partition.hxx"
#include "DirectionPath.h"

Compressor::Compressor(const std::string& imagePath) 
    : imagePath(imagePath), 
      img(cv::imread(imagePath, cv::IMREAD_COLOR)), 
      multicut(img),  // Initialize multicut with img
      vertices(img.rows * img.cols) 
{
    if (img.empty()) {
        std::cerr << "Error: Could not read the image: " << imagePath << std::endl;
    }
    neighborsOffsets = {img.cols, 1};
}

std::pair<std::vector<RGB>, PathInfoVector> Compressor::compressImage(){
    auto start = std::chrono::high_resolution_clock::now();
    setVertexColors();
    auto end = std::chrono::high_resolution_clock::now();
    auto start_to_end = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "time to set vertex colors in ms: " << start_to_end << std::endl;
 
    start = std::chrono::high_resolution_clock::now();
    setEdgeBits();
    end = std::chrono::high_resolution_clock::now();
    start_to_end = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "time to set edge bits in ms: " << start_to_end << std::endl;

    start = std::chrono::high_resolution_clock::now();
    setRegions();
    end = std::chrono::high_resolution_clock::now();
    start_to_end = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "time to set regions in ms: " << start_to_end << std::endl;


    start = std::chrono::high_resolution_clock::now();
    setPaths();
    end = std::chrono::high_resolution_clock::now();
    start_to_end = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "time to set paths in ms: " << start_to_end << std::endl;

    return std::make_pair(multicut.regionColors, multicut.paths);
}


void Compressor::setVertexColors(){
    // Iterate through every pixel from top-left to bottom-right
    int pixel_index = 0;
    for (int y = 0; y < img.rows; ++y) {
        for (int x = 0; x < img.cols; ++x) {
            // Access pixel value at position (x, y)
            cv::Vec3b pixel = img.at<cv::Vec3b>(y, x);

            // Access individual color channels
            uchar blue = pixel[0];
            uchar green = pixel[1];
            uchar red = pixel[2];


            //set rgb values for vertex
            //img_graph.setVertexColor(pixel_index, static_cast<int>(red), static_cast<int>(blue), static_cast<int>(green));
            //std::cout << y*img.cols+x << ", ";
            multicut.setVertexColor(y*img.cols+x, static_cast<int>(red), static_cast<int>(green), static_cast<int>(blue));
            //std::cout << getVertexColor(y*cols+x).red << ", ";
            pixel_index++;
        }
        //printProgressBar(y, rows);

    }
}

void Compressor::setEdgeBits(){
    //std::cout << "\nsetting multicut bits\n";
    int regionIndex = 0;
    int edgeIndex = -1;
    // Check neighbors for every vertex and set multicut bits
    for (int v = 0; v < vertices; ++v) {
        RGB currentColor = multicut.getVertexColor(v);
        // get color of previous pixel (current pixel if there is no previous)
        RGB previousColor = multicut.getVertexColor(std::max(0,v-1));
        if (v == 0 || !compareRGB(currentColor, previousColor)) {
            //regionColors.push_back(currentColor);
            regionIndex++;
        }

        for (int offset : neighborsOffsets) {
            int neighbor = v + offset;
            if (neighbor >= 0 && neighbor < vertices && ((neighbor / img.cols == v / img.cols) || (abs(neighbor - v) > 1))) {
                //addEdge(neighbor, v);
                if(offset == 1 || offset == img.cols){
                    //std::cout << edgeIndex << ", ";
                    edgeIndex++;
                    if(!compareRGB(currentColor, multicut.getVertexColor(neighbor))){
                        //set edgeBit01
                        multicut.edgeBits01[edgeIndex] = true;
                        //std::cout << "offset: " << offset << ", ";
                    }
                }
            }
        }
        //printProgressBar(v, getVertices());
    }
}

void Compressor::setRegions(){
    andres::Partition<int> multicutregion = multicut.getRegionsFromImage();
    //for(int i = 0; i<25; i++){
    //    std::cout << "region of index " << i << ": " << multicutregion.find(i) << ", ";
    //}

    std::vector<int> reps;
    multicutregion.representatives(std::back_inserter(reps));
    //std::cout << std::endl;
    for (int rep : reps) {
        //std::cout << rep << ", ";
        multicut.regionColors.push_back(multicut.getVertexColor(rep));
        //std::cout << static_cast<int>(repRGB.red) << ", ";
    }
    multicut.regionColors.resize(reps.size());
}

void Compressor::setPaths(){
    int edgeI = 0;
    int dfsI = 0;

    
    
    for (bool edge : multicut.edgeBits01){
        // skip non-multicut edges or previously visited edges ||
        if(!edge){
            edgeI++;
            continue;
            }
        if(multicut.visited[edgeI]){
            edgeI++;
            continue;
        }
        //std::cout << std::endl << edgeI;

        // dir rausfinden
        Direction horizontalDir = Direction::DOWN;
        Direction verticalDir = Direction::RIGHT;

        int row = edgeI / (2*img.cols-1);
        int col = edgeI % (2*img.cols-1);

        if(edgeI>2*img.cols*img.rows-img.cols-img.rows-img.cols){
            verticalDir = Direction::UP;
        }else{
            verticalDir = Direction::DOWN;
        }
        if((edgeI+1) % (2*img.cols-1) == 0){
            horizontalDir = Direction::LEFT;
        }else{
            horizontalDir = Direction::RIGHT;
        }

        Direction currentDir = (row % 2 == 0) ? 
            ((edgeI % 2 == 0) ? horizontalDir : verticalDir) : 
            ((edgeI % 2 == 0) ? verticalDir : horizontalDir);

        // dfs starten
        /*
        if(dfsI<5){
            std::cout << "start dfs with " << directionToString(currentDir) << " on " << edgeI << ", visited: " << visited[edgeI] << std::endl;
            std::cout << "visited count: " << std::count(visited.begin(), visited.end(), true) << std::endl;
            }
        */
        
        dfsI++;

        std::vector<bool> directionVector;
        //directionVector = dfs_paths_recursive(edgeI, visited, currentDir, directionVector);
        directionVector = multicut.dfs_paths_iterative(edgeI, currentDir, multicut.visited);
        multicut.paths.emplace_back(edgeI, currentDir, directionVector);
        //std::cout << "path size: " << directionVector.size() << std::endl;
        
        //std::cout << std::endl;
        //for(bool v : visited){
        //    std::cout << v;
        //}
        //std::cout << std::endl;
        edgeI++;
    }
    //std::cout << "edgeI: " << edgeI << std::endl;
    //std::cout << "number of edges: " << 2*rows*cols - cols - rows << std::endl;

    //printPaths();
    
    
    multicut.disconnectedComponents = dfsI;
    //std::cout << "number of disconnected components: " << dfsI << std::endl;

}

Multicut Compressor::getMulticut(){
    return multicut;
}
#include "Decompressor.h"
#include <stack>
#include <queue>
#include "Util.h"
#include <opencv2/opencv.hpp>
#include "Image.h"

Decompressor::Decompressor(std::vector<RGB> regionColors, PathInfoVector paths, int edgeBitsSize, int cols, int rows, cv::Mat img) {
    this->regionColors = regionColors;
    this->paths = paths; 
    this->edgeBitsSize = edgeBitsSize;
    this->reconstructed_edgeBits = std::vector<bool>(edgeBitsSize, false);
    this->cols = cols;
    this->rows = rows;
    this->img = img;
}


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
            pendingEdges.push(std::make_pair(getNeighbor(currentEdge, currentDir, 2, cols, rows), nextDirection(currentDir)));
        }
        if(forward){
            pendingEdges.push(std::make_pair(getNeighbor(currentEdge, currentDir, 1, cols, rows), currentDir));
        }
        if(left){
            pendingEdges.push(std::make_pair(getNeighbor(currentEdge, currentDir, 0, cols, rows), previousDirection(currentDir)));
        }
    }
    return;
}


void Decompressor::reconstructMulticut(){
    cv::Mat image(rows, cols, CV_8UC3, cv::Scalar(0, 0, 0)); 
    int directionBitsSize;
    int numberOfPaths = paths.size();

    std::vector<bool> reconstructed_edgeBits(edgeBitsSize, false);
    int i;
    for(PathInfo pathinfo : paths){
        //std::cout << directionToString(std::get<1>(pathinfo)) << std::endl;
        //printProgressBar(i , paths.size());
        //reconstructed_edgeBits = reconstruct_edgeBits_iterative(std::get<0>(pathinfo), std::get<1>(pathinfo), std::get<2>(pathinfo));
        reconstruct_edgeBits_iterative(std::get<0>(pathinfo), std::get<1>(pathinfo), std::get<2>(pathinfo), reconstructed_edgeBits);
        directionBitsSize += std::get<2>(pathinfo).size();
        //break;
        i++;
    }

    //std::cout << "size of directionbits: " << directionBitsSize << std::endl;


    //std::cout << "\nreconstruction for edgebits01 finished" << std::endl;

    //reconstructed_edgeBits.assign(reconstructed_edgeBits.size(), false);

    andres::Partition<int> reconstruction = getRegions(reconstructed_edgeBits, rows, cols);
    //printColorRegions();
    std::map<int, int> representativeLabels;
    reconstruction.representativeLabeling(representativeLabels);
    //std::vector<int> reps;
    //reconstruction.representatives(std::back_inserter(reps));
    for (int index = 0; index < rows * cols; ++index) {

        //std::cout << reconstruction.find(index) << ", ";

        // Calculate row and column indices from the linear index
        int y = index / cols;
        int x = index % cols;
        if(index%100 == 0){
            //printProgressBar(index, rows*cols);
        }
        // get color
        int region = reconstruction.find(index);
        int continuousLabel = representativeLabels[region];

        // an welchem index steht nummer "region" im vector der representatives
        
        /*
        //quadratic runtime?
        auto it = std::find(reps.begin(), reps.end(), region);
        std::size_t indexInReps;

        if (it != reps.end()) {
            indexInReps = std::distance(reps.begin(), it);
            //std::cout << "Index of region " << region << " in reps: " << indexInReps << std::endl;
        }
        */
        

        //RGB col = regionColors[indexInReps];
        RGB col = regionColors[continuousLabel];


        //RGB col = getVertexColor(0);
        //std::cout << col.green.to_ulong() << std::endl;
        // Set the color (BGR format)
        image.at<cv::Vec3b>(y, x) = cv::Vec3b(col.blue, col.green, col.red);  
    }
    
    //printSize();
    std::cout << "reconstruction and original identical: " << ((areImagesIdentical(img, image)) ? "YES" : "NO") << std::endl;
    
    /*
    cv::destroyAllWindows();
    cv::imshow("Original", img);
    cv::imshow("Reconstruction", image);
    cv::waitKey(0);
    */
}
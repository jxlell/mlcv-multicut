#include "Decompressor.h"
#include <stack>
#include <queue>
#include "Util.h"
#include <opencv2/opencv.hpp>
#include "Image.h"

/**
 * @class Decompression module 
 * TODO: klasse notwendig? 
 */
Decompressor::Decompressor(std::vector<RGB> regionColors, PathInfoVector paths, int edgeBitsSize, int cols, int rows, cv::Mat img, PathInfoVector paths_2bit, RLEVector rle_paths, Straights straights) {
    this->regionColors = regionColors;
    this->paths = paths; 
    this->paths_2bit = paths_2bit;
    this->edgeBitsSize = edgeBitsSize;
    this->reconstructed_edgeBits = std::vector<bool>(edgeBitsSize, false);
    this->cols = cols;
    this->rows = rows;
    this->img = img;
    this->rle_paths = rle_paths;
    this->straights = straights;
}

/**
 * @brief sets edge bits in vector based on the direction vector of one single path
 * @param currentEdge starting edge of the current path 
 * @param currentDir starting direction of the current path
 * @param directionVector boolen direction sequence of the current path 
 * @param reconstructedEdgeBits output edgebits vector which is to be filled with the reconstructed bits 
 */
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

std::vector<bool> Decompressor::reconstruct_edgeBits2bits(PathInfoVector paths){
    std::vector<bool> reconstructed_edgeBits_2bits(edgeBitsSize, false);
    for(PathInfo pathinfo : paths){
        int startEdge = boolVectorToInt(std::get<0>(pathinfo));
        // Direction startDirection = std::get<1>(pathinfo);
        Direction startDirection = getDirectionFromIndex(startEdge, rows, cols);
        std::vector<bool> directionVector = std::get<2>(pathinfo);
        reconstructed_edgeBits_2bits[startEdge] = true;
        int currentEdge = startEdge;
        Direction currentDir = startDirection;
        for (size_t i = 0; i<directionVector.size(); i+=2){
            if(directionVector[i] == 1 && directionVector[i+1] == 1){
                currentEdge = getNeighbor(currentEdge, currentDir, 1, cols, rows);
                reconstructed_edgeBits_2bits[currentEdge] = true;
                continue;
            }
            if(directionVector[i] == 1 && directionVector[i+1] == 0){
                currentEdge = getNeighbor(currentEdge, currentDir, 0, cols, rows);
                currentDir = previousDirection(currentDir);
                reconstructed_edgeBits_2bits[currentEdge] = true;
                continue;
            }
            if(directionVector[i] == 0 && directionVector[i+1] == 1){
                currentEdge = getNeighbor(currentEdge, currentDir, 2, cols, rows);
                currentDir = nextDirection(currentDir);
                reconstructed_edgeBits_2bits[currentEdge] = true;
                continue;
            }
        }
    }
    return reconstructed_edgeBits_2bits;
}

std::vector<bool> Decompressor::reconstructStraights(Straights straights){
    std::vector<bool> reconstructed_edgeBits_straights(edgeBitsSize, false);
    //TODO: prüfen (copilot)
    for(auto straight : straights){
        int startEdge = boolVectorToInt(std::get<0>(straight));
        int count = boolVectorToInt(std::get<1>(straight));
        Direction startDirection = getDirectionFromIndex(startEdge, rows, cols);
        reconstructed_edgeBits_straights[startEdge] = true;
        int currentEdge = startEdge;
        Direction currentDir = startDirection;
        for (int i = 0; i<count; i++){
            currentEdge = getNeighbor(currentEdge, currentDir, 1, cols, rows);
            reconstructed_edgeBits_straights[currentEdge] = true;
        }
    }
    return reconstructed_edgeBits_straights;
}

/**
 * @brief reconstructs edgebits vector based on the path information and in addition uses the color vector to reconstruct the whole image 
 * @see Decompressor::reconstruct_edgeBits_iterative()
 */
void Decompressor::reconstructImage(){
    cv::Mat image(rows, cols, CV_8UC3, cv::Scalar(0, 0, 0)); 
    int directionBitsSize;

    // convert from rle_paths to paths_2bit
    std::vector<PathInfo> paths_2bit;
    for (auto rle : rle_paths) {
        std::vector<bool> edgeI;
        std::vector<bool> zeros_rle;
        std::vector<std::vector<bool>> ones_rle;
        std::vector<uint16_t> ones_rle_16;
        bool start;
        std::tie(edgeI, zeros_rle, ones_rle, start) = rle;
        for (std::vector<bool> vec: ones_rle) {
            ones_rle_16.push_back(boolVectorToInt(vec));
        }
        std::vector<bool> directions2bits = reconstructRLE(zeros_rle, ones_rle_16, start);
        paths_2bit.emplace_back(edgeI, getDirectionFromIndex(boolVectorToInt(edgeI), rows, cols), directions2bits);
    }

    int numberOfPaths = paths.size();

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<bool> reconstructed_edgeBits(edgeBitsSize, false);
    int i;
    for(PathInfo pathinfo : paths){
        //std::cout << directionToString(std::get<1>(pathinfo)) << std::endl;
        //printProgressBar(i , paths.size());
        //reconstructed_edgeBits = reconstruct_edgeBits_iterative(std::get<0>(pathinfo), std::get<1>(pathinfo), std::get<2>(pathinfo));
        int startEdge = boolVectorToInt(std::get<0>(pathinfo));
        //Direction calculated just based on the index, no need to pass it as an argument
        Direction currentDir = getDirectionFromIndex(startEdge, rows, cols);
        reconstruct_edgeBits_iterative(startEdge, currentDir, std::get<2>(pathinfo), reconstructed_edgeBits);
        directionBitsSize += std::get<2>(pathinfo).size();
        //break;
        i++;
    }

    // reconstruct from 2-bit paths
    std::vector<bool> reconstructed_edgeBits_2bits = reconstruct_edgeBits2bits(paths_2bit);

    // reconstruct from straights
    std::vector<bool> reconstructed_edgeBits_straights = reconstructStraights(straights);

    // empty reconstruction
    std::vector<bool> empty_reconstruction = std::vector<bool>(edgeBitsSize, true);


    //std::cout << "size of directionbits: " << directionBitsSize << std::endl;


    //std::cout << "\nreconstruction for edgebits01 finished" << std::endl;

    //reconstructed_edgeBits.assign(reconstructed_edgeBits.size(), false);

    andres::Partition<int> reconstruction = getRegions(reconstructed_edgeBits_straights, rows, cols);
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
    auto end = std::chrono::high_resolution_clock::now();
    auto start_to_end = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    decompressionTime = start_to_end;
    
    cv::destroyAllWindows();
    cv::imshow("Original", img);
    cv::imshow("Reconstruction", image);
    cv::waitKey(0);
    
}

/**
 * @return decompression time in ms 
 */
long long Decompressor::getDecompressionTime() const {
    return decompressionTime;
}
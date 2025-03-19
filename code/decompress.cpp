#include "decompress.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include "DirectionPath.h"
#include "Util.h"
#include "partition.hxx"
#include "Image.h"
#include <tuple>
#include <vector>
#include <chrono>
#include <map>
#include <string>
#include "compress.h"
#include "huffman.h"

bool reconstructImage(CompressedImage compImg, bool showImg){
    cv::Mat originalImg = compImg.originalImage;
    std::vector<RGB> regionColors = compImg.colorVector;
    PathInfoVector paths = compImg.paths;
    std::vector<bool> pathsBitString = compImg.pathsBitString;
    PathInfoVector paths_2bit_nonRLE = compImg.pathInfoVector2bit; // unused
    RLEVector rle_paths = compImg.rleVector;
    Straights straights = compImg.straights;
    std::vector<bool> regionColorBitString = compImg.regionColorBitString;
    int rows = originalImg.rows;
    int cols = originalImg.cols;
    std::vector<bool> straightsHuffmanCodesBitString = compImg.straightsHuffmanCodesBitString;
    std::vector<uint32_t> straightsHuffmanCodesStartPoints = compImg.straightsHuffmanCodesStartPoints;
    HuffmanNode* root = compImg.root;
    std::vector<uint16_t> straightLengthsList = compImg.straightLengthsList;
    std::vector<uint32_t> straightLengthFrequencies = compImg.straightLengthFrequencies;

    
    cv::Mat image(rows, cols, CV_8UC3, cv::Scalar(0, 0, 0)); 
    int directionBitsSize;
    int edgeBitsSize = (cols-1)*rows + cols*(rows-1);


    // parse tree path bitstring
    std::string pathsBitStringStr;
    for (bool bit : pathsBitString) {
        pathsBitStringStr += bit ? "1" : "0";
    }

    std::cout << "length of pathsBitStringStr: " << pathsBitStringStr.size() << std::endl;
    std::cout << "bitstring compression rate: " << (rows * cols * 24) / (double)pathsBitStringStr.size() << std::endl;

    std::string cols_str = pathsBitStringStr.substr(0, 16);
    int cols_int = std::stoi(cols_str, nullptr, 2);
    pathsBitStringStr = pathsBitStringStr.substr(16);
    std::string rows_str = pathsBitStringStr.substr(0, 16);
    int rows_int = std::stoi(rows_str, nullptr, 2);
    pathsBitStringStr = pathsBitStringStr.substr(16);

    int regionColorBitsSize = std::ceil(std::log2(cols_int * rows_int));
    std::string regionColorBitsSizeStr = pathsBitStringStr.substr(0, regionColorBitsSize);
    int regionColorBitsSizeInt = std::stoi(regionColorBitsSizeStr, nullptr, 2);
    pathsBitStringStr = pathsBitStringStr.substr(regionColorBitsSize);
    std::string regionColorBitStringStr = pathsBitStringStr.substr(0, regionColorBitsSizeInt*24);
    std::vector<bool> regionColorBitStringParsed;
    for(char c : regionColorBitStringStr){
        regionColorBitStringParsed.push_back(c == '1');
    }
    pathsBitStringStr = pathsBitStringStr.substr(regionColorBitStringStr.size());

    // Calculate compression rates
    int originalSize = rows * cols * 24; // Assuming 24 bits per pixel for the original image
    int compressedSize = pathsBitString.size() + regionColorBitString.size();

    double compressionRate = (double)compressedSize / originalSize;
    std::cout << "Compression Rate: " << compressionRate << std::endl;

    //int disconnectedComponentsBits = std::ceil(std::log2(cols * rows / 2));
    int disconnectedComponentsBits = std::ceil(cols_int/2) * std::ceil(rows_int/2);

    // std::cout << "Paths bitstring: " << pathsBitStringStr << std::endl;
    std::string numberOfComponentsStr = pathsBitStringStr.substr(0,disconnectedComponentsBits);
    // std::cout << "Number of components: " << numberOfComponentsStr << std::endl;
    int numberOfComponets = std::stoi(numberOfComponentsStr, nullptr, 2);
    std::string startPointBitsStr = pathsBitStringStr.substr(disconnectedComponentsBits, 5);
    int startPointBits = std::stoi(startPointBitsStr, nullptr, 2);
    // std::cout << "Start point bits: " << startPointBits << std::endl;
    pathsBitStringStr = pathsBitStringStr.substr(disconnectedComponentsBits + 5);

    std::string startPointBitstring = pathsBitStringStr.substr(0, startPointBits * numberOfComponets);
    std::string directionsBitstring = pathsBitStringStr.substr(startPointBits * numberOfComponets);

    std::vector<uint32_t> startPoints; 
    for(size_t i = 0; i<startPointBitstring.size(); i+=startPointBits){
        std::string currentStr = startPointBitstring.substr(i, startPointBits);
        uint32_t startPoint = std::stoi(currentStr, nullptr, 2);
        startPoints.push_back(startPoint);
    }

    std::vector<std::vector<bool>> directions;
    std::string currentDirection;
    std::string currentDirectionBitstring;
    for(size_t i = 0; i<directionsBitstring.size(); i+=3){
        currentDirection = directionsBitstring.substr(i, 3);
        currentDirectionBitstring += currentDirection;
        if(currentDirection == "000"){
            std::vector<bool> currentDirectionVector;
            for(size_t j = 0; j<currentDirectionBitstring.size(); j++){
                currentDirectionVector.push_back(currentDirectionBitstring[j] == '1');
            }
            directions.push_back(currentDirectionVector);
            currentDirectionBitstring.clear();
        }
    }

    // Compile paths vector from startPoints and directions
    PathInfoVector paths_from_bitstring;
    for (size_t i = 0; i < startPoints.size(); ++i) {
        paths_from_bitstring.emplace_back(startPoints[i], getDirectionFromIndex(startPoints[i], rows, cols), directions[i]);
    }

    // Print paths from bitstring
    // std::cout << "Paths from bitstring:" << std::endl;
    // for (const auto& path : paths_from_bitstring) {
    //     std::cout << "Start Edge: " << std::get<0>(path) << ", Directions: ";
    //     for (bool dir : std::get<2>(path)) {
    //         std::cout << dir;
    //     }
    //     std::cout << std::endl;
    // }

    // convert from rle_paths to paths_2bit
    PathInfoVector paths_2bit;
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
        paths_2bit.emplace_back(boolVectorToInt(edgeI), getDirectionFromIndex(boolVectorToInt(edgeI), rows, cols), directions2bits);
    }

    // create huffman tree from lengths and frequencies 
    map<int,int> straightLengths;
    for (size_t i = 0; i < straightLengthsList.size(); ++i) {
        straightLengths[straightLengthsList[i]] = straightLengthFrequencies[i];
    }
    std::map<int, string> straightsHuffmanCodes;
    HuffmanNode* reconstructedRoot; 
    std::tie(straightsHuffmanCodes, reconstructedRoot) = buildCodes(straightLengths);

    // std::cout << "huffman trees equal: " << (areHuffmanTreesEqual(root, reconstructedRoot) ? "YES" : "NO") << std::endl;

    //reconstruct straights from straightsHuffmanCodesBitString with huffman codes and straitsHuffmanCodesStartPoints
    std::vector<int> straightsLengthsDecoded;
    string decodedWord = "";
    string straightsString;
    //convert bitstring to actual string
    for (bool bit : straightsHuffmanCodesBitString){
        straightsString += bit ? "1" : "0";
    }
    // std::cout << "First 20 entries of straights string: ";
    // for (size_t i = 0; i < 20 && i < straightsString.size(); ++i) {
    //     std::cout << straightsString[i];
    // }
    // std::cout << std::endl;
    std::tie(decodedWord, straightsLengthsDecoded) = decodeHuffman(reconstructedRoot, straightsString);
    Straights straightsDecoded;
    // std::cout << "First 5 entries of decoded straights:" << std::endl;
    // for (size_t i = 0; i < 5 && i < straightsLengthsDecoded.size(); ++i) {
    //     std::cout << "Start Edge: " << straightsHuffmanCodesStartPoints[i] << ", Count: " << straightsLengthsDecoded[i] << std::endl;
    // }
    // tie together start points vector and legnths vector to get the straights
    // std::cout << "straights lengths decoded size: " << straightsLengthsDecoded.size() << std::endl;
    for (int i = 0; i < straightsLengthsDecoded.size(); i++){
        std::vector<bool> startEdge = intToBool(straightsHuffmanCodesStartPoints[i]);
        std::vector<bool> count = intToBool(straightsLengthsDecoded[i]);
        straightsDecoded.push_back(std::make_tuple(startEdge, count));
    }

    // for(auto& straight : straights){
    //     std::cout << "Start Edge: " << boolVectorToInt(std::get<0>(straight)) << ", Count: " << boolVectorToInt(std::get<1>(straight)) << std::endl;
    //     break;
    // }
    // std::cout << "huffmancodesbitstring size: " << straightsHuffmanCodesBitString.size() << std::endl;
    // std::cout << "straights string size: " << straightsString.size() << std::endl;
    // std::cout << "straights decoded size: " << straightsDecoded.size() << std::endl;
    // std::cout << "straights size: " << straights.size() << std::endl;

    // // Identify indices in which original and decoded straights differ and print differences
    // std::cout << "Differences between original and decoded straights:" << std::endl;
    // for (size_t i = 0; i < straights.size(); ++i) {
    //     if (i >= straightsDecoded.size()) {
    //         std::cout << "Decoded straights is shorter than original straights." << std::endl;
    //         break;
    //     }
    //     if (std::get<0>(straights[i]) != std::get<0>(straightsDecoded[i]) || std::get<1>(straights[i]) != std::get<1>(straightsDecoded[i])) {
    //         std::cout << "Index " << i << " differs." << std::endl;
    //         std::cout << "Original: Start Edge: " << boolVectorToInt(std::get<0>(straights[i])) << ", Count: " << boolVectorToInt(std::get<1>(straights[i])) << std::endl;
    //         std::cout << "Decoded: Start Edge: " << boolVectorToInt(std::get<0>(straightsDecoded[i])) << ", Count: " << boolVectorToInt(std::get<1>(straightsDecoded[i])) << std::endl;
    //     }
    // }
    // if (straightsDecoded.size() > straights.size()) {
    //     std::cout << "Decoded straights is longer than original straights." << std::endl;
    // }

    // Print the first 15 entries of the Huffman bitstring
    // std::cout << "First 15 entries of Huffman bitstring (in decode): ";
    // for (size_t i = 0; i < 15 && i < straightsHuffmanCodesBitString.size(); ++i) {
    //     std::cout << straightsHuffmanCodesBitString[i];
    // }
    // std::cout << std::endl;

    // Compare straights and straightsDecoded
    bool areStraightsIdentical = (straights == straightsDecoded);
    // std::cout << "Are original and decoded straights identical? " << (areStraightsIdentical ? "YES" : "NO") << std::endl;
    // Print indices which are different (only first 5)
    // std::cout << "Indices where original and decoded straights differ (first 5):" << std::endl;
    // int diffCount = 0;
    // for (size_t i = 0; i < straights.size() && diffCount < 5; ++i) {
    //     if (i >= straightsDecoded.size()) {
    //         std::cout << "Decoded straights is shorter than original straights." << std::endl;
    //         break;
    //     }
    //     if (std::get<0>(straights[i]) != std::get<0>(straightsDecoded[i]) || std::get<1>(straights[i]) != std::get<1>(straightsDecoded[i])) {
    //         std::cout << "Index " << i << " differs." << std::endl;
    //         std::cout << "Original: Start Edge: " << boolVectorToInt(std::get<0>(straights[i])) << ", Count: " << boolVectorToInt(std::get<1>(straights[i])) << std::endl;
    //         std::cout << "Decoded: Start Edge: " << boolVectorToInt(std::get<0>(straightsDecoded[i])) << ", Count: " << boolVectorToInt(std::get<1>(straightsDecoded[i])) << std::endl;
    //         diffCount++;
    //     }
    // }

    int numberOfPaths = paths.size();

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<bool> reconstructed_edgeBits(edgeBitsSize, false);
    int i = 0;

    std::vector<bool> visited(edgeBitsSize, false);
    for(PathInfo pathinfo : paths_from_bitstring){
        //std::cout << directionToString(std::get<1>(pathinfo)) << std::endl;
        //printProgressBar(i , paths.size());
        //reconstructed_edgeBits = reconstruct_edgeBits_iterative(std::get<0>(pathinfo), std::get<1>(pathinfo), std::get<2>(pathinfo));
        int startEdge = std::get<0>(pathinfo);
        //Direction calculated just based on the index, no need to pass it as an argument
        Direction currentDir = getDirectionFromIndex(startEdge, rows, cols);
        reconstruct_edgeBits_iterative(startEdge, currentDir, std::get<2>(pathinfo), reconstructed_edgeBits, cols, rows, visited);
        directionBitsSize += std::get<2>(pathinfo).size();
        //break;
        i++;
    }

    // reconstruct from 2-bit paths
    std::vector<bool> reconstructed_edgeBits_2bits = reconstruct_edgeBits2bits(paths_2bit, edgeBitsSize, cols, rows);

    // reconstruct from straights
    std::vector<bool> reconstructed_edgeBits_straights = reconstructStraights(straightsDecoded, edgeBitsSize, cols, rows);
    // Print the decoded straights
    // std::cout << "Decoded Straights size: " << straightsDecoded.size() << std::endl;
    // empty reconstruction
    std::vector<bool> empty_reconstruction = std::vector<bool>(edgeBitsSize, true);

    std::vector<RGB> regionColorsFromBitString = colorBitStringToRGBVector(regionColorBitStringParsed);


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
        RGB col = regionColorsFromBitString[continuousLabel];


        //RGB col = getVertexColor(0);
        //std::cout << col.green.to_ulong() << std::endl;
        // Set the color (BGR format)
        image.at<cv::Vec3b>(y, x) = cv::Vec3b(col.blue, col.green, col.red);  
    }
    
    //printSize();
    bool success = areImagesIdentical(originalImg, image);
    // Compare pixel colors between original and reconstructed images
    int differingPixels = 0;
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            cv::Vec3b originalPixel = originalImg.at<cv::Vec3b>(y, x);
            cv::Vec3b reconstructedPixel = image.at<cv::Vec3b>(y, x);
            if (originalPixel != reconstructedPixel) {
                differingPixels++;
                std::cout << "Pixel mismatch at (" << y << ", " << x << "): "
                          << "Original: (" << (int)originalPixel[2] << ", " << (int)originalPixel[1] << ", " << (int)originalPixel[0] << "), "
                          << "Reconstructed: (" << (int)reconstructedPixel[2] << ", " << (int)reconstructedPixel[1] << ", " << (int)reconstructedPixel[0] << ")" 
                          << std::endl;
            }
        }
    }
    std::cout << "Total differing pixels: " << differingPixels << std::endl;
    std::cout << (success ? "✅" : "❌") << std::endl;
    
    if(showImg){
        cv::destroyAllWindows();
        cv::imshow("Original", originalImg);
        cv::imshow("Reconstruction", image);
        cv::waitKey(0);
    }
    

    // delete huffman tree from memory 
    deleteHuffmanTree(reconstructedRoot);
    
    return success;
}

/**
 * @brief sets edge bits in vector based on the direction vector of one single path
 * @param currentEdge starting edge of the current path 
 * @param currentDir starting direction of the current path
 * @param directionVector boolen direction sequence of the current path 
 * @param reconstructedEdgeBits output edgebits vector which is to be filled with the reconstructed bits 
 */
void reconstruct_edgeBits_iterative(int currentEdge, Direction currentDir, std::vector<bool>& directionVector, std::vector<bool>& reconstructedEdgeBits, int cols, int rows, std::vector<bool>& visited){
    std::stack<std::pair<int, Direction>> pendingEdges;
    std::queue<bool> directionQueue;
    // std::vector<bool> visited(reconstructedEdgeBits.size(), false);
    for (bool dir : directionVector){
        directionQueue.push(dir);
    }
    pendingEdges.push(std::make_pair(currentEdge, currentDir));
    
    while(!pendingEdges.empty()){
        std::tie(currentEdge, currentDir) = pendingEdges.top();
        pendingEdges.pop();
        if(currentEdge == -1){
            std::cout << "edge out of bounds" << std::endl;
            continue;
        }
        reconstructedEdgeBits[currentEdge] = true;
        if(visited[currentEdge]){
            continue;
        }
        visited[currentEdge] = true;
        if(directionQueue.size() == 3){
            continue;
        }
        // check if out of bounds (or visited?)
        int leftEdge = getNeighbor(currentEdge, currentDir, 0, cols, rows);
        int forwardEdge = getNeighbor(currentEdge, currentDir, 1, cols, rows);
        int rightEdge = getNeighbor(currentEdge, currentDir, 2, cols, rows);
        if(leftEdge == -1 || forwardEdge == -1 || rightEdge == -1 || leftEdge >= reconstructedEdgeBits.size() || forwardEdge >= reconstructedEdgeBits.size() || rightEdge >= reconstructedEdgeBits.size()){
            continue;
        }
        bool left = directionQueue.front();
        directionQueue.pop();
        bool forward = directionQueue.front();
        directionQueue.pop();
        bool right = directionQueue.front();
        directionQueue.pop();
        
        if(right){
            pendingEdges.push(std::make_pair(rightEdge, nextDirection(currentDir)));
        }
        if(forward){
            pendingEdges.push(std::make_pair(forwardEdge, currentDir));
        }
        if(left){
            pendingEdges.push(std::make_pair(leftEdge, previousDirection(currentDir)));
        }
    }
    return;
}


std::vector<bool> reconstruct_edgeBits2bits(PathInfoVector paths, int edgeBitsSize, int cols, int rows){
    std::vector<bool> reconstructed_edgeBits_2bits(edgeBitsSize, false);
    for(PathInfo pathinfo : paths){
        int startEdge = std::get<0>(pathinfo);
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

std::vector<bool> reconstructStraights(Straights straights, int edgeBitsSize, int cols, int rows){
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
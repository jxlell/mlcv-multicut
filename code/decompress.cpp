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
    std::vector<bool> edgeBits01 = compImg.edgeBits01;
    std::vector<RGB> regionColors = compImg.colorVector;
    PathInfoVector paths = compImg.paths;
    std::vector<bool> pathsBitString = compImg.pathsBitString;
    PathInfoVector paths_2bit_nonRLE = compImg.pathInfoVector2bit; // unused
    RLEVector rle_paths = compImg.rleVector;
    std::vector<bool> rleBitString = compImg.rleBitString;
    Straights straights = compImg.straights;
    std::vector<bool> regionColorBitString = compImg.regionColorBitString;
    int rows = originalImg.rows;
    int cols = originalImg.cols;

    std::vector<bool> straightsHuffmanCodesBitString;// = compImg.straightsHuffmanCodesBitString;
    std::vector<uint32_t> straightsHuffmanCodesStartPoints;// = compImg.straightsHuffmanCodesStartPoints;
    HuffmanNode* root = compImg.root;
    std::vector<uint16_t> straightLengthsList;// = compImg.straightLengthsList;
    std::vector<uint32_t> straightLengthFrequencies;// = compImg.straightLengthFrequencies;
    std::vector<bool> straightsBitString = compImg.straightsBitString;

    std::vector<bool> horizontalBits = compImg.horizontalBits;
    std::vector<bool> reducedVerticalBits = compImg.reducedVerticalBits;
    std::vector<bool> reducedEdgeBitsBitString = compImg.reducedEdgeBitsBitString;

    std::vector<uint8_t> transparencyValues = compImg.transparencyValues;
    
    cv::Mat image(rows, cols, CV_8UC4, cv::Scalar(0, 0, 0, 0)); 
    int directionBitsSize;
    int edgeBitsSize = (cols-1)*rows + cols*(rows-1);

    

    // parse tree path bitstring
    std::string pathsBitStringStr;
    for (bool bit : pathsBitString) {
        pathsBitStringStr += bit ? "1" : "0";
    }

    std::cout << "length of pathsBitStringStr: " << pathsBitStringStr.size() << std::endl;
    std::cout << "bitstring compression rate: " << (rows * cols * 24) / (double)pathsBitStringStr.size() << std::endl;

    std::cout << "improvement over old edgebits: " << ((double)edgeBits01.size() / (double)(horizontalBits.size() + reducedVerticalBits.size()) - 1);

    std::string cols_str = pathsBitStringStr.substr(0, 16);
    int cols_int = std::stoi(cols_str, nullptr, 2);
    std::cout << "cols: " << cols_int << std::endl;
    pathsBitStringStr = pathsBitStringStr.substr(16);
    std::string rows_str = pathsBitStringStr.substr(0, 16);
    int rows_int = std::stoi(rows_str, nullptr, 2);
    std::cout << "rows: " << rows_int << std::endl;
    pathsBitStringStr = pathsBitStringStr.substr(16);

    int regionColorBitsSize = std::ceil(std::log2(cols_int * rows_int));
    std::string regionColorBitsSizeStr = pathsBitStringStr.substr(0, regionColorBitsSize);
    int regionColorBitsSizeInt = std::stoi(regionColorBitsSizeStr, nullptr, 2);
    std::cout << "regionColorBitsSize: " << regionColorBitsSizeInt << std::endl;
    pathsBitStringStr = pathsBitStringStr.substr(regionColorBitsSize);
    std::string regionColorBitStringStr = pathsBitStringStr.substr(0, regionColorBitsSizeInt*24);
    std::vector<bool> regionColorBitStringParsed;
    for(char c : regionColorBitStringStr){
        regionColorBitStringParsed.push_back(c == '1');
    }
    pathsBitStringStr = pathsBitStringStr.substr(regionColorBitStringStr.size());


    //int disconnectedComponentsBits = std::ceil(std::log2(cols * rows / 2));
    int disconnectedComponentsBits = std::log2(std::ceil(static_cast<double>(cols_int)/2) * std::ceil(static_cast<double>(rows_int)/2));
    std::cout << "disconnect bits: " << disconnectedComponentsBits << std::endl;
    // std::cout << "Paths bitstring: " << pathsBitStringStr << std::endl;
    std::string numberOfComponentsStr = pathsBitStringStr.substr(0,disconnectedComponentsBits);
    // std::cout << "Number of components: " << numberOfComponentsStr << std::endl;
    int numberOfComponets = std::stoi(numberOfComponentsStr, nullptr, 2);
    std::cout << "Number of components: " << numberOfComponets << std::endl;
    std::string startPointBitsStr = pathsBitStringStr.substr(disconnectedComponentsBits, 5);
    int startPointBits = std::stoi(startPointBitsStr, nullptr, 2);
     std::cout << "Start point bits: " << startPointBits << std::endl;
    pathsBitStringStr = pathsBitStringStr.substr(disconnectedComponentsBits + 5);

    std::string startPointBitstring = pathsBitStringStr.substr(0, startPointBits * numberOfComponets);
    std::string directionsBitstring = pathsBitStringStr.substr(startPointBits * numberOfComponets);

    std::vector<uint32_t> startPoints; 
    for(size_t i = 0; i<startPointBitstring.size(); i+=startPointBits){
        std::string currentStr = startPointBitstring.substr(i, startPointBits);
        uint32_t startPoint = std::stoi(currentStr, nullptr, 2);
        // std::cout << "Start point: " << startPoint << std::endl;
        startPoints.push_back(startPoint);
    }
    std::cout << "start points size: " << startPoints.size() << std::endl;

    //std::vector<std::vector<bool>> directions = directionsVectorFromBitstring(directionsBitstring, directionBitsSize);
    
    //std::cout << "directions size: " << directions.size() << std::endl;

    // Compile paths vector from startPoints and directions
    // PathInfoVector paths_from_bitstring;
    // for (size_t i = 0; i < startPoints.size(); ++i) {
    //     paths_from_bitstring.emplace_back(startPoints[i], getDirectionFromIndex(startPoints[i], rows, cols), directions[i]);
    // }

    // Print paths from bitstring
    // std::cout << "Paths from bitstring:" << std::endl;
    // for (const auto& path : paths_from_bitstring) {
    //     std::cout << "Start Edge: " << std::get<0>(path) << ", Directions: ";
    //     for (bool dir : std::get<2>(path)) {
    //         std::cout << dir;
    //     }
    //     std::cout << std::endl;
    // }

    size_t bitIndex = 0;

    // Parse reduced edge bits bitstring

    // Convert vector<bool> to string efficiently
    std::string reducedEdgeBitsBitStringStr;
    reducedEdgeBitsBitStringStr.reserve(reducedEdgeBitsBitString.size());
    for (bool bit : reducedEdgeBitsBitString) {
        reducedEdgeBitsBitStringStr += bit ? '1' : '0';
    }
    
    // Parse cols and rows
    cols_int = std::stoi(reducedEdgeBitsBitStringStr.substr(bitIndex, 16), nullptr, 2);
    bitIndex += 16;
    rows_int = std::stoi(reducedEdgeBitsBitStringStr.substr(bitIndex, 16), nullptr, 2);
    bitIndex += 16;
    
    std::cout << "rows: " << rows_int << std::endl;
    
    // Compute region color bits size
    regionColorBitsSize = std::ceil(std::log2(cols_int * rows_int));
    regionColorBitsSizeInt = std::stoi(reducedEdgeBitsBitStringStr.substr(bitIndex, regionColorBitsSize), nullptr, 2);
    bitIndex += regionColorBitsSize;
    
    std::cout << "regionColorBitsSize: " << regionColorBitsSizeInt << std::endl;
    
    // Parse region color bits
    regionColorBitStringParsed.reserve(regionColorBitsSizeInt * 24);
    for (size_t i = 0; i < regionColorBitsSizeInt * 24; ++i) {
        regionColorBitStringParsed.push_back(reducedEdgeBitsBitStringStr[bitIndex++] == '1');
    }
    
    // Parse horizontal bits amount
    int horizontalBitsAmount = std::stoi(reducedEdgeBitsBitStringStr.substr(bitIndex, 32), nullptr, 2);
    bitIndex += 32;
    
    // Parse horizontal bits
    for (size_t i = 0; i < horizontalBitsAmount; ++i) {
        horizontalBits[i] = (reducedEdgeBitsBitStringStr[bitIndex++] == '1');
    }
    
    // Parse reduced vertical bits amount
    int reducedVerticalBitsAmount = std::stoi(reducedEdgeBitsBitStringStr.substr(bitIndex, 32), nullptr, 2);
    bitIndex += 32;
    
    // Parse reduced vertical bits
    for (size_t i = 0; i < reducedVerticalBitsAmount; ++i) {
        reducedVerticalBits[i] = (reducedEdgeBitsBitStringStr[bitIndex++] == '1');
    }
    

    // // Parse edgebits bitstring
    // bitIndex = 0;

    // // Convert vector<bool> to string efficiently
    // std::string edgeBitsBitStringStr;
    // edgeBitsBitStringStr.reserve(edgeBits01.size());
    // for (bool bit : edgeBits01) {
    //     edgeBitsBitStringStr += bit ? '1' : '0';
    // }

    // // Extract cols and rows
    // cols_str = edgeBitsBitStringStr.substr(bitIndex, 16);
    // cols_int = std::stoi(cols_str, nullptr, 2);
    // bitIndex += 16;

    // rows_str = edgeBitsBitStringStr.substr(bitIndex, 16);
    // rows_int = std::stoi(rows_str, nullptr, 2);
    // bitIndex += 16;

    // std::cout << "cols: " << cols_int << std::endl;
    // std::cout << "rows: " << rows_int << std::endl;

    // // Compute region color bits size
    // regionColorBitsSize = std::ceil(std::log2(cols_int * rows_int));
    // regionColorBitsSizeStr = edgeBitsBitStringStr.substr(bitIndex, regionColorBitsSize);
    // regionColorBitsSizeInt = std::stoi(regionColorBitsSizeStr, nullptr, 2);
    // bitIndex += regionColorBitsSize;

    // std::cout << "regionColorBitsSize: " << regionColorBitsSizeInt << std::endl;

    // // Parse region color bit string
    // regionColorBitStringStr = edgeBitsBitStringStr.substr(bitIndex, regionColorBitsSizeInt * 24);
    // regionColorBitStringParsed.clear();
    // regionColorBitStringParsed.reserve(regionColorBitsSizeInt * 24);
    // for (char c : regionColorBitStringStr) {
    //     regionColorBitStringParsed.push_back(c == '1');
    // }
    // bitIndex += regionColorBitStringStr.size();

    // // Parse edge bits amount
    // std::string edgeBitsAmountStr = edgeBitsBitStringStr.substr(bitIndex, 32);
    // int edgeBitsAmount = std::stoi(edgeBitsAmountStr, nullptr, 2);
    // bitIndex += 32;

    // // Parse edge bits
    // for (size_t i = 0; i < edgeBitsAmount; i++) {
    //     edgeBits01[i] = edgeBitsBitStringStr[bitIndex++] == '1';
    // }








    std::cout << "parse huffman straights bitstring" << std::endl;
    bitIndex = 0;

    // Convert bool vector to a string representation (can be avoided if direct bit operations are used)
    std::string straightsBitStringStr;
    for (bool bit : straightsBitString) {
        straightsBitStringStr += bit ? '1' : '0';
    }

    // Read 16-bit cols
    cols_int = std::stoi(straightsBitStringStr.substr(bitIndex, 16), nullptr, 2);
    bitIndex += 16;
    std::cout << "cols: " << cols_int << std::endl;

    // Read 16-bit rows
    rows_int = std::stoi(straightsBitStringStr.substr(bitIndex, 16), nullptr, 2);
    bitIndex += 16;
    std::cout << "rows: " << rows_int << std::endl;

    // Compute and read regionColorBitsSize
    regionColorBitsSize = std::ceil(std::log2(cols_int * rows_int));
    regionColorBitsSizeInt = std::stoi(straightsBitStringStr.substr(bitIndex, regionColorBitsSize), nullptr, 2);
    bitIndex += regionColorBitsSize;
    std::cout << "regionColorBitsSize: " << regionColorBitsSizeInt << std::endl;

    // Read region color bitstring
    regionColorBitStringParsed.clear();
    for (size_t i = 0; i < regionColorBitsSizeInt * 24; ++i) {
        regionColorBitStringParsed.push_back(straightsBitStringStr[bitIndex++] == '1');
    }

    // Read Huffman start points amount (32 bits)
    int huffmanStartPointsAmount = std::stoi(straightsBitStringStr.substr(bitIndex, 32), nullptr, 2);
    bitIndex += 32;
    std::cout << "huffmanStartPointsAmount: " << huffmanStartPointsAmount << std::endl;

    // Read Huffman start points bit size (5 bits)
    int huffmanStartPointsBits = std::stoi(straightsBitStringStr.substr(bitIndex, 5), nullptr, 2);
    bitIndex += 5;
    std::cout << "huffmanStartPointsBits: " << huffmanStartPointsBits << std::endl;

    // Read Huffman start points
    straightsHuffmanCodesStartPoints.clear();
    for (size_t i = 0; i < huffmanStartPointsAmount; ++i) {
        uint32_t startPoint = std::stoi(straightsBitStringStr.substr(bitIndex, huffmanStartPointsBits), nullptr, 2);
        bitIndex += huffmanStartPointsBits;
        straightsHuffmanCodesStartPoints.push_back(startPoint);
    }

    // Read Huffman string size (64 bits)
    int huffmanStringSize = std::stoi(straightsBitStringStr.substr(bitIndex, 64), nullptr, 2);
    bitIndex += 64;
    std::cout << "huffmanStringSize: " << huffmanStringSize << std::endl;

    // Read Huffman string
    std::string huffmanString = straightsBitStringStr.substr(bitIndex, huffmanStringSize);
    bitIndex += huffmanStringSize;

    // Read straightsLengthsBits (5 bits)
    int straightsLengthsBits = std::stoi(straightsBitStringStr.substr(bitIndex, 5), nullptr, 2);
    bitIndex += 5;
    std::cout << "straightsLengthsBits: " << straightsLengthsBits << std::endl;

    // Read straightsLengths (16 bits)
    int straightsLengths = std::stoi(straightsBitStringStr.substr(bitIndex, 16), nullptr, 2);
    bitIndex += 16;
    std::cout << "straightsLengths: " << straightsLengths << std::endl;

    // Read straight lengths
    straightLengthsList.clear();
    for (size_t i = 0; i < straightsLengths; ++i) {
        uint16_t length = std::stoi(straightsBitStringStr.substr(bitIndex, straightsLengthsBits), nullptr, 2);
        bitIndex += straightsLengthsBits;
        straightLengthsList.push_back(length);
    }

    // Read straightsFrequenciesSize (16 bits)
    int straightsFrequenciesSize = std::stoi(straightsBitStringStr.substr(bitIndex, 16), nullptr, 2);
    bitIndex += 16;
    std::cout << "straightsFrequenciesSize: " << straightsFrequenciesSize << std::endl;

    // Read straightsFrequenciesBits (5 bits)
    int straightsFrequenciesBits = std::stoi(straightsBitStringStr.substr(bitIndex, 5), nullptr, 2);
    bitIndex += 5;
    std::cout << "straightsFrequenciesBits: " << straightsFrequenciesBits << std::endl;

    // Read frequency values
    straightLengthFrequencies.clear();
    for (size_t i = 0; i < straightsFrequenciesSize; ++i) {
        uint32_t frequency = std::stoi(straightsBitStringStr.substr(bitIndex, straightsFrequenciesBits), nullptr, 2);
        bitIndex += straightsFrequenciesBits;
        straightLengthFrequencies.push_back(frequency);
    }



    std::string rleBitStringStr;
    for (bool bit : rleBitString) {
        rleBitStringStr += bit ? '1' : '0';
    }

    std::cout << "rle parse" << std::endl;
    size_t index = 0;

    auto extract_bits = [&](size_t length) {
        std::string result = rleBitStringStr.substr(index, length);
        index += length;
        return result;
    };

    auto extract_int = [&](size_t length) {
        return std::stoi(extract_bits(length), nullptr, 2);
    };

    cols_int = extract_int(16);
    std::cout << "cols: " << cols_int << std::endl;
    rows_int = extract_int(16);
    std::cout << "rows: " << rows_int << std::endl;

    regionColorBitsSize = std::ceil(std::log2(cols_int * rows_int));
    regionColorBitsSizeInt = extract_int(regionColorBitsSize);
    regionColorBitStringParsed.clear();

    regionColorBitStringStr = extract_bits(regionColorBitsSizeInt * 24);
    regionColorBitStringParsed.reserve(regionColorBitStringStr.size());
    for (char c : regionColorBitStringStr) {
        regionColorBitStringParsed.push_back(c == '1');
    }

    int rleStartPointsAmount = extract_int(32);
    int rleStartPointsBits = extract_int(5);
    std::vector<std::vector<bool>> rleStartPoints(rleStartPointsAmount);

    for (auto &startPoint : rleStartPoints) {
        startPoint.reserve(rleStartPointsBits);
        std::string startPointStr = extract_bits(rleStartPointsBits);
        for (char c : startPointStr) {
            startPoint.push_back(c == '1');
        }
    }

    RLEVector rle_paths_parsed;
    std::vector<std::vector<bool>> zeros(rleStartPoints.size());
    std::vector<std::vector<std::vector<bool>>> ones(rleStartPoints.size());
    std::vector<bool> startIndicator;

    for (size_t i = 0; i < rleStartPoints.size(); i++) {
        int numberOfZeros = extract_int(16);
        std::string zerosStr = extract_bits(numberOfZeros);
        zeros[i].reserve(zerosStr.size());
        for (char c : zerosStr) {
            zeros[i].push_back(c == '1');
        }
        
        int numberOfOnes = extract_int(16);
        ones[i].reserve(numberOfOnes);
        for (int j = 0; j < numberOfOnes; j++) {
            std::string onesStr = extract_bits(16);
            std::vector<bool> currentOnes;
            currentOnes.reserve(onesStr.size());
            for (char c : onesStr) {
                currentOnes.push_back(c == '1');
            }
            ones[i].push_back(std::move(currentOnes));
        }
        
        startIndicator.push_back(extract_bits(1)[0] == '1');
        rle_paths_parsed.emplace_back(rleStartPoints[i], zeros[i], ones[i], startIndicator[i]);
    }

    


    // convert from rle_paths to paths_2bit
    PathInfoVector paths_2bit;
    for (auto rle : rle_paths_parsed) {
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
    std::tie(decodedWord, straightsLengthsDecoded) = decodeHuffman(reconstructedRoot, huffmanString);
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

    std::vector<bool> reconstructed_edgeBits_from_paths(edgeBitsSize, false);
    int i = 0;

    std::vector<bool> visited(edgeBitsSize, false);
    // for(PathInfo pathinfo : paths_from_bitstring){
    //     //std::cout << directionToString(std::get<1>(pathinfo)) << std::endl;
    //     //printProgressBar(i , paths.size());
    //     //reconstructed_edgeBits = reconstruct_edgeBits_iterative(std::get<0>(pathinfo), std::get<1>(pathinfo), std::get<2>(pathinfo));
    //     int startEdge = std::get<0>(pathinfo);
    //     //Direction calculated just based on the index, no need to pass it as an argument
    //     Direction currentDir = getDirectionFromIndex(startEdge, rows, cols);
    //     reconstruct_edgeBits_iterative(startEdge, currentDir, std::get<2>(pathinfo), reconstructed_edgeBits_from_paths, cols, rows, visited);
    //     directionBitsSize += std::get<2>(pathinfo).size();
    //     //break;
    //     i++;
    // }

    reconstructed_edgeBits_from_paths.assign(edgeBitsSize, false);
    visited.assign(edgeBitsSize, false);
    std::queue<bool> directionQueue;
    for (char c : directionsBitstring){
        directionQueue.push(c == '1');
    }
    for(size_t i = 0; i < startPoints.size(); i++){
        reconstruct_edgeBits_iterative(startPoints[i], getDirectionFromIndex(startPoints[i], rows, cols), reconstructed_edgeBits_from_paths, cols, rows, visited, directionQueue);
    }
    

    //reconstruct edgebits from horizontals/verticals
    std::vector<bool> reconstructed_edgeBits_horizontals = reconstruct_edgeBits_from_Horizontals(horizontalBits, reducedVerticalBits, cols, rows);

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
    andres::Partition<int> reconstruction = getRegions(reconstructed_edgeBits_from_paths, rows, cols);
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
        image.at<cv::Vec4b>(y, x) = cv::Vec4b(col.blue, col.green, col.red, transparencyValues[index]);  
    }

    // for (auto t : transparencyValues){
    //     std::cout << (int)t << ", ";
    // }
    // bool transparencyMatch = true;
    // for (int y = 0; y < rows; ++y) {
    //     for (int x = 0; x < cols; ++x) {
    //         int index = y * cols + x;
    //         if (image.at<cv::Vec4b>(y, x)[3] != transparencyValues[index]) {
    //             transparencyMatch = false;
    //             std::cout << "Mismatch at (" << y << ", " << x << "): "
    //                       << "Image transparency = " << (int)image.at<cv::Vec4b>(y, x)[3]
    //                       << ", Vector transparency = " << (int)transparencyValues[index] << std::endl;
    //         }
    //     }
    // }
    // std::cout << (transparencyMatch ? "All transparency values match." : "Transparency values do not match.") << std::endl;


    //printSize();
    //bool success = areImagesIdentical(originalImg, image);
    bool success = (edgeBits01 == reconstructed_edgeBits_from_paths) && (edgeBits01 == reconstructed_edgeBits_horizontals) && (edgeBits01 == reconstructed_edgeBits_2bits) && (edgeBits01 == reconstructed_edgeBits_straights);
    std::cout << (success ? "✅" : "❌") << std::endl;
    
    if(showImg){
        cv::imwrite("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/output_files/reconstructed.png", image);
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
void reconstruct_edgeBits_iterative(int currentEdge, Direction currentDir, std::vector<bool>& reconstructedEdgeBits, int cols, int rows, std::vector<bool>& visited, std::queue<bool>& directionQueue){
    std::stack<std::pair<int, Direction>> pendingEdges;
    pendingEdges.push(std::make_pair(currentEdge, currentDir));
    visited[currentEdge] = true;
    
    while(!pendingEdges.empty()){
        std::tie(currentEdge, currentDir) = pendingEdges.top();
        pendingEdges.pop();
        reconstructedEdgeBits[currentEdge] = true;
        // check if out of bounds (or visited?)
        int leftEdge = getNeighbor(currentEdge, currentDir, 0, cols, rows);
        int forwardEdge = getNeighbor(currentEdge, currentDir, 1, cols, rows);
        int rightEdge = getNeighbor(currentEdge, currentDir, 2, cols, rows);
        if(leftEdge == -1 || forwardEdge == -1 || rightEdge == -1 || leftEdge >= reconstructedEdgeBits.size() || forwardEdge >= reconstructedEdgeBits.size() || rightEdge >= reconstructedEdgeBits.size()){
            continue;
        }
        bool left = false;
        if(!visited[leftEdge]){
            left = directionQueue.front();
            directionQueue.pop();
            visited[leftEdge] = true;
        }
        bool front = false;
        if(!visited[forwardEdge]){
            front = directionQueue.front();
            directionQueue.pop();
            visited[forwardEdge] = true;
        }
        bool right = false;
        if(!visited[rightEdge]){
            right = directionQueue.front();
            directionQueue.pop();
            visited[rightEdge] = true;
        }

        if(right){
            pendingEdges.push(std::make_pair(rightEdge, nextDirection(currentDir)));
        }
        if(front){
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


std::vector<bool> reconstruct_edgeBits_from_Horizontals(std::vector<bool>& horizontalBits, std::vector<bool>& reducedVerticals, int cols, int rows){
    std::vector<bool> reconstructed_edgeBits(rows * (cols - 1) + cols * (rows - 1), false);

    int row = 0;
    int col = 0;
    int reducedVerticalIndex = 0;
    int verticalIndex = 0;
    int edgebitsIndex = 1;
    
    //reconstructed_edgeBits[1] = reducedVerticals[0];
    while(true){
        if(col == cols - 1){
            break;
        }

        //edgebitsIndex = mapVerticalToEdgebitsIndex(verticalIndex, cols, rows);
        reconstructed_edgeBits[edgebitsIndex] = reducedVerticals[reducedVerticalIndex];
        edgebitsIndex = getNeighbor(edgebitsIndex, Direction::DOWN, 1, cols, rows);

        if(row == rows - 1){
            row = 0;
            col++;
            reducedVerticalIndex++;
            verticalIndex++;
            edgebitsIndex = col * 2 + 1;
            continue;
        }

        int leftI = row + (rows-1) * col;
        int rightI = row + (rows-1) * (col + 1);
        bool left = horizontalBits[leftI];
        bool right = horizontalBits[rightI];

        if(left || right){
            reducedVerticalIndex++;
        }

        verticalIndex++;
        row++;
        
    }

    // set known horizontal bits in edgebitmap
    int horizontalIndex = 0;
    for (int col = 0; col < cols; col++){
        for (int row = 0; row < rows - 1; row++){
            int edgeIndex = row * (cols * 2 - 1) + col * 2;
            reconstructed_edgeBits[edgeIndex] = horizontalBits[horizontalIndex];
            horizontalIndex++;
        }
    }


    return reconstructed_edgeBits;
}


std::vector<std::vector<bool>> directionsVectorFromBitstring(std::string directionsBitstring, int directionBitsSize){
    std::string currentDirectionBitstring;
    std::vector<std::vector<bool>> directions;
    for(size_t i = 0; i<directionsBitstring.size(); i++){
        currentDirectionBitstring += directionsBitstring[i];
        if(currentDirectionBitstring.size() >= 3 && 
        currentDirectionBitstring.substr(currentDirectionBitstring.size() - 3) == "000"){
            std::vector<bool> currentDirectionVector;
            for(size_t j = 0; j<currentDirectionBitstring.size(); j++){
                currentDirectionVector.push_back(currentDirectionBitstring[j] == '1');
            }
            directions.push_back(currentDirectionVector);
            currentDirectionBitstring.clear();
        }
    }
    return directions;
}
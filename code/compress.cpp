#include "compress.h"
#include <iostream>
#include <tuple>
#include <vector>
#include "Util.h"
#include "DirectionPath.h"
#include <opencv2/opencv.hpp>
#include "Multicut.h"
#include "mc.h"
#include "partition.hxx"
#include <map>
#include <string>
#include "huffman.h"
#include <chrono>



CompressedImage compress(const std::string& imagePath){
    cv::Mat img = cv::imread(imagePath, cv::IMREAD_COLOR);
    std::vector<bool> edgeBits01((img.cols-1)*img.rows + img.cols*(img.rows-1), false);
    std::vector<int> neighborsOffsets = {img.cols, 1};
    std::vector<RGB> regionColors;
    PathInfoVector paths;
    PathInfoVector paths_2bit; 
    RLEVector rle_paths;
    Straights straights;
    std::vector<bool> straightsHuffmanCodesBitString;
    std::vector<uint32_t> straightsHuffmanCodesStartPoints;
    HuffmanNode* root; 
    std::vector<uint16_t> straightLengthsList;
    std::vector<uint32_t> straightLengthFrequencies;
    edgeBits01 = setEdgeBits(img, edgeBits01, neighborsOffsets);
    int bits = 0;

    regionColors = setRegions(img, neighborsOffsets, img.cols * img.rows);
    auto regionColorBitString = boolVectorFromRGBVector(regionColors);
    //calculate old compression rate
    double oldCompressionRate = static_cast<double>(img.rows*img.cols*24)/(edgeBits01.size() + regionColors.size() * 24);
    // std::cout << "Old Compression Rate: " << oldCompressionRate << std::endl;

    paths = setPaths(edgeBits01, img);

    auto _2bitpaths = set2BitPaths(edgeBits01, img);
    paths_2bit = std::get<0>(_2bitpaths);
    // edgeI,  zeros,          ones,           start
    // 32 bit, 1bit (vector),  16bit (vector), 1bit
    rle_paths = std::get<1>(_2bitpaths);

    // calculate storage size for RLE-paths
    bits = 0;
    bits += calculateBoolVectorStorage(regionColorBitString);
    bits += rle_paths.size() * 32 + 24*8; // 32 bits for start points
    bits += rle_paths.size() + 24*8; // 1 bit to indicate if ones or zeros vector starts 

    for(auto rle : rle_paths){
        bits += std::get<1>(rle).size();
        bits += 24*8; // overhead
        bits += std::get<2>(rle).size() * 16;
        bits += 24*8; // overhead
    }
    double rleCompressionRate = static_cast<double>(img.cols * img.rows * 24) / bits;
    // std::cout << "RLE Compression Rate: " << rleCompressionRate << std::endl;

    // calculate storage size for paths (2bit-directions)
    bits = 0;
    bits += calculateBoolVectorStorage(regionColorBitString);
    bits += paths_2bit.size() * 16 + 24*8; // start points
    //direction vector 
    for(auto path : paths_2bit){
        bits += std::get<2>(path).size();
        bits += 24*8; // overhead 
    }
    double pathCompressionRate = static_cast<double>(img.cols * img.rows * 24) / bits;
    // std::cout << "Path Compression Rate: " << pathCompressionRate << std::endl;

    std::tie(straights, straightsHuffmanCodesBitString, straightsHuffmanCodesStartPoints, root, straightLengthsList, straightLengthFrequencies) = setStraights(edgeBits01, img);
    // calculate storage size for straights (huffman encoded)
    bits = 0;
    bits += calculateBoolVectorStorage(straightsHuffmanCodesBitString);
    bits += straightsHuffmanCodesStartPoints.size() * 32 + 24*8;
    bits += calculateBoolVectorStorage(regionColorBitString);
    bits += straightLengthsList.size() * 16 + 24*8;
    bits += straightLengthFrequencies.size() * 16 + 24*8;
    double straightsHuffmanCompressionRate = static_cast<double>(img.cols * img.rows * 24) / bits;
    // std::cout << "Straights Huffman Compression Rate: " << straightsHuffmanCompressionRate << std::endl;
    // calculate storage size for straights (non-huffman encoded)
    bits = 0;
    bits += calculateBoolVectorStorage(regionColorBitString);
    bits += straights.size() * 32 + 24*8; // 32 bits for start points
    bits += straights.size() * 16 + 24*8; // 16 bits for lengths
    double straightsCompressionRate = static_cast<double>(img.cols * img.rows * 24) / bits;
    // std::cout << "Straights Compression Rate: " << straightsCompressionRate << std::endl;


    return {regionColors, paths, img, paths_2bit, rle_paths, straights, 
    regionColorBitString, straightsHuffmanCodesBitString, 
    straightsHuffmanCodesStartPoints, root, straightLengthsList, straightLengthFrequencies,
    pathCompressionRate, rleCompressionRate ,oldCompressionRate,straightsCompressionRate,straightsHuffmanCompressionRate};
}


std::vector<bool> setEdgeBits(cv::Mat img, std::vector<bool> edgeBits01, std::vector<int> neighborsOffsets){
    //std::cout << "\nsetting multicut bits\n";
    int regionIndex = 0;
    int edgeIndex = -1;
    // Check neighbors for every vertex and set multicut bits
    int vertices = img.cols * img.rows; 

    for (int v = 0; v < vertices; ++v) {
        RGB currentColor = getVertexColor(v, vertices, img);
        // get color of previous pixel (current pixel if there is no previous)
        RGB previousColor = getVertexColor(std::max(0,v-1), vertices, img);
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
                    if(!compareRGB(currentColor, getVertexColor(neighbor, vertices, img))){
                        //set edgeBit01
                        edgeBits01[edgeIndex] = true;
                        //std::cout << "offset: " << offset << ", ";
                    }
                }
            }
        }
        //printProgressBar(v, getVertices());
    }

    return edgeBits01;
}

std::vector<RGB> setRegions(cv::Mat img, std::vector<int> neighborsOffsets, int vertices){
    andres::Partition<int> multicutregion = getRegionsFromImage(img, neighborsOffsets, vertices);
    std::vector<RGB> regionColors;
    std::vector<int> reps;
    multicutregion.representatives(std::back_inserter(reps));
    //std::cout << std::endl;
    for (int rep : reps) {
        //std::cout << rep << ", ";
        regionColors.push_back(getVertexColor(rep, vertices, img));
        //std::cout << static_cast<int>(repRGB.red) << ", ";
    }
    regionColors.resize(reps.size());
    return regionColors;
}

PathInfoVector setPaths(std::vector<bool> edgeBits01, cv::Mat img){
    int edgeI = 0;
    int dfsI = 0;
    std::vector<bool> visited(edgeBits01.size(), false);
    PathInfoVector paths;
    
    for (bool edge : edgeBits01){
        // skip non-multicut edges or previously visited edges ||
        if(!edge){
            edgeI++;
            continue;
        }
        if(visited[edgeI]){
            edgeI++;
            continue;
        }
        //std::cout << std::endl << edgeI;

        // dir rausfinden
        // get current direction (either down or right) from current index 
        Direction currentDir = getDirectionFromIndex(edgeI, img.rows, img.cols);

        dfsI++;

        std::vector<bool> directionVector;
        //directionVector = dfs_paths_recursive(edgeI, visited, currentDir, directionVector);
        directionVector = dfs_paths_iterative(edgeI, currentDir, visited, img, edgeBits01);
        paths.emplace_back(intToBool(edgeI), currentDir, directionVector);
        //std::cout << "path size: " << directionVector.size() << std::endl;

        edgeI++;
    }
    //std::cout << "edgeI: " << edgeI << std::endl;
    //std::cout << "number of edges: " << 2*rows*cols - cols - rows << std::endl;

    //printPaths();

    // Calculate storage space for the path vector
    double totalBitsPathVector = 0;
    for (const auto& path : paths) {
        std::vector<bool> edgeI;
        Direction dir;
        std::vector<bool> directionVector;

        std::tie(edgeI, dir, directionVector) = path;
        totalBitsPathVector += edgeI.size();
        
        //totalBitsPathVector += 32; // 32 bits for starting point
        //totalBitsPathVector += ceil(log2(2 * img.rows * img.cols - img.rows - img.cols)); // 32 bits for starting point
        if(!directionVector.empty()){
            //direction wird berechnet und nicht mehr gespeichert
            //totalBitsPathVector += 8;  // 8 bits for starting direction (smallest addressable unit)
            totalBitsPathVector += directionVector.size(); // Size of directionVector in bits
        }
    }

    //std::cout << "Total bits for path vector: " << totalBitsPathVector << std::endl;
    //std::cout << "bits needed per start point: " << ceil(log2(2 * img.rows * img.cols - img.rows - img.cols)) << std::endl;
    
    int disconnectedComponents = dfsI;
    
    //std::cout << "number of disconnected components: " << dfsI << std::endl;

    return paths;
}

std::tuple<PathInfoVector, RLEVector> set2BitPaths(std::vector<bool> edgeBits01, cv::Mat img){
    std::vector<bool> visited(edgeBits01.size(), false);
    RLEVector rle_paths;
    PathInfoVector paths_2bit;

    for (int edgeI = 0; edgeI < edgeBits01.size(); edgeI++){
        int iter_i = 0;
        if(visited[edgeI]){continue;}
        if(!edgeBits01[edgeI]){
            visited[edgeI] = true;
            continue;
        }
        visited[edgeI] = true;
        //std::cout << "pass if\n";
        // get current direction (either down or right) from current index 
        std::vector<bool> directions2bits; 
        Direction startDir;
        Direction currentDir;
        startDir = getDirectionFromIndex(edgeI, img.rows, img.cols);
        currentDir = startDir;
        int currentEdge = edgeI;
        int left = 0;
        int front = 0;
        int right = 0;
        while(true){
            //std::cout << "iter: " << iter_i++ << std::endl;
            //FIXME: manchmal endlosschleife (gradient.png)
            left = getNeighbor(currentEdge, currentDir, 0, img.cols, img.rows);
            front = getNeighbor(currentEdge, currentDir, 1, img.cols, img.rows);
            right = getNeighbor(currentEdge, currentDir, 2, img.cols, img.rows);

            // edge out of bounds 
            if(left == -1 || front == -1 || right == -1 || 
            left >= edgeBits01.size() || front >= edgeBits01.size() || right >= edgeBits01.size()){
                visited[currentEdge] = true;
                break; 
            }

            // check if all neighbors are visited
            //TODO: wird eigentlich auch noch von den folgenden ifs abgefangen
            if(visited[left] && visited[front] && visited[right]){
                visited[currentEdge] = true;
                break;
            }

            //front
            if(edgeBits01[front] && !visited[front]){
                currentEdge = getNeighbor(currentEdge, currentDir, 1, img.cols, img.rows);
                directions2bits.push_back(1);
                directions2bits.push_back(1);
                visited[currentEdge] = true;
                continue;
            }

            //left
            if(edgeBits01[left] && !visited[left]){
                currentEdge = getNeighbor(currentEdge, currentDir, 0, img.cols, img.rows);
                currentDir = previousDirection(currentDir);
                directions2bits.push_back(1);
                directions2bits.push_back(0);
                visited[currentEdge] = true;
                continue;
            }
            
            //right
            if(edgeBits01[right] && !visited[right]){
                currentEdge = getNeighbor(currentEdge, currentDir, 2, img.cols, img.rows);
                currentDir = nextDirection(currentDir);
                directions2bits.push_back(0);
                directions2bits.push_back(1);
                visited[currentEdge] = true;
                continue;
            }else{
                //std::cout << edgeBits01[front] << std::endl;
                //std::cerr << "Warning: No valid path found from edge " << currentEdge << std::endl;
                visited[currentEdge] = true;
                break;
            }
        }
        // std::cout << "edgeI: " << edgeI << std::endl;
        // for (bool bit : directions2bits) {
        //     std::cout << bit;
        // }
        if(directions2bits.empty()){
            // std::cout << "empty path" << std::endl;
            rle_paths.emplace_back(intToBool(edgeI), std::vector<bool>(), std::vector<std::vector<bool>>(), false);
        }else{
            // zeros, ones, start 
            std::tuple<std::vector<bool>, std::vector<uint16_t>, bool> rle = getRLE(directions2bits);
            //vector of runs, each vector contains the length of a 1s run
            std::vector<std::vector<bool>> rle_directions;
            for(uint16_t run : std::get<1>(rle)){
                rle_directions.push_back(intToBool(run));
            }
            // edgeI,  zeros,          ones,           start
            // 32 bit, 1bit (vector),  16bit (vector), 1bit
            rle_paths.emplace_back(intToBool(edgeI), std::get<0>(rle), rle_directions, std::get<2>(rle));
        }
        paths_2bit.emplace_back(intToBool(edgeI), startDir, directions2bits);
    }

    // Calculate storage space for the paths vector
    double totalBits2BitPaths = 0;
    int emptyPathsCount = 0;
    for (const auto& path : paths_2bit) {
        std::vector<bool> edgeI;
        Direction dir;
        std::vector<bool> directions2bits;

        std::tie(edgeI, dir, directions2bits) = path;

        totalBits2BitPaths += edgeI.size();
        //totalBits2BitPaths += 32; // 32 bits for starting point
        //totalBits2BitPaths += ceil(log2(2 * img.rows * img.cols - img.rows - img.cols)); // 32 bits for starting point
        
        //totalBits2BitPaths += 8;  // 8 bits for starting direction (smallest addressable unit)
        if(!directions2bits.empty()){
            //direction wird berechnet und nicht mehr gespeichert
            //totalBits2BitPaths += 8; // only store direction if there is a path following
            totalBits2BitPaths += directions2bits.size(); // Size of directions2bits in bits
        }
        else {
            emptyPathsCount++;
        }
    }
    // std::cout << "Number of 2-bit paths: " << multicut.paths_2bit.size() << std::endl;
    // std::cout << "Total bits for 2-bit paths: " << totalBits2BitPaths << std::endl;
    // std::cout << "Number of empty paths: " << emptyPathsCount << std::endl;

    // Identify the longest run of ones in the direction vectors
    int longestRun = 0;
    for (const auto& path : paths_2bit) {
        std::vector<bool> edgeI;
        Direction dir;
        std::vector<bool> directions2bits;

        std::tie(edgeI, dir, directions2bits) = path;

        int currentRun = 0;
        for (bool bit : directions2bits) {
            if (bit) {
                currentRun++;
                if (currentRun > longestRun) {
                    longestRun = currentRun;
                }
            } else {
                currentRun = 0;
            }
        }
    }
    //std::cout << "Longest run of ones in direction vectors: " << longestRun << std::endl;

    // Calculate bits needed for run length encoding
    double totalBitsRLE = 0;
    for (const auto& path : paths_2bit) {
        std::vector<bool> edgeI;
        Direction dir;
        std::vector<bool> directions2bits;

        std::tie(edgeI, dir, directions2bits) = path;

        totalBitsRLE += edgeI.size();
        //totalBitsRLE += 32; // 32 bits for starting point 
        //totalBitsRLE += ceil(log2(2 * img.rows * img.cols - img.rows - img.cols)); // 32 bits for starting point
        if(!directions2bits.empty()){
            totalBitsRLE += 8; // only store direction if there is a path following
        }
        //totalBitsRLE += 8;  // 8 bits for starting direction (smallest addressable unit)

        int currentRun = 0;
        for (size_t i = 0; i < directions2bits.size(); ++i) {
            if (!directions2bits[i]) {
            totalBitsRLE += 16; // for length of 1s
            totalBitsRLE += 1; // for lengths of 0s (0 for length 1, 1 for length 2)
            // Skip the next bit if it is also 0
            if (i + 1 < directions2bits.size() && !directions2bits[i + 1]) {
                ++i;
            }
            }
        }
    }
    // std::cout << "Total bits for run length encoding: " << totalBitsRLE << std::endl;


    // Count the runs of 0s in the 2-bit paths
    int totalRunsOfZeros = 0;
    for (const auto& path : paths_2bit) {
        std::vector<bool> edgeI;
        Direction dir;
        std::vector<bool> directions2bits;

        std::tie(edgeI, dir, directions2bits) = path;

        bool inRun = false;
        for (bool bit : directions2bits) {
            if (!bit) {
                if (!inRun) {
                    inRun = true;
                    totalRunsOfZeros++;
                }
            } else {
                inRun = false;
            }
        }
    }
    //std::cout << "Total runs of 0s in 2-bit paths: " << totalRunsOfZeros << std::endl;

    // print 2-bit paths
    //print2bitpaths(multicut.paths_2bit);
    return {paths_2bit, rle_paths};
}

std::tuple<Straights, std::vector<bool>, std::vector<uint32_t>, HuffmanNode*, std::vector<uint16_t>, std::vector<uint32_t>> setStraights(std::vector<bool> edgeBits01, cv::Mat img){
    Straights straights;
    std::vector<bool> visited(edgeBits01.size(), false);
    for(int edgeI=0; edgeI < edgeBits01.size(); edgeI++){
        if(visited[edgeI]){continue;}
        if(edgeBits01[edgeI]){
            int count = 0;
            Direction currentDir = getDirectionFromIndex(edgeI, img.rows, img.cols);
            int currentEdge = edgeI;
            visited[currentEdge] = true;
            while (true) {
                int nextEdge = getNeighbor(currentEdge, currentDir, 1, img.cols, img.rows);

                // ensure next edge is in the scope of the edgebits vector 
                if (nextEdge == -1 || nextEdge >= edgeBits01.size() || !edgeBits01[nextEdge]) {
                    break;
                }
                // wenn nextEdge schon visited, dann weiterzählen, aber diese edge als
                // start edge aus straights entfernen (auch zugehörigen count)

                count++;
                currentEdge = nextEdge;
                visited[currentEdge] = true;
            }
            straights.push_back(std::make_tuple(intToBool(edgeI), intToBool(count)));
        }
    }

    // create histogram of straight lengths
    //std::vector<int> straightLengths(std::max(img.cols+1, img.rows+1), 0);
    std::map<int, int> straightLengths;
    for (const auto& straight : straights) {
        std::vector<bool> startEdge;
        std::vector<bool> count;
        std::tie(startEdge, count) = straight;
        straightLengths[boolVectorToInt(count)]++;
    }

    // store straighLengths
    std::vector<uint16_t> straightLengthsList;
    std::vector<uint32_t> straightLengthFrequencies;
    for (const auto& pair : straightLengths) {
        straightLengthsList.push_back(pair.first);
        straightLengthFrequencies.push_back(pair.second);
    }
    // std::cout << "length of straightLengthsList: " << straightLengthsList.size() << std::endl;

    // show length histogram 
    // std::cout << "Histogram:\n";
    // if (!straightLengths.empty()) {
    //     straightLengths.erase(straightLengths.begin());
    // }
    // for (const auto& pair : straightLengths) {
    //     std::cout << pair.first << ": " << pair.second << std::endl;
    // }

    // build and store huffman codes
    std::map<int, string> straightsHuffmanCodes;
    HuffmanNode* root; 
    std::tie(straightsHuffmanCodes, root) = buildCodes(straightLengths);
    
    // special case for only one distinct length
    if (!straightsHuffmanCodes.empty() && straightsHuffmanCodes.size() == 1) {
        auto firstElement = *straightsHuffmanCodes.begin();
        straightsHuffmanCodes[firstElement.first] = "0";
    }
    // Print Huffman codes for straights
    // std::cout << "Huffman codes for straights:" << std::endl;
    // for (const auto& pair : straightsHuffmanCodes) {
    //     std::cout << "Length: " << pair.first << " Code: " << pair.second << std::endl;
    // }
    

    //vectors to store huffman codes
    // bitstring stores all huffman codes in a single string
    std::vector<bool> straightsHuffmanCodesListString;
    // start points stores the start points of each huffman code
    std::vector<uint32_t> straightsStartPointsList;

    std::vector<uint32_t> straightsHuffmanCodesStartPoints;
    for (const auto& straight : straights) {
        std::vector<bool> startEdge;
        std::vector<bool> count;
        std::tie(startEdge, count) = straight;
        straightsHuffmanCodesStartPoints.push_back(boolVectorToInt(startEdge));
    }

    // storing start points in a vector and the list of corresponding huffman codes in a bitstring
    // std::cout << "size of codes vector: " << straightsHuffmanCodes.size() << std::endl;
    for (auto it = straightsHuffmanCodes.begin(); it != straightsHuffmanCodes.end(); ++it){
        straightsStartPointsList.push_back(it->first);
        for (char c : it->second){
            straightsHuffmanCodesListString.push_back(c == '1');
        }
    }
    // std::cout << "size of bitstring: " << straightsHuffmanCodesListString.size() << std::endl;
    // std::cout << "size of start points: " << straightsStartPointsList.size() << std::endl;
    // Print first 10 values of the bitstring
    // std::cout << "First 50 values of list string: ";
    // for (int i = 0; i < 50 && i < straightsHuffmanCodesListString.size(); ++i) {
    //     std::cout << straightsHuffmanCodesListString[i];
    // }
    // std::cout << std::endl;

    // std::cout << "Huffman codes (straights) (first 10 values):\n";
    // int i = 0;
    // for (const auto& pair : straightsHuffmanCodes) {
    //     std::cout << pair.first << ": " << pair.second << std::endl;
    //     if (i++ == 10) {
    //         break;
    //     }
    // }

    // for every straight append its length encoded as huffman code to a boolean bitstring
    std::vector<bool> straightsHuffmanCodesBitString;
    // std::cout << "straights size (compress): " << straights.size() << std::endl;
    for (const auto& straight : straights) {
        std::vector<bool> startEdge;
        std::vector<bool> count;
        std::tie(startEdge, count) = straight;
        int countInt = boolVectorToInt(count);
        std::string huffmanCode = straightsHuffmanCodes[countInt];
        for (char c : huffmanCode) {
            straightsHuffmanCodesBitString.push_back(c == '1');
        }
    }

    // Print first 5 entries of the straights vector
    // std::cout << "First 5 entries of straights vector: " << std::endl;
    // for (int i = 0; i < 5 && i < straights.size(); ++i) {
    //     std::vector<bool> startEdge;
    //     std::vector<bool> count;
    //     std::tie(startEdge, count) = straights[i];
    //     std::cout << "Start Edge: ";
    //     std::cout << boolVectorToInt(startEdge);
    //     std::cout << ", Count: ";
    //     std::cout << boolVectorToInt(count);
    //     std::cout << std::endl;
    // }

    // // Print first 15 entries of the straightsHuffmanCodesBitString
    // std::cout << "First 15 entries of straightsHuffmanCodesBitString: ";
    // for (int i = 0; i < 15 && i < straightsHuffmanCodesBitString.size(); ++i) {
    //     std::cout << straightsHuffmanCodesBitString[i];
    // }
    // std::cout << std::endl;

    // // Print first 5 entries of the straightsHuffmanCodesStartPoints
    // std::cout << "First 5 entries of straightsHuffmanCodesStartPoints: ";
    // for (int i = 0; i < 5 && i < straightsStartPointsList.size(); ++i) {
    //     std::cout << straightsStartPointsList[i] << " ";
    // }
    // std::cout << std::endl;




    // decode test
    // std::string decodeTest = "111110011001010"; // code for 60
    // std::string decodedWord = "";
    // std::vector<int> straightsLengthsDecoded;
    // std::tie(decodedWord,straightsLengthsDecoded) = decodeHuffman(root, decodeTest);
    // for(int i : straightsLengthsDecoded){
    //     std::cout << i << std::endl;
    // }

    // delete huffman tree from memory (only needed to create bitstring)
    //deleteHuffmanTree(root);

    return {straights, straightsHuffmanCodesBitString, straightsHuffmanCodesStartPoints, root, straightLengthsList, straightLengthFrequencies};
}

/**
 * @brief calculates compression rate based on the bits needed for storing the path vector
 * as well as the color vector, compares to storing color for every pixel individually
 * @return factor of compression 
 */
double getCompressionRate(std::vector<RGB> regionColors, PathInfoVector paths, cv::Mat img){
    double totalBits = 0;

    // Calculate bits for color regions
    totalBits += regionColors.size() * 3 * 8;
    totalBits += 2; 
    if(paths.size() == 0){
            return 1;
        }
    for (const auto& path : paths) {
        std::vector<bool> edgeI;
        Direction dir;
        std::vector<bool> directionVector;

        std::tie(edgeI, dir, directionVector) = path;

        totalBits += edgeI.size();
        //totalBits += 32; // 32 bits for starting point
        // starting direction is calculated and not stored
        //totalBits += 8; // 8 bits for starting direction (smalles addressable unit)
        totalBits += directionVector.size(); // Size of directionVector in bits
    }
    //std::cout << "total bits: " << totalBits << std::endl;
    // Ensure that the calculation results in a double
    double compressionRate = static_cast<double>(img.cols * img.rows) * 24.0 / totalBits;
    
    return compressionRate;
}

double getRLECompressionRate(std::vector<RGB> regionColors, RLEVector rle_paths, cv::Mat img){
    double totalBitsRLE = 0;
    // Calculate bits for color regions
    totalBitsRLE += regionColors.size() * 3 * 8;

    // Calculate bits for the paths vector
    for (const auto& path : rle_paths) {
        std::tuple <std::vector<bool>, std::vector<bool>, std::vector<std::vector<bool>>, bool> rle = path;
        //totalBitsRLE += 32; // 32 bits for starting point
        totalBitsRLE += std::get<0>(rle).size(); // Size of edgeI in bits
        totalBitsRLE += std::get<1>(rle).size(); // Size of zeros_rle in bits
        //totalBitsRLE += std::get<2>(rle).size() * 16; // Size of ones_rle in bits
        for (std::vector<bool> vec : std::get<2>(rle)){
            totalBitsRLE += vec.size();
        }
        totalBitsRLE++; // 1 bit indicates which run starts the sequence 
    }
    // std::cout << "Total bits for RLE: " << totalBitsRLE << std::endl;
    // if(static_cast<double>(vertices) * 24.0 / totalBitsRLE  > 1000){
    //     std::cout << "high rate: " << imagePath << std::endl;
    // }
    return static_cast<double>(img.cols * img.rows) * 24.0 / totalBitsRLE;
}

/**
 * @brief compression rate of method without multicut paths 
 * @return factor of compression
 */
double getOldCompressionRate(cv::Mat img, std::vector<RGB> regionColors){
    int edgeBits01size = (img.cols-1)*img.rows + img.cols*(img.rows-1);
    double compRate = static_cast<double>(img.cols * img.rows) * 24.0 / 
                      (static_cast<double>(edgeBits01size) + 
                       static_cast<double>(regionColors.size()) * 3.0 * 8.0);
    return compRate;
}


double getStraightsCompressionRate(std::vector<RGB> regionColors, Straights straights, cv::Mat img){
    double totalBitsStraights = 0;
    totalBitsStraights += regionColors.size() * 3 * 8;
    for (const auto& straight : straights) {
        std::vector<bool> startEdge;
        std::vector<bool> count;
        std::tie(startEdge, count) = straight;
        totalBitsStraights += startEdge.size();
        totalBitsStraights += count.size();
    }
    return static_cast<double>(img.rows * img.cols) * 24.0 / totalBitsStraights;
}

/**
 * @return percentage of edges connecting two different-colored pixels 
 */
double getMulticutPercentage(std::vector<bool> edgeBits01){
    return 100*std::count(edgeBits01.begin(), edgeBits01.end(), true) / edgeBits01.size();
}
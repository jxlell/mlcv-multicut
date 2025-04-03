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
    // condition never met, since IMREAD_COLOR cannot be 16U
    if (img.depth() == CV_16U) {
        std::cout << "Image depth rgb image : " << img.depth() << std::endl;
        img.convertTo(img, CV_8U, 1.0 / 256.0); // Scale down to 0-255 range
    }

    // Direction testDir = getDirectionFromIndex(36,5,5);
    // std::cout << "testDir: " << directionToString(testDir) << std::endl;

    // std::cout << getPixelIndexFromEdgeIndex(39, 5, 5).first << std::endl;
    // std::cout << getPixelIndexFromEdgeIndex(39, 5, 5).second << std::endl;
    // std::cout << getPixelIndexFromEdgeIndex(31, 7, 3).first << std::endl;
    // std::cout << getPixelIndexFromEdgeIndex(31, 7, 3).second << std::endl;
    
    // std::cout << "Stopping program for testing." << std::endl;
    // exit(0);

    cv::Mat img_transparent = cv::imread(imagePath, cv::IMREAD_UNCHANGED);
    if (img_transparent.depth() == CV_16U) {
        std::cout << "Image depth transparent image: " << img_transparent.depth() << std::endl;
        img_transparent.convertTo(img_transparent, CV_8U, 1.0 / 256.0); // Scale down to 0-255 range
        cv::imwrite("code/output_files/converted_image.png", img_transparent);
    }
    std::cout << "reading transparency" << std::endl;
    std::cout << "channels: " << img_transparent.channels() << std::endl;
    std::vector<uint8_t> transparencyValues;
    if (img_transparent.channels() == 4) {
        for (int row = 0; row < img_transparent.rows; ++row) {
            for (int col = 0; col < img_transparent.cols; ++col) {
                cv::Vec4b pixel = img_transparent.at<cv::Vec4b>(row, col);
                transparencyValues.push_back(pixel[3]); // Alpha channel
            }
        }
        // std::cout << "transparency 1: " << (int)transparencyValues[0] << std::endl;
    }
    else {
        transparencyValues = std::vector<uint8_t>(img.rows * img.cols, 255);
    }



    methods compressionMethods = {
        true, // useEdgebits
        true, // useReducedEdgebits
        true, // useTree
        true, // use2bits
        true, // useRLE
        true, // useStraights
        true  // useHuffman
    };  


    

    std::vector<bool> edgeBits01((img.cols-1)*img.rows + img.cols*(img.rows-1), false);
    std::vector<bool> edgeBitsBitString;
    std::vector<bool> reducedEdgeBitsBitString;
    std::vector<int> neighborsOffsets = {img.cols, 1};
    std::vector<RGB> regionColors;
    PathInfoVector paths;
    std::vector<bool> pathsBitString; 
    PathInfoVector paths_2bit; 
    std::vector<bool> paths2bitBitString;
    RLEVector rle_paths;
    std::vector<bool> rleBitString;
    std::vector<bool> paths2bitRLEBitString;
    Straights straights;
    std::vector<bool> straightsNoHuffBitString;
    std::vector<bool> straightsBitString;
    std::vector<bool> straightsHuffmanCodesBitString;
    std::vector<uint32_t> straightsHuffmanCodesStartPoints;
    HuffmanNode* root; 
    std::vector<uint16_t> straightLengthsList;
    std::vector<uint32_t> straightLengthFrequencies;

    long long tree_compression_time = 0;
    long long old_compression_time = 0;
    long long rle_compression_time = 0;
    long long straights_compression_time;
    long long straights_huffman_compression_time = 0;
    long long reduced_edgebits_compression_time = 0;
    long long paths2bit_compression_time = 0;

    long long regionColorsBitStringTime = 0;

    double oldCompressionRate = 0;
    double newEdgeBitsCompressionRate = 0;
    double pathCompressionRate = 0;
    double paths2bit_compression_rate = 0;
    double rleCompressionRate = 0;
    double straightsCompressionRate = 0;
    double straightsHuffmanCompressionRate = 0;

    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();

    // start = std::chrono::high_resolution_clock::now();
    // regionColors = setRegions(img, neighborsOffsets, img.cols * img.rows);
    // end = std::chrono::high_resolution_clock::now();
    // std::cout << "UF set regions time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
    start = std::chrono::high_resolution_clock::now();
    regionColors = getRegionsFromImageSearch(img);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "dfs set regions time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;

    // std::cout << "region colors: " << std::endl;
    // for (const auto& color : regionColors) {
    //     std::cout << "R: " << static_cast<int>(color.red) 
    //               << ", G: " << static_cast<int>(color.green) 
    //               << ", B: " << static_cast<int>(color.blue) << std::endl;
    // }


    std::vector<RGB> differentialColors;
    differentialColors = dpcm(regionColors);
    // std::cout << "differential colors: " << std::endl;
    // for (const auto& color : differentialColors) {
    //     std::cout << "R: " << static_cast<int>(color.red) 
    //               << ", G: " << static_cast<int>(color.green) 
    //               << ", B: " << static_cast<int>(color.blue) << std::endl;
    // }
    std::vector<uint8_t> flat_differences = flatten_differences(differentialColors);
    std::map<uint8_t, int> frequencyMapDifferences = createFrequencyMap(flat_differences);
    // std::cout << "frequency map: " << std::endl;
    // for (const auto& pair : frequencyMapDifferences) {
    //     std::cout << "Value: " << static_cast<int>(pair.first) 
    //               << ", Frequency: " << pair.second << std::endl;
    // }
    auto [RGBHuffmanCodes, RGBroot] = buildRGBCodes(frequencyMapDifferences);
    // std::cout << "RGB Huffman Codes:" << std::endl;
    // for (const auto& pair : RGBHuffmanCodes) {
    //     std::cout << "Value: " << static_cast<int>(pair.first) 
    //               << ", Code: " << pair.second << std::endl;
    // }

    std::vector<bool> RGBDifferencesHuffmanBitString;

    // Encode the keys (first values of the map) with 8 bits each
    for (const auto& pair : frequencyMapDifferences) {
        std::vector<bool> keyBits = intToBool(pair.first, 8);
        RGBDifferencesHuffmanBitString.insert(RGBDifferencesHuffmanBitString.end(), keyBits.begin(), keyBits.end());
    }

    // Encode the frequencies with 32 bits each
    int frequencyBitsAmount = std::ceil(std::log2(img.cols * img.rows));
    std::vector<bool> frequencyBitsAmountVector = intToBool(frequencyBitsAmount, 5);
    RGBDifferencesHuffmanBitString.insert(RGBDifferencesHuffmanBitString.end(), frequencyBitsAmountVector.begin(), frequencyBitsAmountVector.end());
    for (const auto& pair : frequencyMapDifferences) {
        std::vector<bool> frequencyBits = intToBool(pair.second, frequencyBitsAmount);
        RGBDifferencesHuffmanBitString.insert(RGBDifferencesHuffmanBitString.end(), frequencyBits.begin(), frequencyBits.end());
    }

    // add huffman-encoded difference values to the bitstring
    int huffmanBitsAmount = std::ceil(std::log2(flat_differences.size()));
    std::vector<bool> huffmanBitsAmountVector = intToBool(huffmanBitsAmount, 5);
    RGBDifferencesHuffmanBitString.insert(RGBDifferencesHuffmanBitString.end(), huffmanBitsAmountVector.begin(), huffmanBitsAmountVector.end());
    for (const auto& diff : flat_differences) {
        std::string huffmanBits = RGBHuffmanCodes[diff];
        RGBDifferencesHuffmanBitString.insert(RGBDifferencesHuffmanBitString.end(), huffmanBits.begin(), huffmanBits.end());
    }

    std::cout << "frequency map bitstring size: " << RGBDifferencesHuffmanBitString.size() << std::endl;

    // // Output the size of the bitstring for verification
    // std::cout << "Frequency map bitstring size: " << RGBDifferencesHuffmanBitString.size() << " bits" << std::endl;

    // // Calculate bits needed to store all region colors with 8 bits per channel
    // int bitsPerChannel = 8;
    // int totalBitsRegionColors = regionColors.size() * 3 * bitsPerChannel; // 3 channels (R, G, B)
    // std::cout << "Bits needed to store all region colors with 8 bits per channel: " << totalBitsRegionColors << std::endl;

    // // Calculate bits needed to store differences Huffman encoded
    // int totalBitsHuffmanEncoded = 0;
    // for (const auto& diff : flat_differences) {
    //     totalBitsHuffmanEncoded += RGBHuffmanCodes[diff].size();
    // }
    // totalBitsHuffmanEncoded += RGBDifferencesHuffmanBitString.size(); // Add the size of the frequency map bitstring
    // std::cout << "Bits needed to store differences Huffman encoded: " << totalBitsHuffmanEncoded << std::endl;

    // // Compare the two
    // if (totalBitsHuffmanEncoded < totalBitsRegionColors) {
    //     std::cout << "Huffman encoding is more efficient by " 
    //               << (totalBitsRegionColors - totalBitsHuffmanEncoded) << " bits." << std::endl;
    // } else {
    //     std::cout << "Storing with 8 bits per channel is more efficient by " 
    //               << (totalBitsHuffmanEncoded - totalBitsRegionColors) << " bits." << std::endl;
    // }

    // std::cout << "size of regionColors: " << regionColors.size() << std::endl;
    std::set<RGB, RGBComparator> regionColorsSet;
    for (auto color : regionColors) {
        regionColorsSet.insert(color);
    }
    // std::cout << "Number of unique colors: " << regionColorsSet.size() << std::endl;
    auto regionColorBitString = boolVectorFromRGBVector(regionColors);
    std::cout << "size of regionColorBitString: " << regionColorBitString.size() << std::endl;
    end = std::chrono::high_resolution_clock::now();
    regionColorsBitStringTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();


    if(compressionMethods.useEdgebits){
        std::cout << "setting edgebits" << std::endl;
        start = std::chrono::high_resolution_clock::now();
        edgeBits01 = setEdgeBits(img, edgeBits01, neighborsOffsets);
        // set edgebits bitstring
        std::vector<bool> cols_bitstring = intToBool(img.cols, 16);
        std::vector<bool> rows_bitstring = intToBool(img.rows, 16);
        int regionColorsInt = regionColorBitString.size()/24;
        int regionColorBitsSize = std::ceil(std::log2(img.cols * img.rows));
        // std::cout << "region color bits size: " << regionColorBitsSize << std::endl;
        std::vector<bool> regionColorBits = intToBool(regionColorsInt, regionColorBitsSize);
        edgeBitsBitString.insert(edgeBitsBitString.end(), cols_bitstring.begin(), cols_bitstring.end());
        edgeBitsBitString.insert(edgeBitsBitString.end(), rows_bitstring.begin(), rows_bitstring.end());
        edgeBitsBitString.insert(edgeBitsBitString.end(), regionColorBits.begin(), regionColorBits.end());
        edgeBitsBitString.insert(edgeBitsBitString.end(), regionColorBitString.begin(), regionColorBitString.end());

        int edgeBitsAmount = edgeBits01.size();
        std::vector<bool> edgeBitsAmountVector = intToBool(edgeBitsAmount, 32);
        edgeBitsBitString.insert(edgeBitsBitString.end(), edgeBitsAmountVector.begin(), edgeBitsAmountVector.end());
        for(auto bit : edgeBits01){
            edgeBitsBitString.push_back(bit);
        }
        end = std::chrono::high_resolution_clock::now();
        old_compression_time += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        oldCompressionRate = (img.rows * img.cols * 24) / (double)(edgeBitsBitString.size());

    }   
    std::vector<bool> verticalBits;
    std::vector<bool> horizontalBits;
    std::vector<bool> reducedHoroizontalBits;
    std::vector<bool> reducedVerticalBits;
    
    //std::vector<bool> verticalBits = setVerticalBits(img);
    // double multicutPercentage = getMulticutPercentage(edgeBits01);
    // std::cout << "Percentage of edge bits set to 1: " << multicutPercentage << "%" << std::endl;

    if(compressionMethods.useReducedEdgebits){
        std::cout << "setting reduced edgebits" << std::endl;
        start = std::chrono::high_resolution_clock::now();
        horizontalBits = setHorizontalBits(img);
        reducedVerticalBits = reduceVerticalBits(img, edgeBits01);
        
        // set reduced edge bits bitstring
        std::vector<bool> cols_bitstring = intToBool(img.cols, 16);
        std::vector<bool> rows_bitstring = intToBool(img.rows, 16);
        reducedEdgeBitsBitString.insert(reducedEdgeBitsBitString.end(), cols_bitstring.begin(), cols_bitstring.end());
        reducedEdgeBitsBitString.insert(reducedEdgeBitsBitString.end(), rows_bitstring.begin(), rows_bitstring.end());
        int regionColorsInt = regionColorBitString.size()/24;
        int regionColorBitsSize = std::ceil(std::log2(img.cols * img.rows));
        // std::cout << "region color bits size: " << regionColorBitsSize << std::endl;
        std::vector<bool> regionColorBits = intToBool(regionColorsInt, regionColorBitsSize);
        reducedEdgeBitsBitString.insert(reducedEdgeBitsBitString.end(), regionColorBits.begin(), regionColorBits.end());
        reducedEdgeBitsBitString.insert(reducedEdgeBitsBitString.end(), regionColorBitString.begin(), regionColorBitString.end());
    
        int horizontalBitsAmount = horizontalBits.size();
        std::vector<bool> horizontalBitsAmountVector = intToBool(horizontalBitsAmount, 32);
        reducedEdgeBitsBitString.insert(reducedEdgeBitsBitString.end(), horizontalBitsAmountVector.begin(), horizontalBitsAmountVector.end());
        for(auto bit : horizontalBits){
            reducedEdgeBitsBitString.push_back(bit);
        }
        int reducedVerticalBitsAmount = reducedVerticalBits.size();
        std::vector<bool> reducedVerticalBitsAmountVector = intToBool(reducedVerticalBitsAmount, 32);
        reducedEdgeBitsBitString.insert(reducedEdgeBitsBitString.end(), reducedVerticalBitsAmountVector.begin(), reducedVerticalBitsAmountVector.end());
        for(auto bit : reducedVerticalBits){
            reducedEdgeBitsBitString.push_back(bit);
        }
        newEdgeBitsCompressionRate = (img.rows * img.cols * 24) / (double)(reducedEdgeBitsBitString.size());    
        end = std::chrono::high_resolution_clock::now();
        reduced_edgebits_compression_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }

    if(compressionMethods.useTree){
        std::cout << "setting paths" << std::endl;
        start = std::chrono::high_resolution_clock::now();
        paths = setPaths(edgeBits01, img);
        //set bitstring for paths 
        int totalDirectionBits = 0;
        for (const auto& path : paths) {
            totalDirectionBits += std::get<2>(path).size();
        }
        std::cout << "Total direction bits in paths: " << totalDirectionBits << std::endl;
        std::vector<bool> cols_bitstring = intToBool(img.cols, 16);
        std::vector<bool> rows_bitstring = intToBool(img.rows, 16);
        pathsBitString.insert(pathsBitString.end(), cols_bitstring.begin(), cols_bitstring.end());
        pathsBitString.insert(pathsBitString.end(), rows_bitstring.begin(), rows_bitstring.end());
        // std::cout << "region color bitstring size: " << regionColorBitString.size()/24 << std::endl;
        int regionColorsInt = regionColorBitString.size()/24;
        int regionColorBitsSize = std::ceil(std::log2(img.cols * img.rows));
        // std::cout << "region color bits size: " << regionColorBitsSize << std::endl;
        std::vector<bool> regionColorBits = intToBool(regionColorsInt, regionColorBitsSize);
        pathsBitString.insert(pathsBitString.end(), regionColorBits.begin(), regionColorBits.end());
        pathsBitString.insert(pathsBitString.end(), regionColorBitString.begin(), regionColorBitString.end());

        // hier statt region color bitstring huffman dpcm werte und frequency map bitstring inserten


        std::cout << "size after cols+rows+region color: " << pathsBitString.size() << std::endl;

        //int disconnectedComponentsBits = std::ceil(std::log2(img.cols * img.rows / 2));
        int disconnectedComponentsBits = std::max(static_cast<double>(std::ceil(std::log2(std::max(img.cols, img.rows)))), std::max(static_cast<double>(1), std::log2(std::ceil(static_cast<double>(img.cols)/2) * std::ceil(static_cast<double>(img.rows)/2))));
        std::cout << "paths size: " << paths.size() << std::endl;
        std::vector<bool> numberOfDisconnectedComponents = intToBool(paths.size(), disconnectedComponentsBits);
        // std::cout << "disconnected comp bits: " << disconnectedComponentsBits << std::endl;
        int startPointBits = std::ceil(std::log2(edgeBits01.size()));
        std::vector<bool> startPointBitsVector = intToBool(startPointBits, 5);
        // add number of components to bitstring
        pathsBitString.insert(pathsBitString.end(), numberOfDisconnectedComponents.begin(), numberOfDisconnectedComponents.end());
        // add number of bits for start points to bitstring
        pathsBitString.insert(pathsBitString.end(), startPointBitsVector.begin(), startPointBitsVector.end());
        for (auto& path : paths){
            auto& startEdge = std::get<0>(path);
            std::vector<bool> startBool = intToBool(startEdge,startPointBits);
            pathsBitString.insert(pathsBitString.end(), startBool.begin(), startBool.end());
            // std::cout << "direction size: " << std::get<2>(path).size() << std::endl;
            // std::cout << "startEdge: " << startEdge << ", path: " << std::endl;
            // int i = 0;
            // for (bool bit : std::get<2>(path)) {
            //     std::cout << bit;
            //     // if(++i % 3 == 0){
            //     //     std::cout << "-";
            //     // }
            // }
            // std::cout << std::endl;
        }
        for(auto& path : paths){
            // add direction vector to bitstring, delimited by 000
            pathsBitString.insert(pathsBitString.end(), std::get<2>(path).begin(), std::get<2>(path).end());
        }
        // std::cout << "paths bitstring: ";
        // int i = 0;
        // for(bool bit : pathsBitString){
        //     std::cout << bit;
        // }
        // std::cout << std::endl;


        end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "time to construct tree paths: " << duration.count() << "ms" << std::endl;
        tree_compression_time = duration.count();
        pathCompressionRate = (img.rows * img.cols * 24) / (double)pathsBitString.size();
        std::cout << "Path Compression Rate: " << pathCompressionRate << std::endl;
    }

    if(compressionMethods.use2bits){
        start = std::chrono::high_resolution_clock::now();
        auto _2bitpaths = set2BitPaths(edgeBits01, img);
        paths_2bit = std::get<0>(_2bitpaths);
        // for (const auto& path : paths_2bit) {
        //     uint32_t edgeI;
        //     Direction dir;
        //     std::vector<bool> directions2bits;

        //     std::tie(edgeI, dir, directions2bits) = path;

        //     std::cout << "Start Edge: " << edgeI << ", Start Direction: " << static_cast<int>(dir) << ", Path: ";
        //     for (bool bit : directions2bits) {
        //         std::cout << bit;
        //     }
        //     std::cout << std::endl;
        // }
        int totalDirectionBits = 0;
        for (const auto& path : paths_2bit) {
            totalDirectionBits += std::get<2>(path).size();
        }
        std::cout << "Total direction bits in 2-bit paths: " << totalDirectionBits << std::endl;
        std::cout << "paths_2bit size: " << paths_2bit.size() << std::endl;
        // edgeI,  zeros,          ones,           start
        // 32 bit, 1bit (vector),  16bit (vector), 1bit
        rle_paths = std::get<1>(_2bitpaths);
        std::vector<uint32_t> rleStartPoints;
        for (auto rle : rle_paths) {
            rleStartPoints.push_back(boolVectorToInt(std::get<0>(rle)));
            // std::cout << "start: " << boolVectorToInt(std::get<0>(rle)) << std::endl;
        }
            
        // set paths_2bit bitstring
        int paths2BitStringNoDirSize = 0;
        start = std::chrono::high_resolution_clock::now();
        std::vector<bool> cols_bitstring = intToBool(img.cols, 16);
        std::vector<bool> rows_bitstring = intToBool(img.rows, 16);
        int regionColorsInt = regionColorBitString.size()/24;
        int regionColorBitsSize = std::ceil(std::log2(img.cols * img.rows));
        std::vector<bool> regionColorBits = intToBool(regionColorsInt, regionColorBitsSize);
        paths2bitBitString.insert(paths2bitBitString.end(), cols_bitstring.begin(), cols_bitstring.end());
        paths2bitBitString.insert(paths2bitBitString.end(), rows_bitstring.begin(), rows_bitstring.end());
        paths2bitBitString.insert(paths2bitBitString.end(), regionColorBits.begin(), regionColorBits.end());
        paths2bitBitString.insert(paths2bitBitString.end(), regionColorBitString.begin(), regionColorBitString.end());
        std::cout << "size after cols+rows+region color: " << paths2bitBitString.size() << std::endl;
        int paths2bitAmount = paths_2bit.size();
        std::vector<bool> paths2bitAmountVector = intToBool(paths2bitAmount, 32);
        paths2bitBitString.insert(paths2bitBitString.end(), paths2bitAmountVector.begin(), paths2bitAmountVector.end());
        int paths2bitStartPointsBits = std::ceil(std::log2(edgeBits01.size()));
        std::vector<bool> paths2bitStartPointsBitsVector = intToBool(paths2bitStartPointsBits, 5);
        paths2bitBitString.insert(paths2bitBitString.end(), paths2bitStartPointsBitsVector.begin(), paths2bitStartPointsBitsVector.end());
        for (auto path : paths_2bit){
            std::vector<bool> startBool = intToBool(std::get<0>(path), paths2bitStartPointsBits);
            paths2bitBitString.insert(paths2bitBitString.end(), startBool.begin(), startBool.end());
        }
        paths2BitStringNoDirSize = paths2bitBitString.size();
        auto endNoDir = std::chrono::high_resolution_clock::now();
        auto durationNoDir = std::chrono::duration_cast<std::chrono::milliseconds>(endNoDir - start);
        for(auto path : paths_2bit){
            paths2bitBitString.insert(paths2bitBitString.end(), std::get<2>(path).begin(), std::get<2>(path).end());
            paths2bitBitString.push_back(false);
            paths2bitBitString.push_back(false);
            // paths2bitBitString.push_back(false);
        }
        // std::cout << "paths2bit bitstring: ";
        // for(bool bit : paths2bitBitString){
        //     std::cout << bit;
        // }
        // std::cout << std::endl;
        
        end = std::chrono::high_resolution_clock::now();
        paths2bit_compression_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        paths2bit_compression_rate = static_cast<double>(img.cols * img.rows * 24) / (paths2bitBitString.size());

        // set rle bitstring
        start = std::chrono::high_resolution_clock::now();
        for (auto& path : paths_2bit) {
            std::vector<bool>& directions = std::get<2>(path);

            size_t i = 0;
            while (i < directions.size()) {
                // Extract 2-bit direction
                bool bit1 = directions[i];
                bool bit2 = directions[i + 1];
                std::vector<bool> dir = {bit1, bit2};
                i += 2;

                // Count repetitions
                int count = 1;
                while (i + 1 < directions.size() && directions[i] == bit1 && directions[i + 1] == bit2) {
                    count++;
                    i += 2;
                }

                // Compute count bit size (store it explicitly)
                int countBitsSize = std::ceil(std::log2(count + 1));
                std::vector<bool> countBitsSizeVec = intToBool(countBitsSize, 5); // Always 5 bits

                // Store direction (2 bits)
                paths2bitRLEBitString.push_back(bit1);
                paths2bitRLEBitString.push_back(bit2);

                // Store count bit size (5 bits)
                paths2bitRLEBitString.insert(paths2bitRLEBitString.end(), countBitsSizeVec.begin(), countBitsSizeVec.end());

                // Store count (variable length)
                std::vector<bool> countBits = intToBool(count, countBitsSize);
                paths2bitRLEBitString.insert(paths2bitRLEBitString.end(), countBits.begin(), countBits.end());
            }

            // Append "00" as delimiter
            paths2bitRLEBitString.push_back(false);
            paths2bitRLEBitString.push_back(false);
        }

        rleCompressionRate = static_cast<double>(img.cols * img.rows * 24) / (paths2bitRLEBitString.size() + paths2BitStringNoDirSize);
        // std::cout << "new rle comp rate: " << rleCompressionRate << std::endl;
        // std::cout << "paths2bitRLEBitString size: " << paths2bitRLEBitString.size() << std::endl;
        end = std::chrono::high_resolution_clock::now();
        rle_compression_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        rle_compression_time += durationNoDir.count();
    }

    if(compressionMethods.useStraights){
        start = std::chrono::high_resolution_clock::now();
        std::tie(straights, straightsHuffmanCodesBitString, straightsHuffmanCodesStartPoints, root, straightLengthsList, straightLengthFrequencies) = setStraights(edgeBits01, img);
        end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "time to set straights: " << duration.count() << "ms" << std::endl;
        straights_huffman_compression_time = duration.count();
        straights_compression_time = duration.count();
    
    
    
        // set straights bitstring (no huffman)
        start = std::chrono::high_resolution_clock::now();
        // cols, rows, regionColors
        std::vector<bool> cols_bitstring = intToBool(img.cols, 16);
        std::vector<bool> rows_bitstring = intToBool(img.rows, 16);
        int regionColorsInt = regionColorBitString.size()/24;
        int regionColorBitsSize = std::ceil(std::log2(img.cols * img.rows));
        std::vector<bool> regionColorBits = intToBool(regionColorsInt, regionColorBitsSize);
        straightsNoHuffBitString.insert(straightsNoHuffBitString.end(), cols_bitstring.begin(), cols_bitstring.end());
        straightsNoHuffBitString.insert(straightsNoHuffBitString.end(), rows_bitstring.begin(), rows_bitstring.end());
        straightsNoHuffBitString.insert(straightsNoHuffBitString.end(), regionColorBits.begin(), regionColorBits.end());
        straightsNoHuffBitString.insert(straightsNoHuffBitString.end(), regionColorBitString.begin(), regionColorBitString.end());
    
        // list start points 
        std::vector<bool> huffmanStartPointsAmount = intToBool(straightsHuffmanCodesStartPoints.size(), 32);
        straightsNoHuffBitString.insert(straightsNoHuffBitString.end(), huffmanStartPointsAmount.begin(), huffmanStartPointsAmount.end());
        int huffmanStartPointsBits = std::ceil(std::log2(edgeBits01.size()));
        std::vector<bool> huffmanStartPointsBitsVector = intToBool(huffmanStartPointsBits, 5);
        straightsNoHuffBitString.insert(straightsNoHuffBitString.end(), huffmanStartPointsBitsVector.begin(), huffmanStartPointsBitsVector.end());
        for(auto& startPoint : straightsHuffmanCodesStartPoints){
            std::vector<bool> startPointBits = intToBool(startPoint, huffmanStartPointsBits);
            straightsNoHuffBitString.insert(straightsNoHuffBitString.end(), startPointBits.begin(), startPointBits.end());
        }
    
        // list lengths
        int straightBits = std::ceil(std::log2(std::max(img.cols, img.rows)));
        std::vector<bool> straightBitsVector = intToBool(straightBits, 5);
        straightsNoHuffBitString.insert(straightsNoHuffBitString.end(), straightBitsVector.begin(), straightBitsVector.end());
        for(auto straight : straights){
            auto& count = std::get<1>(straight);
            std::vector<bool> countBits = intToBool(boolVectorToInt(count), straightBits);
            straightsNoHuffBitString.insert(straightsNoHuffBitString.end(), countBits.begin(), countBits.end());
        }
        std::cout << "straights (no huff) comp rate: " << (img.rows * img.cols * 24) / (double)straightsNoHuffBitString.size() << std::endl;
        end = std::chrono::high_resolution_clock::now();
        straights_compression_time += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        straightsCompressionRate = (img.rows * img.cols * 24) / (double)straightsNoHuffBitString.size();
    
        // ---------------
        //set bitstring for straights (huffman)
    
        start = std::chrono::high_resolution_clock::now();
        // cols, rows, regionColors
        straightsBitString.insert(straightsBitString.end(), cols_bitstring.begin(), cols_bitstring.end());
        straightsBitString.insert(straightsBitString.end(), rows_bitstring.begin(), rows_bitstring.end());
        straightsBitString.insert(straightsBitString.end(), regionColorBits.begin(), regionColorBits.end());
        straightsBitString.insert(straightsBitString.end(), regionColorBitString.begin(), regionColorBitString.end());
    
        // list start points 
        huffmanStartPointsAmount = intToBool(straightsHuffmanCodesStartPoints.size(), 32);
        straightsBitString.insert(straightsBitString.end(), huffmanStartPointsAmount.begin(), huffmanStartPointsAmount.end());
        huffmanStartPointsBits = std::ceil(std::log2(edgeBits01.size()));
        huffmanStartPointsBitsVector = intToBool(huffmanStartPointsBits, 5);
        straightsBitString.insert(straightsBitString.end(), huffmanStartPointsBitsVector.begin(), huffmanStartPointsBitsVector.end());
        for(auto& startPoint : straightsHuffmanCodesStartPoints){
            std::vector<bool> startPointBits = intToBool(startPoint, huffmanStartPointsBits);
            straightsBitString.insert(straightsBitString.end(), startPointBits.begin(), startPointBits.end());
        }
    
        // length of huffman bitstring + huffman bitstring
        std::vector<bool> huffmanCodesBitstringSize = intToBool(straightsHuffmanCodesBitString.size(), 64);
        straightsBitString.insert(straightsBitString.end(), huffmanCodesBitstringSize.begin(), huffmanCodesBitstringSize.end());
        straightsBitString.insert(straightsBitString.end(), straightsHuffmanCodesBitString.begin(), straightsHuffmanCodesBitString.end());
        
        // lengths
        int straightsLenghtsBits = std::ceil(std::log2(std::max(img.cols, img.rows)));
        std::vector<bool> straightsLengthsBitsVector = intToBool(straightsLenghtsBits, 5);
        straightsBitString.insert(straightsBitString.end(), straightsLengthsBitsVector.begin(), straightsLengthsBitsVector.end());
        std::vector<bool> straightLengthsListSize = intToBool(straightLengthsList.size(), 16);
        straightsBitString.insert(straightsBitString.end(), straightLengthsListSize.begin(), straightLengthsListSize.end());
        for(auto& length : straightLengthsList){
            std::vector<bool> lengthBits = intToBool(length, straightsLenghtsBits);
            straightsBitString.insert(straightsBitString.end(), lengthBits.begin(), lengthBits.end());
        }
    
        // frequencies
        std::vector<bool> straightLengthFrequenciesSize = intToBool(straightLengthFrequencies.size(), 16);
        straightsBitString.insert(straightsBitString.end(), straightLengthFrequenciesSize.begin(), straightLengthFrequenciesSize.end());
        int straightsFrequenciesBits = std::ceil(std::log2(img.cols * img.rows));
        std::vector<bool> straightLengthFrequenciesBits = intToBool(straightsFrequenciesBits, 5);
        straightsBitString.insert(straightsBitString.end(), straightLengthFrequenciesBits.begin(), straightLengthFrequenciesBits.end());
        for(auto& frequency : straightLengthFrequencies){
            std::vector<bool> frequencyBits = intToBool(frequency, straightsFrequenciesBits);
            straightsBitString.insert(straightsBitString.end(), frequencyBits.begin(), frequencyBits.end());
        }
        end = std::chrono::high_resolution_clock::now();
        straights_huffman_compression_time += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        straightsHuffmanCompressionRate = (img.rows * img.cols * 24) / (double)straightsBitString.size();
    }
    
    tree_compression_time += regionColorsBitStringTime;
    old_compression_time += regionColorsBitStringTime;
    rle_compression_time += regionColorsBitStringTime;
    straights_compression_time += regionColorsBitStringTime;
    straights_huffman_compression_time += regionColorsBitStringTime;
    reduced_edgebits_compression_time += regionColorsBitStringTime;
    paths2bit_compression_time += regionColorsBitStringTime;


    return {compressionMethods,
        regionColors, edgeBits01, edgeBitsBitString, paths, pathsBitString, img, paths_2bit, paths2bitBitString, paths2bitRLEBitString, rle_paths, 
        // rleBitString,
        straights, straightsNoHuffBitString,
    regionColorBitString, straightsHuffmanCodesBitString, 
    straightsHuffmanCodesStartPoints, root, straightLengthsList, straightLengthFrequencies, straightsBitString,
    horizontalBits, reducedVerticalBits, reducedEdgeBitsBitString,
    pathCompressionRate, rleCompressionRate ,oldCompressionRate,straightsCompressionRate,straightsHuffmanCompressionRate, newEdgeBitsCompressionRate, paths2bit_compression_rate,
    transparencyValues,
    tree_compression_time, old_compression_time, rle_compression_time, straights_compression_time, straights_huffman_compression_time, reduced_edgebits_compression_time, paths2bit_compression_time};
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

std::vector<bool> setVerticalBits(cv::Mat img){
    std::vector<bool> verticalBits;
    int vertices = img.cols * img.rows;
    RGB previousColor = getVertexColor(0, vertices, img);
    RGB currentColor;
    for(int v = 1; v < vertices; v++){
        if(v % img.cols == 0){
            previousColor = getVertexColor(v, vertices, img);
            continue;
        }
        currentColor = getVertexColor(v, vertices, img);
        verticalBits.push_back(!compareRGB(previousColor, currentColor));
        previousColor = currentColor;
    }
    return verticalBits;
}

std::vector<bool> reduceVerticalBits(cv::Mat img, std::vector<bool>& edgeBits01){
    std::vector<bool> reducedVerticalBits;
    int cols = img.cols;
    int rows = img.rows;
    int currentCol = 0;
    int currentEdge = 1;
    int previousEdge;
    reducedVerticalBits.push_back(edgeBits01[1]);
    while(true){
        if(currentEdge == edgeBits01.size()-1 || currentEdge >= edgeBits01.size()){
            break;
        }
        if(currentEdge > edgeBits01.size() - cols){
            currentCol+=2;
            currentEdge = currentCol+1;
            // if(edgeBits01[currentEdge] != reducedVerticalBits.back()){
            //     reducedVerticalBits.push_back(edgeBits01[currentEdge]);
            // }
            reducedVerticalBits.push_back(edgeBits01[currentEdge]);
        }
        bool left = edgeBits01[getNeighbor(currentEdge, Direction::DOWN, 0, cols, rows)];
        bool right = edgeBits01[getNeighbor(currentEdge, Direction::DOWN, 2, cols, rows)];
        bool front = edgeBits01[getNeighbor(currentEdge, Direction::DOWN, 1, cols, rows)];
        if(left || right){
            reducedVerticalBits.push_back(front);
        }
        currentEdge = getNeighbor(currentEdge, Direction::DOWN, 1, cols, rows);
        //TODO: evtl. bug durch return werte von getNeighbor (gibt auch manchmal -5 zurück o.ä.)
        

    }

    return reducedVerticalBits;
}

std::vector<bool> setHorizontalBits(cv::Mat img){
    std::vector<bool> horizontalBits;
    int vertices = img.cols * img.rows;
    RGB previousColor = getVertexColor(0, vertices, img, true);
    RGB currentColor;
    for(int v = 1; v < vertices; v++){
        if(v % img.rows == 0){
            previousColor = getVertexColor(v, vertices, img, true);
            // horizontalBits.push_back(true);
            continue;
        }
        currentColor = getVertexColor(v, vertices, img, true);
        horizontalBits.push_back(!compareRGB(previousColor, currentColor));
        previousColor = currentColor;
    }
    return horizontalBits;
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

// DFS to get color region indices and their colors
std::vector<RGB> getRegionsFromImageSearch(cv::Mat img) {
    int rows = img.rows;
    int cols = img.cols;
    int vertices = rows * cols;

    std::vector<bool> visited(vertices, false);
    std::vector<int> regionIndices;  // Stores starting indices of regions
    std::vector<RGB> regionColors;   // Stores corresponding region colors

    for (int index = 0; index < vertices; index++) {
        if (visited[index]) continue;  // Skip already visited pixels

        // Start DFS from this new region
        std::stack<int> stack;
        stack.push(index);
        visited[index] = true;
        regionIndices.push_back(index);  // Store the new region start index

        RGB regionColor = getVertexColor(index, vertices, img);
        regionColors.push_back(regionColor);  // Store the region color

        while (!stack.empty()) {
            int current = stack.top();
            stack.pop();

            int col = current % cols;
            int row = current / cols;

            // Check right neighbor
            int right = current + 1;
            if (col < cols - 1 && !visited[right] && compareRGB(regionColor, getVertexColor(right, vertices, img))) {
                stack.push(right);
                visited[right] = true;
            }

            // Check below neighbor
            int below = current + cols;
            if (row < rows - 1 && !visited[below] && compareRGB(regionColor, getVertexColor(below, vertices, img))) {
                stack.push(below);
                visited[below] = true;
            }

            // Check left neighbor
            int left = current - 1;
            if (col > 0 && !visited[left] && compareRGB(regionColor, getVertexColor(left, vertices, img))) {
                stack.push(left);
                visited[left] = true;
            }

            // Check above neighbor
            int above = current - cols;
            if (row > 0 && !visited[above] && compareRGB(regionColor, getVertexColor(above, vertices, img))) {
                stack.push(above);
                visited[above] = true;
            }
        }
    }

    return regionColors;  
}



PathInfoVector setPaths(std::vector<bool> edgeBits01, cv::Mat img){
    int edgeI = 0;
    int dfsI = 0;
    std::vector<bool> visited(edgeBits01.size(), false);
    PathInfoVector paths;
    int threeBitCount = 0;
    
    for (bool edge : edgeBits01){
        // skip non-multicut edges or previously visited edges ||
        if(!edge){
            //visited[edgeI] = true;
            edgeI++;
            continue;
        }
        if(visited[edgeI]){
            edgeI++;
            continue;
        }
        //std::cout << std::endl << edgeI;
        //visited[edgeI] = true;

        // dir rausfinden
        // get current direction (either down or right) from current index 
        Direction currentDir = getDirectionFromIndex(edgeI, img.rows, img.cols);

        dfsI++;

        std::vector<bool> directionVector;
        //directionVector = dfs_paths_recursive(edgeI, visited, currentDir, directionVector);
        directionVector = dfs_paths_iterative(edgeI, currentDir, visited, img, edgeBits01, threeBitCount);
        // directionVector.push_back(false);
        // directionVector.push_back(false);
        // directionVector.push_back(false);
        paths.emplace_back(edgeI, currentDir, directionVector);
        //std::cout << "path size: " << directionVector.size() << std::endl;

        edgeI++;
    }

    std::cout << "-->>>for 3 bit directions: " << threeBitCount << std::endl;
    //std::cout << "edgeI: " << edgeI << std::endl;
    //std::cout << "number of edges: " << 2*rows*cols - cols - rows << std::endl;

    // print paths
    // for (auto path : paths) {
    //     std::vector<bool> edgeI;
    //     Direction dir;
    //     std::vector<bool> directionVector;
    //     std::tie(edgeI, dir, directionVector) = path;
    //     std::cout << "Start Edge: " << boolVectorToInt(edgeI) << ", Direction: " << static_cast<int>(dir) << ", Path: ";
    //     for (bool bit : directionVector) {
    //         std::cout << bit;
    //     }
    //     std::cout << std::endl;
    // }

    // Calculate storage space for the path vector
    double totalBitsPathVector = 0;
    for (const auto& path : paths) {
        uint32_t edgeI;
        Direction dir;
        std::vector<bool> directionVector;

        std::tie(edgeI, dir, directionVector) = path;
        totalBitsPathVector += 32;
        
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
        paths_2bit.emplace_back(edgeI, startDir, directions2bits);
    }

    // Calculate storage space for the paths vector
    double totalBits2BitPaths = 0;
    int emptyPathsCount = 0;
    for (const auto& path : paths_2bit) {
        uint32_t edgeI;
        Direction dir;
        std::vector<bool> directions2bits;

        std::tie(edgeI, dir, directions2bits) = path;

        totalBits2BitPaths += 32;
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
        uint32_t edgeI;
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
        uint32_t edgeI;
        Direction dir;
        std::vector<bool> directions2bits;

        std::tie(edgeI, dir, directions2bits) = path;

        totalBitsRLE += 32;
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
        uint32_t edgeI;
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
 * @return percentage of edges connecting two different-colored pixels 
 */
double getMulticutPercentage(std::vector<bool> edgeBits01){
    int count = std::count(edgeBits01.begin(), edgeBits01.end(), true);
    // std::cout << "0 in edgebits: " << edgeBits01.size() - count << std::endl;
    // std::cout << "1 in edgebits: " << count << std::endl;
    return 100*count / edgeBits01.size();
}


void getAnomalies(std::vector<bool> edgeBits01, cv::Mat img){
    int count = 0;
    for (int col = 1; col <= 2 * img.cols - 3; col += 2){
        int current = col;
        while (getNeighbor(current, Direction::DOWN, 1, img.cols, img.rows) != -1){
            bool left = edgeBits01[getNeighbor(current, Direction::DOWN, 0, img.cols, img.rows)];
            bool right = edgeBits01[getNeighbor(current, Direction::DOWN, 2, img.cols, img.rows)];
            bool front = edgeBits01[getNeighbor(current, Direction::DOWN, 1, img.cols, img.rows)];
            bool currentEdge = edgeBits01[current];
            if((left || right) && (front == currentEdge)){
                // std::cout << "Anomaly found at edge " << current << std::endl;
                count++;
            }
            current = getNeighbor(current, Direction::DOWN, 1, img.cols, img.rows);
        }
    }
    std::cout << "Number of anomalies: " << count << std::endl;
}

std::vector<RGB> dpcm(std::vector<RGB> colors){
    std::vector<RGB> dpcmColors;
    dpcmColors.push_back(colors[0]);
    for(int i = 1; i < colors.size(); i++){
        RGB currentColor = colors[i];
        RGB previousColor = colors[i-1];
        RGB dpcmColor;
        dpcmColor.red = currentColor.red - previousColor.red;
        dpcmColor.green = currentColor.green - previousColor.green;
        dpcmColor.blue = currentColor.blue - previousColor.blue;
        dpcmColors.push_back(dpcmColor);
    }
    return dpcmColors;
}

std::vector<uint8_t> flatten_differences(std::vector<RGB> differences){
    std::vector<uint8_t> flatDifferences;
    for (const auto& color : differences) {
        flatDifferences.push_back(color.red);
        flatDifferences.push_back(color.green);
        flatDifferences.push_back(color.blue);
    }
    return flatDifferences;
}

// create frequency map for differences 
std::map<uint8_t, int> createFrequencyMap(const std::vector<uint8_t>& differences) {
    std::map<uint8_t, int> frequencyMap;
    for (const auto& diff : differences) {
        frequencyMap[diff]++;
    }
    return frequencyMap;
}
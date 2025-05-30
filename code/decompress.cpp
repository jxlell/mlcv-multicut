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
extern "C" {
    #include "zlib.h"
  }
  

decompInfo reconstructImage(CompressedImage compImg, bool showImg){
    methods compressionMethods = compImg.compressionMethods;
    cv::Mat originalImg = compImg.originalImage;
    std::vector<bool> edgeBits01 = compImg.edgeBits01;
    std::vector<bool> edgeBitsBitString = compImg.edgeBitsBitString;
    std::vector<RGB> regionColors = compImg.regionColors;
    PathInfoVector paths = compImg.paths;
    std::vector<bool> pathsBitString = compImg.pathsBitString;
    PathInfoVector paths_2bit_nonRLE = compImg.pathInfoVector2bit;
    std::vector<bool> paths2bitBitString = compImg.paths2bitBitString;
    std::vector<bool> paths2bitRLEBitString = compImg.paths2bitRLEBitString;
    RLEVector rle_paths = compImg.rleVector;
    // std::vector<bool> rleBitString = compImg.rleBitString;
    Straights straights = compImg.straights;
    std::vector<bool> straightsNoHuffBitString = compImg.straightsNoHuffBitString;
    std::vector<bool> regionColorBitString = compImg.regionColorBitString;
    int rows = originalImg.rows;
    int cols = originalImg.cols;

    long long tree_decompression_time = 0;
    long long old_decompression_time = 0;
    long long rle_decompression_time = 0;
    long long straights_huffman_decompression_time = 0;
    long long reduced_edgebits_decompression_time = 0;
    long long paths_2bits_decompression_time = 0;
    long long straights_decompression_time = 0;

    double rebuild_dpcm_huffman_time = 0;
    double decode_colors_time = 0;
    double assemble_tree_paths_time = 0;
    double reconstruct_tree_edgebits_time = 0;
    double dfs_reconstruction_time = 0;
    // long long UF_reconstruction_time = 0;
    double reconstruct_rcmv_time = 0;
    double reconstruct_rcmv_cmv_time = 0;
    double dec_reconstruction_time = 0;
    double dec_cmv_reconstruction_time = 0;
    double sls_reconstruction_time = 0;
    double sls_cmv_reconstruction_time = 0;

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

    HuffmanNode* reconstructedRoot;  
    std::vector<bool> reconstructed_edgeBits_from_paths(edgeBitsSize, false);
    std::vector<bool> edgeBitsFromBitString(edgeBitsSize, false);
    std::vector<bool> reconstructed_edgeBits_horizontals(edgeBitsSize, false);
    std::vector<bool> reconstructed_edgeBits_2bits(edgeBitsSize, false);
    std::vector<bool> reconstructed_edgeBits_straights(edgeBitsSize, false);
    std::vector<bool> reconstructed_edgeBits_straights_no_huff(edgeBitsSize, false);
    std::vector<bool> reconstructed_edgeBits_2bits_original(edgeBitsSize, false);
    std::vector<bool> regionColorsBitStringFromEdgeBits;
    std::vector<bool> regionColorsBitStringFromReducedEdgeBits;
    std::vector<bool> regionColorsBitStringFromTree;
    std::vector<bool> regionColorsBitStringFrom2BitPaths;
    std::vector<bool> regionColorsBitStringFromRLE;
    std::vector<bool> regionColorsBitStringFromStraights;
    std::vector<bool> regionColorsBitStringFromStraightsHuffman;


    int cols_int = 0;
    int rows_int = 0;
    std::string rows_str;
    std::string cols_str;
    int regionColorBitsSize = 0;
    int regionColorBitsSizeInt = 0;
    std::string regionColorBitsSizeStr;
    std::string regionColorBitStringStr;
    std::vector<bool> regionColorBitStringParsed;
    std::vector<RGB> decodedColorsTree;
    std::vector<RGB> inflatedRegionColorsVec;
    std::vector<RGB> decodedColorsTree_inflated;

    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();


    if (compressionMethods.useTree) {
    start = std::chrono::high_resolution_clock::now();

    std::string pathsBitStringStr;
    for (bool bit : pathsBitString) {
        pathsBitStringStr += bit ? '1' : '0';
    }

    size_t offset = 0;

    // 1. Extract image dimensions
    std::string cols_str = pathsBitStringStr.substr(offset, 16);
    int cols_int = std::stoi(cols_str, nullptr, 2);
    offset += 16;

    std::string rows_str = pathsBitStringStr.substr(offset, 16);
    int rows_int = std::stoi(rows_str, nullptr, 2);
    offset += 16;

    // 2. Extract and inflate region colors
    std::string deflatedBitsAmount = pathsBitStringStr.substr(offset, 32);
    int deflatedBitsAmountInt = std::stoi(deflatedBitsAmount, nullptr, 2);
    std::cout << "Deflated color bytes: " << deflatedBitsAmountInt << std::endl;
    offset += 32;

    std::vector<uint8_t> deflatedColorData(deflatedBitsAmountInt);
    for (int i = 0; i < deflatedBitsAmountInt; ++i) {
        std::string byteStr = pathsBitStringStr.substr(offset + i * 8, 8);
        deflatedColorData[i] = static_cast<uint8_t>(std::stoi(byteStr, nullptr, 2));
    }
    offset += deflatedBitsAmountInt * 8;

    std::vector<uint8_t> inflatedColorBytes;
    bool inflateSuccess = ZlibInflate(deflatedColorData, inflatedColorBytes);
    std::cout << "Zlib color success: " << (inflateSuccess ? "YES" : "NO") << std::endl;

    std::vector<bool> inflatedColorBitstring;
    for (uint8_t byte : inflatedColorBytes) {
        for (int i = 7; i >= 0; --i) {
            inflatedColorBitstring.push_back((byte >> i) & 1);
        }
    }
    decodedColorsTree_inflated = decodeDifferences(inflatedColorBytes);
    
    decode_colors_time = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start).count();

    // 3. Inflate compressed directional path bitstring
    start = std::chrono::high_resolution_clock::now();

    std::string compressedDTSizeStr = pathsBitStringStr.substr(offset, 32);
    int compressedDTSizeBytes = std::stoi(compressedDTSizeStr, nullptr, 2);
    offset += 32;

    std::vector<uint8_t> deflatedDTData(compressedDTSizeBytes);
    for (int i = 0; i < compressedDTSizeBytes; ++i) {
        std::string byteStr = pathsBitStringStr.substr(offset + i * 8, 8);
        deflatedDTData[i] = static_cast<uint8_t>(std::stoi(byteStr, nullptr, 2));
    }
    offset += compressedDTSizeBytes * 8;

    std::vector<uint8_t> inflatedDTBytes;
    bool inflateSuccessDT = ZlibInflate(deflatedDTData, inflatedDTBytes);
    std::cout << "Zlib DT success: " << (inflateSuccessDT ? "YES" : "NO") << std::endl;

    std::vector<bool> DT_bitstring_inflated;
    for (uint8_t byte : inflatedDTBytes) {
        for (int i = 7; i >= 0; --i) {
            DT_bitstring_inflated.push_back((byte >> i) & 1);
        }
    }

    // 4. Extract metadata from DT_bitstring_inflated
    size_t dt_offset = 0;

    int disconnectedComponentsBits = std::max(
        static_cast<double>(std::ceil(std::log2(std::max(cols_int, rows_int) + 1))),
        std::max(static_cast<double>(1),
                 std::log2(std::ceil(static_cast<double>(cols_int) / 2) *
                           std::ceil(static_cast<double>(rows_int) / 2) + 1))
    );

    std::string numberOfComponentsStr = "";
    for (int i = 0; i < disconnectedComponentsBits; ++i)
        numberOfComponentsStr += DT_bitstring_inflated[dt_offset++] ? '1' : '0';
    int numberOfComponents = std::stoi(numberOfComponentsStr, nullptr, 2);

    std::string startPointBitsStr = "";
    for (int i = 0; i < 5; ++i)
        startPointBitsStr += DT_bitstring_inflated[dt_offset++] ? '1' : '0';
    int startPointBits = std::stoi(startPointBitsStr, nullptr, 2);

    std::vector<uint32_t> startPoints;
    for (int i = 0; i < numberOfComponents; ++i) {
        std::string currentStr = "";
        for (int b = 0; b < startPointBits; ++b)
            currentStr += DT_bitstring_inflated[dt_offset++] ? '1' : '0';
        uint32_t startPoint = std::stoi(currentStr, nullptr, 2);
        startPoints.push_back(startPoint);
    }

    // 5. Remaining bits are directions
    std::queue<bool> directionQueue;
    while (dt_offset < DT_bitstring_inflated.size()) {
        directionQueue.push(DT_bitstring_inflated[dt_offset++]);
    }

    assemble_tree_paths_time = std::chrono::duration<double, std::milli>(
        std::chrono::high_resolution_clock::now() - start).count();
    tree_decompression_time += assemble_tree_paths_time;

    // 6. Reconstruct edgebits
    start = std::chrono::high_resolution_clock::now();

    reconstructed_edgeBits_from_paths.assign(edgeBitsSize, false);
    std::vector<bool> visited(edgeBitsSize, false);

    for (size_t i = 0; i < startPoints.size(); ++i) {
        reconstruct_edgeBits_iterative(
            startPoints[i],
            getDirectionFromIndex(startPoints[i], rows_int, cols_int),
            reconstructed_edgeBits_from_paths,
            cols_int,
            rows_int,
            visited,
            directionQueue
        );
    }

    reconstruct_tree_edgebits_time = std::chrono::duration<double, std::milli>(
        std::chrono::high_resolution_clock::now() - start).count();
}

    
    
    size_t bitIndex = 0;

    if(compressionMethods.useReducedEdgebits){
        // Parse reduced edge bits bitstring
        start = std::chrono::high_resolution_clock::now();
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
        
        // std::cout << "rows: " << rows_int << std::endl;
        
        // Compute region color bits size
        regionColorBitsSize = std::ceil(std::log2(cols_int * rows_int + 1));
        regionColorBitsSizeInt = std::stoi(reducedEdgeBitsBitStringStr.substr(bitIndex, regionColorBitsSize), nullptr, 2);
        bitIndex += regionColorBitsSize;
        
        // std::cout << "regionColorBitsSize: " << regionColorBitsSizeInt << std::endl;
        
        // Parse region color bits
        regionColorsBitStringFromReducedEdgeBits.reserve(regionColorBitsSizeInt * 24);
        for (size_t i = 0; i < regionColorBitsSizeInt * 24; ++i) {
            regionColorsBitStringFromReducedEdgeBits.push_back(reducedEdgeBitsBitStringStr[bitIndex++] == '1');
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
        end = std::chrono::high_resolution_clock::now();
        reconstruct_rcmv_time = std::chrono::duration<double, std::milli>(end - start).count();
        start = std::chrono::high_resolution_clock::now();
        
        //reconstruct edgebits from horizontals/verticals
        reconstructed_edgeBits_horizontals = reconstruct_edgeBits_from_Horizontals(horizontalBits, reducedVerticalBits, cols, rows);
        end = std::chrono::high_resolution_clock::now();
        reconstruct_rcmv_cmv_time = std::chrono::duration<double, std::milli>(end - start).count();
        reduced_edgebits_decompression_time += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    }


    if(compressionMethods.useEdgebits){
        // Parse edgebits bitstring
        // std::cout << "parse edgebits bitstring" << std::endl;

        start = std::chrono::high_resolution_clock::now();
        bitIndex = 0;

        // Convert vector<bool> to string efficiently
        std::string edgeBitsBitStringStr;
        edgeBitsBitStringStr.reserve(edgeBits01.size());
        for (bool bit : edgeBitsBitString) {
            edgeBitsBitStringStr += bit ? '1' : '0';
        }

        // Extract cols and rows
        cols_str = edgeBitsBitStringStr.substr(bitIndex, 16);
        cols_int = std::stoi(cols_str, nullptr, 2);
        bitIndex += 16;

        rows_str = edgeBitsBitStringStr.substr(bitIndex, 16);
        rows_int = std::stoi(rows_str, nullptr, 2);
        bitIndex += 16;

        // std::cout << "cols: " << cols_int << std::endl;
        // std::cout << "rows: " << rows_int << std::endl;

        // Compute region color bits size
        regionColorBitsSize = std::ceil(std::log2(cols_int * rows_int + 1));
        regionColorBitsSizeStr = edgeBitsBitStringStr.substr(bitIndex, regionColorBitsSize);
        regionColorBitsSizeInt = std::stoi(regionColorBitsSizeStr, nullptr, 2);
        bitIndex += regionColorBitsSize;

        // std::cout << "regionColorBitsSize: " << regionColorBitsSizeInt << std::endl;

        // Parse region color bit string
        regionColorBitStringStr = edgeBitsBitStringStr.substr(bitIndex, regionColorBitsSizeInt * 24);
        regionColorsBitStringFromEdgeBits.reserve(regionColorBitsSizeInt * 24);
        for (char c : regionColorBitStringStr) {
            regionColorsBitStringFromEdgeBits.push_back(c == '1');
        }
        bitIndex += regionColorBitStringStr.size();

        // Parse edge bits amount
        std::string edgeBitsAmountStr = edgeBitsBitStringStr.substr(bitIndex, 32);
        int edgeBitsAmount = std::stoi(edgeBitsAmountStr, nullptr, 2);
        bitIndex += 32;

        // Parse edge bits
        for (size_t i = 0; i < edgeBitsAmount; i++) {
            edgeBitsFromBitString[i] = edgeBitsBitStringStr[bitIndex++] == '1';
        }
        end = std::chrono::high_resolution_clock::now();
        old_decompression_time += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    }
    

    if(compressionMethods.use2bits){
        //reconstruct 2bitpaths from paths_2bit_bitstring
        // std::cout << "parse 2bitpaths bitstring" << std::endl;
        start = std::chrono::high_resolution_clock::now();
        PathInfoVector paths_2bit_from_bitstring;
        std::string paths2bitBitStringStr;
        for (bool bit : paths2bitBitString) {
            paths2bitBitStringStr += bit ? "1" : "0";
        }
        bitIndex = 0;

        // Extract cols
        cols_str = paths2bitBitStringStr.substr(bitIndex, 16);
        cols_int = std::stoi(cols_str, nullptr, 2);
        bitIndex += 16;
        // std::cout << "cols: " << cols_int << std::endl;

        // Extract rows
        rows_str = paths2bitBitStringStr.substr(bitIndex, 16);
        rows_int = std::stoi(rows_str, nullptr, 2);
        bitIndex += 16;
        // std::cout << "rows: " << rows_int << std::endl;

        // Compute region color bits size
        regionColorBitsSize = std::ceil(std::log2(cols_int * rows_int + 1));
        regionColorBitsSizeStr = paths2bitBitStringStr.substr(bitIndex, regionColorBitsSize);
        regionColorBitsSizeInt = std::stoi(regionColorBitsSizeStr, nullptr, 2);
        bitIndex += regionColorBitsSize;

        // std::cout << "regionColorBitsSize: " << regionColorBitsSizeInt << std::endl;

        // Parse region color bit string
        regionColorBitStringStr = paths2bitBitStringStr.substr(bitIndex, regionColorBitsSizeInt * 24);
        regionColorsBitStringFrom2BitPaths.reserve(regionColorBitsSizeInt * 24);
        for (char c : regionColorBitStringStr) {
            regionColorsBitStringFrom2BitPaths.push_back(c == '1');
        }
        bitIndex += regionColorBitStringStr.size();

        // Extract paths2bitAmount (number of paths)
        std::string paths2bitAmountStr = paths2bitBitStringStr.substr(bitIndex, 32);
        int paths2bitAmount = std::stoi(paths2bitAmountStr, nullptr, 2);
        bitIndex += 32;
        // std::cout << "paths2bitAmount: " << paths2bitAmount << std::endl;

        // Extract paths2bitStartPointsBits (how many bits each start point takes)
        std::string paths2bitStartPointsBitsStr = paths2bitBitStringStr.substr(bitIndex, 5);
        int paths2bitStartPointsBits = std::stoi(paths2bitStartPointsBitsStr, nullptr, 2);
        bitIndex += 5;
        // std::cout << "paths2bitStartPointsBits: " << paths2bitStartPointsBits << std::endl;

        std::vector<int> startPoints2Bits;
        for (int i = 0; i < paths2bitAmount; i++) {
            std::string startPointStr = paths2bitBitStringStr.substr(bitIndex, paths2bitStartPointsBits);
            int startPoint = std::stoi(startPointStr, nullptr, 2);
            startPoints2Bits.push_back(startPoint);
            bitIndex += paths2bitStartPointsBits;
        }


        // Parse paths (2-bit directions until "00" delimiter)
        for (int i = 0; i < paths2bitAmount; i++) {
            std::vector<bool> pathDirections;
            
            while (bitIndex + 2 <= paths2bitBitStringStr.size()) {
                std::string dir = paths2bitBitStringStr.substr(bitIndex, 2);
                bitIndex += 2;

                if (dir == "00") break; // Stop at delimiter

                pathDirections.push_back(dir[0] == '1');
                pathDirections.push_back(dir[1] == '1');
            }

            // Add parsed data to vector
            paths_2bit_from_bitstring.emplace_back(startPoints2Bits[i], getDirectionFromIndex(startPoints2Bits[i], rows_int, cols_int), pathDirections);
        }
        end = std::chrono::high_resolution_clock::now();
        paths_2bits_decompression_time += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        dec_reconstruction_time = std::chrono::duration<double, std::milli>(end - start).count();
        start = std::chrono::high_resolution_clock::now();

        // Compare paths_2bit_from_bitstring and paths_2bit_nonRLE
        bool pathsMatch = (paths_2bit_from_bitstring == paths_2bit_nonRLE);

        // std::cout << "Do paths_2bit_from_bitstring and paths_2bit_nonRLE match? " 
                // << (pathsMatch ? "YES" : "NO") << std::endl;


        // reconstruct from original 2bitpaths
        start = std::chrono::high_resolution_clock::now();
        reconstructed_edgeBits_2bits_original = reconstruct_edgeBits2bits(paths_2bit_from_bitstring, edgeBitsSize, cols_int, rows_int);
        end = std::chrono::high_resolution_clock::now();
        paths_2bits_decompression_time += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        dec_cmv_reconstruction_time = std::chrono::duration<double, std::milli>(end - start).count();

        // ----------------------------------------------
        // reconstruct only directions from rle bitstring
        start = std::chrono::high_resolution_clock::now();
        bitIndex = 0;
        std::vector<std::vector<bool>> reconstructedPaths;
        int countBitsMax = std::ceil(std::log2(std::max(rows_int, cols_int) + 1));

        while (bitIndex + 2 <= paths2bitRLEBitString.size()) {
            std::vector<bool> path;

            while (bitIndex + 2 <= paths2bitRLEBitString.size()) {
                // Read direction (2 bits)
                bool bit1 = paths2bitRLEBitString[bitIndex];
                bool bit2 = paths2bitRLEBitString[bitIndex + 1];
                bitIndex += 2;

                // Stop if delimiter "00"
                if (!bit1 && !bit2) break;

                if(bit1 && bit2){
                    // Read count (fixed length -> countBitsMax)
                    std::vector<bool> countBits(paths2bitRLEBitString.begin() + bitIndex, paths2bitRLEBitString.begin() + bitIndex + countBitsMax);
                    int count = boolVectorToInt(countBits);
                    bitIndex += countBitsMax;

                    // Reconstruct repeated direction
                    for (int j = 0; j < count; j++) {
                        path.push_back(bit1);
                        path.push_back(bit2);
                    }
                }
                else{
                    // Store direction (2 bits) (10 or 01)
                    path.push_back(bit1);
                    path.push_back(bit2);
                }
            }

            reconstructedPaths.push_back(path);
        }
        // use startPoints2Bits, reconstructedPaths to get new pathsFromRLE vector
        PathInfoVector pathsFromRLE;
        for (size_t i = 0; i < startPoints2Bits.size(); ++i) {
            pathsFromRLE.emplace_back(
                startPoints2Bits[i], 
                getDirectionFromIndex(startPoints2Bits[i], rows_int, cols_int), 
                reconstructedPaths[i]
            );
        }
        end = std::chrono::high_resolution_clock::now();
        rle_decompression_time += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        // std::cout << "match?: " << (pathsFromRLE == paths_2bit_nonRLE) << std::endl;

        // reconstruct from 2-bit paths (from first RLE)
        start = std::chrono::high_resolution_clock::now();
        reconstructed_edgeBits_2bits = reconstruct_edgeBits2bits(pathsFromRLE, edgeBitsSize, cols, rows);
        end = std::chrono::high_resolution_clock::now();
        rle_decompression_time += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    }
    

    if(compressionMethods.useStraights && compressionMethods.useHuffman){
        // reconstruct no huffman straights 
        bitIndex = 0;
        Straights straightsNoHuff;
        start = std::chrono::high_resolution_clock::now();
        // std::cout << "no huff straights reconstruction" << std::endl;
        std::string straightsNoHuffBitStringStr;
        for (bool bit : straightsNoHuffBitString) {
            straightsNoHuffBitStringStr += bit ? '1' : '0';
        }
        // std::cout << "size: " << straightsNoHuffBitStringStr.size() << std::endl;
        // Extract cols
        cols_str = straightsNoHuffBitStringStr.substr(bitIndex, 16);
        cols_int = std::stoi(cols_str, nullptr, 2);
        bitIndex += 16;
        // std::cout << "cols: " << cols_int << std::endl;

        // Extract rows
        rows_str = straightsNoHuffBitStringStr.substr(bitIndex, 16);
        rows_int = std::stoi(rows_str, nullptr, 2);
        bitIndex += 16;
        // std::cout << "rows: " << rows_int << std::endl;

        // Compute region color bits size
        regionColorBitsSize = std::ceil(std::log2(cols_int * rows_int + 1));
        regionColorBitsSizeStr = straightsNoHuffBitStringStr.substr(bitIndex, regionColorBitsSize);
        regionColorBitsSizeInt = std::stoi(regionColorBitsSizeStr, nullptr, 2);
        bitIndex += regionColorBitsSize;

        // std::cout << "regionColorBitsSize: " << regionColorBitsSizeInt << std::endl;

        // Parse region color bit string
        regionColorBitStringStr = straightsNoHuffBitStringStr.substr(bitIndex, regionColorBitsSizeInt * 24);
        regionColorsBitStringFromStraights.reserve(regionColorBitsSizeInt * 24);
        for (char c : regionColorBitStringStr) {
            regionColorsBitStringFromStraights.push_back(c == '1');
        }
        bitIndex += regionColorBitStringStr.size();
        // std::cout << "parsed region color" << std::endl;

        // Extract straights amount
        std::string straightsAmountStr = straightsNoHuffBitStringStr.substr(bitIndex, 32);
        int straightsAmount = std::stoi(straightsAmountStr, nullptr, 2);
        bitIndex += 32;
        // std::cout << "extracted straights amounts" << std::endl;

        // extract start points 
        std::string straightsNoHuffStartPointBits = straightsNoHuffBitStringStr.substr(bitIndex, 5);
        int straightsNoHuffStartPointBitsInt = std::stoi(straightsNoHuffStartPointBits, nullptr, 2);
        bitIndex += 5;
        std::vector<std::vector<bool>> noHuffStartPoints;
        for(int i = 0; i < straightsAmount; i++){
            std::string startPointStr = straightsNoHuffBitStringStr.substr(bitIndex, straightsNoHuffStartPointBitsInt);
            int startPoint = std::stoi(startPointStr, nullptr, 2);
            bitIndex += straightsNoHuffStartPointBitsInt;
            noHuffStartPoints.push_back(intToBool(startPoint));
            // std::cout << "start point: " << startPoint << std::endl;
        }
        // std::cout << "extracted start points" << std::endl;

        // extract lengths
        std::string straightsNoHuffLengthsBits = straightsNoHuffBitStringStr.substr(bitIndex, 5);
        int straightsNoHuffLengthsBitsInt = std::stoi(straightsNoHuffLengthsBits, nullptr, 2);
        bitIndex += 5;
        // std::cout << "extracted start bits" << std::endl;
        std::vector<std::vector<bool>> noHuffLengths;
        for(int i = 0; i < straightsAmount; i++){
            std::string lengthStr = straightsNoHuffBitStringStr.substr(bitIndex, straightsNoHuffLengthsBitsInt);
            int length = std::stoi(lengthStr, nullptr, 2);
            bitIndex += straightsNoHuffLengthsBitsInt;
            noHuffLengths.push_back(intToBool(length));
            // std::cout << "length: " << length << std::endl;
        }
        // Combine noHuffStartPoints and noHuffLengths into straightsNoHuff
        for (size_t i = 0; i < noHuffStartPoints.size(); ++i) {
            straightsNoHuff.push_back(std::make_tuple(noHuffStartPoints[i], noHuffLengths[i]));
        }

        // reconstruct from no nuff straights
        reconstructed_edgeBits_straights_no_huff = reconstructStraights(straightsNoHuff, edgeBitsSize, cols, rows);
        end = std::chrono::high_resolution_clock::now();
        straights_decompression_time += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        // std::cout << "ho huff reconstruction match?: " << (straightsNoHuff == straights) << std::endl;

        // --------------------------------------
        // reconstruct huffman straights 
        // std::cout << "parse huffman straights bitstring" << std::endl;
        start = std::chrono::high_resolution_clock::now();
        bitIndex = 0;

        // Convert bool vector to a string representation (can be avoided if direct bit operations are used)
        std::string straightsBitStringStr;
        for (bool bit : straightsBitString) {
            straightsBitStringStr += bit ? '1' : '0';
        }

        // Read 16-bit cols
        cols_int = std::stoi(straightsBitStringStr.substr(bitIndex, 16), nullptr, 2);
        bitIndex += 16;
        // std::cout << "cols: " << cols_int << std::endl;

        // Read 16-bit rows
        rows_int = std::stoi(straightsBitStringStr.substr(bitIndex, 16), nullptr, 2);
        bitIndex += 16;
        // std::cout << "rows: " << rows_int << std::endl;

        // Compute and read regionColorBitsSize
        regionColorBitsSize = std::ceil(std::log2(cols_int * rows_int + 1));
        regionColorBitsSizeInt = std::stoi(straightsBitStringStr.substr(bitIndex, regionColorBitsSize), nullptr, 2);
        bitIndex += regionColorBitsSize;
        // std::cout << "regionColorBitsSize: " << regionColorBitsSizeInt << std::endl;

        // Read region color bitstring
        for (size_t i = 0; i < regionColorBitsSizeInt * 24; ++i) {
            regionColorsBitStringFromStraightsHuffman.push_back(straightsBitStringStr[bitIndex++] == '1');
        }

        // Read Huffman start points amount (32 bits)
        int huffmanStartPointsAmount = std::stoi(straightsBitStringStr.substr(bitIndex, 32), nullptr, 2);
        bitIndex += 32;
        // std::cout << "huffmanStartPointsAmount: " << huffmanStartPointsAmount << std::endl;

        // Read Huffman start points bit size (5 bits)
        int huffmanStartPointsBits = std::stoi(straightsBitStringStr.substr(bitIndex, 5), nullptr, 2);
        bitIndex += 5;
        // std::cout << "huffmanStartPointsBits: " << huffmanStartPointsBits << std::endl;

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
        // std::cout << "huffmanStringSize: " << huffmanStringSize << std::endl;

        // Read Huffman string
        std::string huffmanString = straightsBitStringStr.substr(bitIndex, huffmanStringSize);
        bitIndex += huffmanStringSize;

        // Read straightsLengthsBits (5 bits)
        int straightsLengthsBits = std::stoi(straightsBitStringStr.substr(bitIndex, 5), nullptr, 2);
        bitIndex += 5;
        // std::cout << "straightsLengthsBits: " << straightsLengthsBits << std::endl;

        // Read straightsLengths (16 bits)
        int straightsLengths = std::stoi(straightsBitStringStr.substr(bitIndex, 16), nullptr, 2);
        bitIndex += 16;
        // std::cout << "straightsLengths: " << straightsLengths << std::endl;

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
        // std::cout << "straightsFrequenciesSize: " << straightsFrequenciesSize << std::endl;

        // Read straightsFrequenciesBits (5 bits)
        int straightsFrequenciesBits = std::stoi(straightsBitStringStr.substr(bitIndex, 5), nullptr, 2);
        bitIndex += 5;
        // std::cout << "straightsFrequenciesBits: " << straightsFrequenciesBits << std::endl;

        // Read frequency values
        straightLengthFrequencies.clear();
        for (size_t i = 0; i < straightsFrequenciesSize; ++i) {
            uint32_t frequency = std::stoi(straightsBitStringStr.substr(bitIndex, straightsFrequenciesBits), nullptr, 2);
            bitIndex += straightsFrequenciesBits;
            straightLengthFrequencies.push_back(frequency);
        }

        end = std::chrono::high_resolution_clock::now();
        straights_huffman_decompression_time += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        // create huffman tree from lengths and frequencies 
        start = std::chrono::high_resolution_clock::now();
        map<int,int> straightLengths;
        for (size_t i = 0; i < straightLengthsList.size(); ++i) {
            straightLengths[straightLengthsList[i]] = straightLengthFrequencies[i];
        }
        std::map<int, string> straightsHuffmanCodes;
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
        end = std::chrono::high_resolution_clock::now();
        sls_reconstruction_time = std::chrono::duration<double, std::milli>(end - start).count();
        start = std::chrono::high_resolution_clock::now();
        reconstructed_edgeBits_straights = reconstructStraights(straightsDecoded, edgeBitsSize, cols, rows);
        end = std::chrono::high_resolution_clock::now();
        sls_cmv_reconstruction_time = std::chrono::duration<double, std::milli>(end - start).count();
        straights_huffman_decompression_time += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        // delete huffman tree from memory 
        deleteHuffmanTree(reconstructedRoot);   
    }
    

    

    // Compare straights and straightsDecoded
    //bool areStraightsIdentical = (straights == straightsDecoded);
    

    int numberOfPaths = paths.size();


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

    


    // Print the decoded straights
    // std::cout << "Decoded Straights size: " << straightsDecoded.size() << std::endl;
    // empty reconstruction
    std::vector<bool> empty_reconstruction = std::vector<bool>(edgeBitsSize, true);

    std::vector<RGB> regionColorsFromBitString = colorBitStringToRGBVector(regionColorsBitStringFromTree);
    bool success = false;
    bool allRegionColorBitStringsMatch = true;

    std::vector<std::vector<bool>> regionColorBitStrings = {
        regionColorsBitStringFromEdgeBits,
        regionColorsBitStringFromReducedEdgeBits,
        regionColorsBitStringFromTree,
        regionColorsBitStringFrom2BitPaths,
        regionColorsBitStringFromRLE,
        regionColorsBitStringFromStraights,
        regionColorsBitStringFromStraightsHuffman
    };

    for (size_t i = 0; i < regionColorBitStrings.size(); ++i) {
        if (regionColorBitStrings[i].empty()) {
            continue; // Skip empty bitstrings
        }
        for (size_t j = i + 1; j < regionColorBitStrings.size(); ++j) {
            if (regionColorBitStrings[j].empty()) {
                continue; // Skip empty bitstrings
            }
            if (regionColorBitStrings[i] != regionColorBitStrings[j]) {
                allRegionColorBitStringsMatch = false;
                std::cout << "Mismatch found between regionColorBitString " << i 
                          << " and regionColorBitString " << j << std::endl;
            }
        }
    }

    // std::cout << "All region color bitstrings match: " 
    //           << (allRegionColorBitStringsMatch ? "YES" : "NO") << std::endl;

    success = allRegionColorBitStringsMatch;
    // std::cout << "region colors match?: " << (regionColorsFromBitString == regionColors) << std::endl;

    //std::cout << "size of directionbits: " << directionBitsSize << std::endl;



    //std::cout << "\nreconstruction for edgebits01 finished" << std::endl;

    //reconstructed_edgeBits.assign(reconstructed_edgeBits.size(), false);
    start = std::chrono::high_resolution_clock::now();
    

    // reconstruct with UnionFind
    // andres::Partition<int> reconstruction = getRegions(reconstructed_edgeBits_from_paths, rows_int, cols_int);
    
    // //printColorRegions();
    // std::map<int, int> representativeLabels;
    // reconstruction.representativeLabeling(representativeLabels);
    // //std::vector<int> reps;
    // //reconstruction.representatives(std::back_inserter(reps));
    // for (int index = 0; index < rows * cols; ++index) {

    //     //std::cout << reconstruction.find(index) << ", ";

    //     // Calculate row and column indices from the linear index
    //     int y = index / cols;
    //     int x = index % cols;
    //     if(index%100 == 0){
    //         //printProgressBar(index, rows*cols);
    //     }
    //     // get color
    //     int region = reconstruction.find(index);
    //     int continuousLabel = representativeLabels[region];

    //     // an welchem index steht nummer "region" im vector der representatives
        
    //     /*
    //     //quadratic runtime?
    //     auto it = std::find(reps.begin(), reps.end(), region);
    //     std::size_t indexInReps;

    //     if (it != reps.end()) {
    //         indexInReps = std::distance(reps.begin(), it);
    //         //std::cout << "Index of region " << region << " in reps: " << indexInReps << std::endl;
    //     }
    //     */
        

    //     //RGB col = regionColors[indexInReps];
    //     RGB col = regionColorsFromBitString[continuousLabel];


    //     //RGB col = getVertexColor(0);
    //     //std::cout << col.green.to_ulong() << std::endl;
    //     // Set the color (BGR format)
    //     image.at<cv::Vec4b>(y, x) = cv::Vec4b(col.blue, col.green, col.red, transparencyValues[index]);  
    // }
    // end = std::chrono::high_resolution_clock::now();
    // std::cout << "UF reconstruction time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;
    
    // reconstruct with dfs
    start = std::chrono::high_resolution_clock::now();
    // image = setRegionColorsFromImageSearch(image, reconstructed_edgeBits_from_paths, regionColorsFromBitString, transparencyValues);
    // image = setRegionColorsFromImageSearch(image, reconstructed_edgeBits_from_paths, decodedColorsTree, transparencyValues);
    // image = setRegionColorsFromImageSearch(image, reconstructed_edgeBits_from_paths, inflatedRegionColorsVec, transparencyValues);
    image = setRegionColorsFromImageSearch(image, reconstructed_edgeBits_from_paths, decodedColorsTree_inflated, transparencyValues);
    end = std::chrono::high_resolution_clock::now();
    // std::cout << "DFS reconstruction time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;
    dfs_reconstruction_time = std::chrono::duration<double, std::milli>(end - start).count();

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

    end = std::chrono::high_resolution_clock::now();
    tree_decompression_time += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    old_decompression_time += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    rle_decompression_time += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    straights_huffman_decompression_time += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    reduced_edgebits_decompression_time += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    //printSize();
    //bool success = areImagesIdentical(originalImg, image);
    //bool success = (edgeBits01 == reconstructed_edgeBits_from_paths) && (edgeBits01 == reconstructed_edgeBits_horizontals) && (edgeBits01 == reconstructed_edgeBits_2bits) && (edgeBits01 == reconstructed_edgeBits_straights) && (edgeBits01 == edgeBitsFromBitString) && (edgeBits01 == reconstructed_edgeBits_2bits_original) && (edgeBits01 == reconstructed_edgeBits_straights_no_huff);
    // bool success = true;

    if (compressionMethods.useTree) {
        bool match = (edgeBits01 == reconstructed_edgeBits_from_paths);
        success = success && match;
        if (!match)
            std::cout << "Reconstructed edge bits from tree do not match!\n";
    }

    if (compressionMethods.useReducedEdgebits) {
        bool match = (edgeBits01 == reconstructed_edgeBits_horizontals);
        success = success && match;
        if (!match)
            std::cout << "Reconstructed edge bits from horizontals do not match!\n";
    }

    if (compressionMethods.use2bits) {
        bool match1 = (edgeBits01 == reconstructed_edgeBits_2bits);
        bool match2 = (edgeBits01 == reconstructed_edgeBits_2bits_original);
        success = success && match1 && match2;
        if (!match1)
            std::cout << "Reconstructed edge bits from 2bits do not match!\n";
        if (!match2)
            std::cout << "Reconstructed edge bits from 2bits original do not match!\n";
    }

    if (compressionMethods.useStraights && compressionMethods.useHuffman) {
        bool match1 = (edgeBits01 == reconstructed_edgeBits_straights);
        bool match2 = (edgeBits01 == reconstructed_edgeBits_straights_no_huff);
        success = success && match1 && match2;
        if (!match1)
            std::cout << "Reconstructed edge bits from straights do not match!\n";
        if (!match2)
            std::cout << "Reconstructed edge bits from straights no huff do not match!\n";
    }

    if (compressionMethods.useEdgebits) {
        bool match = (edgeBits01 == edgeBitsFromBitString);
        success = success && match;
        if (!match)
            std::cout << "Reconstructed edge bits from bitstring do not match!\n";
    }


    cv::Mat reconstructedBGR;
    cv::cvtColor(image, reconstructedBGR, cv::COLOR_BGRA2BGR);

    success = success && areImagesIdentical(originalImg, reconstructedBGR);

    // std::cout << "channels for original image: " << originalImg.channels() << std::endl;
    // cv::Vec4b firstPixel = image.at<cv::Vec4b>(0, 0);
    // std::cout << "First pixel RGBA values: R=" << (int)firstPixel[2]
    //           << ", G=" << (int)firstPixel[1]
    //           << ", B=" << (int)firstPixel[0]
    //           << ", A=" << (int)firstPixel[3] << std::endl;
    // // List pixel colors (r, g, b) from each image
    // std::cout << "Original Image Pixel Colors (R, G, B):" << std::endl;
    // for (int y = 0; y < originalImg.rows; ++y) {
    //     for (int x = 0; x < originalImg.cols; ++x) {
    //         cv::Vec3b color = originalImg.at<cv::Vec3b>(y, x);
    //         std::cout << "(" << (int)color[2] << ", " << (int)color[1] << ", " << (int)color[0] << ") ";
    //     }
    //     std::cout << std::endl;
    // }

    // std::cout << "Reconstructed Image Pixel Colors (R, G, B):" << std::endl;
    // for (int y = 0; y < image.rows; ++y) {
    //     for (int x = 0; x < image.cols; ++x) {
    //         cv::Vec4b color = image.at<cv::Vec4b>(y, x);
    //         std::cout << "(" << (int)color[2] << ", " << (int)color[1] << ", " << (int)color[0] << ") ";
    //     }
    //     std::cout << std::endl;
    // }



    std::cout << (success ? "✅" : "❌") << std::endl;
    
    if(showImg){
        cv::imwrite("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/output_files/reconstructed.png", image);
        cv::destroyAllWindows();
        cv::imshow("Original", originalImg);
        cv::imshow("Reconstruction", image);
        cv::waitKey(0);
    }


    decompInfo decomp_info = 
    {success, 
    tree_decompression_time,
    old_decompression_time,
    rle_decompression_time,
    straights_huffman_decompression_time,
    reduced_edgebits_decompression_time,
    straights_decompression_time,
    paths_2bits_decompression_time,
    rebuild_dpcm_huffman_time,
    decode_colors_time,
    assemble_tree_paths_time,
    reconstruct_tree_edgebits_time,
    dfs_reconstruction_time,
    reconstruct_rcmv_time,
    reconstruct_rcmv_cmv_time,
    dec_reconstruction_time,
    dec_cmv_reconstruction_time,
    sls_reconstruction_time,
    sls_cmv_reconstruction_time
    };
    
    return decomp_info;
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

std::vector<RGB> decodeDifferences(const std::vector<uint8_t>& encodedDifferences) {
    std::vector<RGB> decodedPixels;

    if (encodedDifferences.size() < 3) {
        throw std::runtime_error("Encoded data too short to decode");
    }

    // First pixel is stored explicitly
    RGB firstPixel = { encodedDifferences[0], encodedDifferences[1], encodedDifferences[2] };
    decodedPixels.push_back(firstPixel);

    uint8_t r = firstPixel.red;
    uint8_t g = firstPixel.green;
    uint8_t b = firstPixel.blue;

    // Decode differences cyclically
    for (size_t i = 3; i < encodedDifferences.size(); i += 3) {
        uint8_t prev_r = r;
        r = static_cast<uint8_t>(r + static_cast<int8_t>(encodedDifferences[i]));     // Restore R
        g = static_cast<uint8_t>(r + static_cast<int8_t>(encodedDifferences[i + 1])); // Restore G
        b = static_cast<uint8_t>(r + static_cast<int8_t>(encodedDifferences[i + 2])); // Restore B

        decodedPixels.push_back({ r, g, b });
    }

    return decodedPixels;
}

bool ZlibInflate(const std::vector<uint8_t>& compressedData, std::vector<uint8_t>& decompressedData){
    z_stream strm{};
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    int ret = inflateInit(&strm);
    if (ret != Z_OK) {
        std::cout << "inflateInit failed: " << ret << std::endl;
        return false; // Initialization failed
    }

    const size_t chunkSize = 262144; // 256 KB
    uint8_t outBuffer[chunkSize];

    strm.avail_in = compressedData.size();
    strm.next_in = const_cast<uint8_t*>(compressedData.data());

    do{
        
        strm.avail_out = chunkSize;
        strm.next_out = outBuffer;

        ret = inflate(&strm, Z_NO_FLUSH);
        if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) {
            std::cout << "inflate failed: " << ret << std::endl;
            inflateEnd(&strm);
            return false; // Decompression failed
        }

        size_t have = chunkSize - strm.avail_out;
        decompressedData.insert(decompressedData.end(), outBuffer, outBuffer + have);
    }while(ret != Z_STREAM_END);

    inflateEnd(&strm);

    return true;
}

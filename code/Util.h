#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include "partition.hxx"
#include "DirectionPath.h"
#include <filesystem>
#include <type_traits>
#include <fstream>
#include <iostream>
#include "Image.h"
#include <fstream>
#include <unordered_set>


struct methods{
    bool useEdgebits = true;
    bool useReducedEdgebits = true;
    bool useTree = true;
    bool use2bits = true;
    bool useRLE = true;
    bool useStraights = true;
    bool useHuffman = true;
};

andres::Partition<int> getRegions(std::vector<bool>& edgeBitsVector, int rows, int cols);
int getNeighbor(int currentEdge, Direction currentDirection, int neighborIndex, int cols, int rows);
Direction nextDirection(Direction dir);
Direction previousDirection(Direction dir);
int getEdgeBitFromList(int v, int w, std::vector<bool>& edgebitsvector, int rows, int cols);
int mapVerticalToEdgebitsIndex(int verticalIndex, int cols, int rows);
int mapHorizontalToEdgebitsIndex(int horizontalIndex, int cols, int rows);
void printProgressBar(int progress, int total);
Direction getDirectionFromIndex(int index, int row, int col);
std::string directionToString(Direction dir);
std::tuple<std::vector<bool>, std::vector<uint16_t>, bool> getRLE(std::vector<bool> bits);
std::vector<bool> reconstructRLE(std::vector<bool> zeros_rle, std::vector<uint16_t> ones_rle, bool start);
std::vector<bool> intToBool(int num, int padding=0);
int boolVectorToInt(const std::vector<bool>& binary);
std::vector<bool> boolVectorFromRGBVector(std::vector<RGB>& regionColors);
std::vector<RGB> colorBitStringToRGBVector(std::vector<bool>& bitString);
int calculateBoolVectorStorage(std::vector<bool>& boolVector);
/**
 * @brief writes values of arbitrary type into a csv file 
 * @param p1 filepath
 * @param values values to be added to the file comma-separated 
 * @param category image category for allocating the correct output folder 
 */
template<typename T>
void writeToOutput(const std::filesystem::path& p1, const std::vector<T>& values, const std::string category) {
    // Create the output file path using the provided path p1
    std::filesystem::create_directories("2ndoutput/" + category);
    std::ofstream outputFile("2ndoutput/" + category + "/output_" + p1.filename().string() + ".csv");

    // Check if the file opened successfully
    if (!outputFile.is_open()) {
        std::cerr << "Error: Unable to open the file." << std::endl;
        return;
    }

    // Iterate over the vector and write its elements to the file
    for (size_t i = 0; i < values.size(); ++i) {
        outputFile << values[i]; // Write the element

        // Add a comma if it's not the last element
        if (i != values.size() - 1) {
            outputFile << ",";
        }
    }
        
    outputFile.close();
}
int countImgFiles(const std::filesystem::path& parentDir, std::unordered_set<std::string> category_set);
int countDirectImgFiles(const std::filesystem::path& parentDir);
std::pair<int,int> getPixelIndexFromEdgeIndex(int edgeIndex, int cols, int rows);
int getEdgeIndexFromPixelIndices(int pixelIndex1, int pixelIndex2, int cols, int rows);
cv::Mat setRegionColorsFromImageSearch(cv::Mat img, std::vector<bool>& edgeBits, std::vector<RGB>& regionColors, std::vector<uint8_t>& transparencyValues);
std::vector<RGB> separatedChannelstoRGBVector(const std::vector<uint8_t>& separated);

#endif // UTIL_H
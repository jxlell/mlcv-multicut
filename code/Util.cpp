#include "Util.h"
#include <iostream>
#include "DirectionPath.h"
#include <filesystem>
#include <fstream>
#include <numeric>
#include <type_traits>
#include <unordered_set>
#include <stack>

int getNeighbor(int currentEdge, Direction currentDir, int neighborIndex, int cols, int rows){
    std::vector<int> edgeOffsets;

    int row = currentEdge/(2*cols-1) + 1; // 25 for 6409
    int column = (currentEdge % (2*cols-1)) / 2; // 0 for 6409
    //std::cout << "row: " << row << std::endl;
    //std::cout << "column: " << column << std::endl;

    switch (currentDir)
    {
    // offsets are in order of direction 3-bit-representation
    //TODO: row is never == 0?
    case Direction::UP:
        if(row == 0){
            return -1;
        }
        // last row case?
        else if(row==rows){
            // calculate column in last row differently
            column = currentEdge % (2*cols-1);
            edgeOffsets = {-(2*cols-1)+column, -(2*cols-1)+column+1, -(2*cols-1)+column+2};
        }else{
            edgeOffsets = {-2*(cols-1)-2, -2*(cols-1)-1, -2*(cols-1)};
        }
        break;
    case Direction::DOWN:
        if(row == rows){
            return -1;
        }
        // last row case?
        if(row==rows-1){
            //std::cout << "last row case for DOWN: " << currentEdge << std::endl;
            edgeOffsets = {1,2*cols-2-column, -1};
        }else{
            edgeOffsets = {1,2*cols-1, -1};
        }
        break;
    case Direction::LEFT:
        if(column == 0){
            return -1;
        }
        // last row case?
        if(row==rows-1){
            //std::cout << "last row case for LEFT: " << currentEdge << std::endl;
            edgeOffsets = {2*cols-2-column, -2, -1};
        }else{
            edgeOffsets = {2*(cols-1), -2,-1};
        }
        break;
    case Direction::RIGHT:
        if(column == cols-1){
            return -1;
        }
        // last row case?
        if(row==rows-1){
            //std::cout << "last row case for RIGHT: " << currentEdge << ", column " << column << std::endl;
            edgeOffsets = {1, 2, 2*cols-1-column};
        }
        else{
            edgeOffsets = {1, 2, 2*(cols-1)+2};
        }
        break;
    
    default:
        edgeOffsets = {1, 2, 2*(cols-1)+2};
        break;
    }
    
    //return edgeBits01[currentEdge + edgeOffsets[neighborIndex]];
    return std::max(-1,currentEdge + edgeOffsets[neighborIndex]);
}

Direction nextDirection(Direction dir) {
    return static_cast<Direction>((static_cast<int>(dir) + 1) % 4);
}

Direction previousDirection(Direction dir) {
    int newDir = (static_cast<int>(dir) - 1) % 4;
    if (newDir < 0) newDir += 4;  // Handle negative wrap-around
    return static_cast<Direction>(newDir);
}

andres::Partition<int> getRegions(std::vector<bool>& edgeBitsVector, int rows, int cols){
    andres::Partition<int> region(rows*cols);
    std::vector<int> neighborsOffsets = {cols, 1};
    for (int index = 0; index < rows * cols; ++index) {
        for (int offset : neighborsOffsets) {
            int neighbor = index + offset;
            if (neighbor >= 0 && neighbor < rows*cols && ((neighbor / cols == index / cols) || (abs(neighbor - index) > 1))) {
                // check if neighbours are separated by multicut 
                // if no: merge 

                if(getEdgeBitFromList(index, neighbor, edgeBitsVector, rows, cols) == 0){
                    region.merge(index, neighbor);
                    //std::cout << reconstruction.find(index) << ", ";
                    //std::cout << "merged " << index << " and " << neighbor << std::endl;
                }

                
            }
        }
    }
    return region;
}

int getEdgeBitFromList(int v, int w, std::vector<bool>& edgebitsvector, int rows, int cols) {
    int index = std::min(v,w);
    int row = index / cols + 1;
    int col = index % cols + 1;
    //std::cout << std::endl << "row: " << row << std::endl;
    //rechter nachbar?
    //TODO: berechnung nicht richtig für last row case!!
    if(abs(v-w) == 1 && row != rows){
        int rightBit = edgebitsvector[index * 2 - (row - 2)];
        //std::cout << "rightBitIndex: " << index * 2 - (row - 2) << std::endl;
        return rightBit;
    }
    if(abs(v-w) == 1 && row == rows){
        int rightBit = edgebitsvector[index * 2 - (row - 2) - col];
        //std::cout << "rightBitIndex: " << index * 2 - (row - 1) << std::endl;
        return rightBit;
    }
    //sonst unterer nachbar
    else{
        int edgeI = index * 2 - (row - 1);
        int downBit = edgebitsvector[edgeI];
        //std::cout << "downBitIndex: " << edgeI << std::endl;
        return downBit;
    }
    
}

void printProgressBar(int progress, int total) {
    float percentage = static_cast<float>(progress) / total;
    int width = 50;
    int barWidth = static_cast<int>(percentage * width);

    std::cout << "\r[";
    for (int i = 0; i < barWidth; ++i) {
        std::cout << "=";
    }
    for (int i = barWidth; i < width; ++i) {
        std::cout << " ";
    }
    std::cout << "] " << std::setprecision(3) << percentage * 100.0 << "%";
    std::cout.flush();
}

Direction getDirectionFromIndex(int index, int rows, int cols){
    Direction horizontalDir = Direction::DOWN;
    Direction verticalDir = Direction::RIGHT;

    int row = index / (2*cols-1);
    int col = index % (2*cols-1);

    if(index>2*cols*rows-cols-rows-cols){
        verticalDir = Direction::UP;
        return verticalDir;
    }else{
        verticalDir = Direction::DOWN;
    }
    if((index+1) % (2*cols-1) == 0){
        horizontalDir = Direction::LEFT;
    }else{
        horizontalDir = Direction::RIGHT;
    }

    
    Direction currentDir = (row % 2 == 0) ? 
        ((index % 2 == 0) ? horizontalDir : verticalDir) : 
        ((index % 2 == 0) ? verticalDir : horizontalDir);

    return currentDir;
}

std::string directionToString(Direction dir){
    switch (dir)
    {
    case Direction::UP:
        return "UP";
        break;
    case Direction::DOWN:
        return "DOWN";
        break;
    case Direction::LEFT:
        return "LEFT";
        break;
    case Direction::RIGHT:
        return "RIGHT";
        break;
    default:
        return "UNKNOWN";
        break;
    }
}

std::tuple<std::vector<bool>, std::vector<uint16_t>, bool> getRLE(std::vector<bool> bits){
    bool start = bits[0];
    std::vector<uint16_t> ones_rle;
    std::vector<bool> zeros_rle;
    for(int i = 0; i < bits.size(); i++){
        if(i > 0 && bits[i] == 0 && bits[i-1] == 0){
            continue;
        }
        if(bits[i] == 0){
            if(bits[i+1] == 0 && i+1 < bits.size()){
                zeros_rle.push_back(1);
            }else{
                zeros_rle.push_back(0);
            }
            continue;
        }
        if(bits[i] == 1){
            int count = 1;
            while(i+1 < bits.size() && bits[i+1] == 1){
                count++;
                i++;
            }
            ones_rle.push_back(count);
        }

    }


    return std::make_tuple(zeros_rle, ones_rle, start);
}


std::vector<bool> reconstructRLE(std::vector<bool> zeros_rle, std::vector<uint16_t> ones_rle, bool start){
    std::vector<bool> reconstructed;
    if(!start){
        for(size_t i = 0; i<zeros_rle.size(); i++){
            if(zeros_rle[i] == 1){
                reconstructed.push_back(false);
                reconstructed.push_back(false);
            }else{
                reconstructed.push_back(false);
            }
            if(i < ones_rle.size()){
                int count = ones_rle[i];
                while(count > 0){
                    reconstructed.push_back(true);
                    count--;
                }
            }
        }
    }else{
        for(size_t i = 0; i<ones_rle.size(); i++){
            int count = ones_rle[i];
            while(count > 0){
                reconstructed.push_back(true);
                count--;
            }
            if(i < zeros_rle.size() && zeros_rle[i] == 1){
                reconstructed.push_back(false);
                reconstructed.push_back(false);
            }else if(i < zeros_rle.size()){
                reconstructed.push_back(false);
            }
        }
    }
    
    return reconstructed;
}

std::vector<bool> intToBool(int num, int padding) {
    std::vector<bool> binary;
    if (num == 0) {
        binary.push_back(false); // Special case for 0
    } else {
        while (num > 0) {
            binary.push_back(num & 1); // Extract LSB
            num >>= 1; // Right shift
        }
        std::reverse(binary.begin(), binary.end()); // Reverse to get MSB first
    }
    
    if (padding > 0 && binary.size() < static_cast<size_t>(padding)) {
        binary.insert(binary.begin(), padding - binary.size(), false); // Add leading zeros
    }
    
    return binary;
}

int boolVectorToInt(const std::vector<bool>& binary) {
    int num = 0;
    for (bool bit : binary) {
        num = (num << 1) | bit; // Shift left and OR with bit
    }
    return num;
}

std::vector<bool> boolVectorFromRGBVector(std::vector<RGB>& regionColors){
    std::vector<bool> boolVector;
    boolVector.reserve(regionColors.size() * 24); // Each RGB has 24 bits

    for (const auto& color : regionColors) {
        for (int i = 7; i >= 0; --i) { // Extract bits for red
            boolVector.push_back((color.red >> i) & 1);
        }
        for (int i = 7; i >= 0; --i) { // Extract bits for green
            boolVector.push_back((color.green >> i) & 1);
        }
        for (int i = 7; i >= 0; --i) { // Extract bits for blue
            boolVector.push_back((color.blue >> i) & 1);
        }
    }

    return boolVector;
}

std::vector<RGB> colorBitStringToRGBVector(std::vector<bool>& bitString){
    std::vector<RGB> regionColors;
    for (size_t i = 0; i < bitString.size(); i+=24) {
        RGB color;
        color.red = boolVectorToInt(std::vector<bool>(bitString.begin() + i, bitString.begin() + i + 8));
        color.green = boolVectorToInt(std::vector<bool>(bitString.begin() + i + 8, bitString.begin() + i + 16));
        color.blue = boolVectorToInt(std::vector<bool>(bitString.begin() + i + 16, bitString.begin() + i + 24));
        regionColors.push_back(color);
    }
    return regionColors;
}

std::vector<RGB> separatedChannelstoRGBVector(const std::vector<uint8_t>& separated) {
    size_t numPixels = separated.size() / 3;
    std::vector<RGB> rgbVector;
    rgbVector.reserve(numPixels);

    for (size_t i = 0; i < numPixels; ++i) {
        RGB color;
        color.red   = separated[i];
        color.green = separated[i + numPixels];
        color.blue  = separated[i + 2 * numPixels];
        rgbVector.push_back(color);
    }

    return rgbVector;
}

int calculateBoolVectorStorage(std::vector<bool>& boolVector){
    // 64-bit chunks + overhead (8 bytes for pointer to memory, size and capacity respectively)
    return ((boolVector.capacity()+7) / 8) * 8 + 24*8;
}

/**
 * @brief counts number of JPG and PNG files in a directory 
 * @param parentDir directory path 
 * @return number of images (.jpg and .png)
 */
int countImgFiles(const std::filesystem::path& parentDir, std::unordered_set<std::string> category_set) {
    int count = 0;
    for (const auto& entry : std::filesystem::directory_iterator(parentDir)) {
        if (!entry.is_directory() || category_set.find(entry.path().filename().string()) == category_set.end()) {
            continue;
        }
        for (const auto& dirEntry : std::filesystem::directory_iterator(entry)) {
            if (dirEntry.path().extension() == ".png" || dirEntry.path().extension() == ".jpg") {
                ++count;
            }
        }
    }
    return count;
}

int countDirectImgFiles(const std::filesystem::path& parentDir) {
    int count = 0;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(parentDir)) {
        if (entry.path().extension() == ".png" || entry.path().extension() == ".jpg") {
            ++count;
        }
    }
    return count;
}

int mapHorizontalToEdgebitsIndex(int verticalIndex, int cols, int rows){
    int row = verticalIndex / (rows - 1);
    int col = verticalIndex % (rows - 1);

    int edgebitsIndex = 0;
    edgebitsIndex = (cols - 1 + rows) * row * (col + 1);

    return edgebitsIndex;
}

int mapVerticalToEdgebitsIndex(int verticalIndex, int cols, int rows){
    int row = verticalIndex / (rows - 1);
    int col = verticalIndex % (rows - 1);

    int edgebitsIndex = 0;
    edgebitsIndex = (cols - 1 + rows) * row * (col + 1) + cols - 1;

    return edgebitsIndex;
}


std::pair<int,int> getPixelIndexFromEdgeIndex(int edgeIndex, int cols, int rows){
    if(edgeIndex < 0 || edgeIndex >= (2 * rows * cols) - rows - cols){
        return std::make_pair(-1, -1);
    }
    
    bool horizontal;
    Direction dir = getDirectionFromIndex(edgeIndex, rows, cols);
    if(dir == Direction::UP || dir == Direction::DOWN){
        horizontal = false;
    }else{
        horizontal = true;
    }
    // std::cout << (horizontal ? "horizontal" : "vertical") << std::endl;
    int row = edgeIndex / (2 * cols - 1);
    int col = (edgeIndex % (2 * cols - 1)) / 2;
    if(!horizontal && row == rows - 1){
        col = edgeIndex % (2 * cols - 1);
    }

    int pixelIndex = row * cols + col;

    if(!horizontal){
        return std::make_pair(pixelIndex, pixelIndex + 1);
    }

    return std::make_pair(pixelIndex, pixelIndex + cols);
}

int getEdgeIndexFromPixelIndices(int pixelIndex1, int pixelIndex2, int cols, int rows){
    if(pixelIndex1 < 0 || pixelIndex2 < 0 || pixelIndex1 >= rows * cols || pixelIndex2 >= rows * cols){
        return -1;
    }
    if (pixelIndex1 > pixelIndex2) {
        std::swap(pixelIndex1, pixelIndex2);
    }
    int row1 = pixelIndex1 / cols;
    int col1 = pixelIndex1 % cols;
    int row2 = pixelIndex2 / cols;
    int col2 = pixelIndex2 % cols;

    if(row1 == row2 && abs(col1 - col2) == 1){
        if(row1 == rows - 1){
            // last row case
            return (row1 * (2 * cols - 1)) + col1;
        }
        return (row1 * (2 * cols - 1)) + (col1 + col2);
    }else if(col1 == col2 && abs(row1 - row2) == 1){
        return (row1 * (2 * cols - 1)) + (col1 + col2);
    }
    
    return -1;
}

cv::Mat setRegionColorsFromImageSearch(cv::Mat img, std::vector<bool>& edgeBits, std::vector<RGB>& regionColors, std::vector<uint8_t>& transparencyValues){
    int rows = img.rows;
    int cols = img.cols;
    int vertices = rows * cols;
    cv::Mat image(img.rows, img.cols, CV_8UC4, cv::Scalar(0, 0, 0, 0)); 
    std::stack<int> stack;
    std::vector<bool> visited(vertices, false);
    int regionIndex = 0;
    for(int index = 0; index < vertices; index++){
        if(visited[index]){
            continue;
        }
        stack.push(index);

        RGB currentColor = regionColors[regionIndex];
        regionIndex++;

        while(!stack.empty()){
            int current = stack.top();
            stack.pop();
            visited[current] = true;
            int row = current / cols;
            int col = current % cols;
            image.at<cv::Vec4b>(row, col) = cv::Vec4b(currentColor.blue, currentColor.green, currentColor.red, transparencyValues[current]);

            // check right neighbor
            int right = current + 1;
            if (col < cols - 1 && !visited[right] && getEdgeBitFromList(current, right, edgeBits, rows, cols) == 0) {
                stack.push(right);
                visited[right] = true;
            }

            // check below neighbor
            int below = current + cols;
            if (row < rows - 1 && !visited[below] && getEdgeBitFromList(current, below, edgeBits, rows, cols) == 0) {
                stack.push(below);
                visited[below] = true;
            }

            // check left neighbor
            int left = current - 1;
            if (col > 0 && !visited[left] && getEdgeBitFromList(current, left, edgeBits, rows, cols) == 0) {
                stack.push(left);
                visited[left] = true;
            }

            // check above neighbor
            int above = current - cols;
            if (row > 0 && !visited[above] && getEdgeBitFromList(current, above, edgeBits, rows, cols) == 0) {
                stack.push(above);
                visited[above] = true;
            }
        }
    }
    return image;
}

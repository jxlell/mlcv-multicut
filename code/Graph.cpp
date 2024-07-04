// Graph.cpp
#include "Graph.h"
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <stack>
#include <unordered_set>
#include "partition.hxx"
#include <cstdlib>
#include <queue>

Graph::Graph(const std::string& imagePath) {
    

    //img = cv::imread(imagePath);
    img = cv::imread(imagePath, /*cv::IMREAD_UNCHANGED*/ cv::IMREAD_COLOR);
    //img = generateRandomImage(39,29);

    if (img.empty()) {
        std::cerr << "Error: Could not read the image: " << imagePath << std::endl;
    }
    this->cols = img.cols;
    this->rows = img.rows;
    //this->neighborsOffsets = {-cols, -1, cols, 1};
    this->neighborsOffsets = {cols, 1};
    vertices = rows*cols;


    this->imagePath = imagePath;
    adjList.resize(vertices);
    vertexValues.resize(vertices, 0);
    vertexColors.resize(vertices);
    //TODO: init size richtig
    edgeBits.resize(2*vertices, std::bitset<1>(0));
    edgeBitsRLE.resize(2*vertices, std::bitset<1>(0));
    edgeBits01.resize((cols-1)*rows + cols*(rows-1));
    visited.resize(edgeBits01.size());
    vertexRegions.resize(vertices, -1);

    regionRepresentatives.resize(cols*rows, -1);
    dualBits.resize((cols+1)*(rows+1));
}

int Graph::getVertices() const{
    return vertices;
}

std::string Graph::getImagePath() const{
    return imagePath;
}

std::vector<int> Graph::getVertexRegions(){
    return vertexRegions;
}

//std::vector<RGB> getRegionColors(){
//    return regionColors;
//}

// Function to add an edge to the graph
void Graph::addEdge(int v, int w) {
    if (find(adjList[v].begin(), adjList[v].end(), w) == adjList[v].end()) {
        adjList[v].push_back(w); // Add w to v's list
        adjList[w].push_back(v); // Add v to w's list (for an undirected graph)
    }
}


// Function to add a vertex to the graph
void Graph::addVertex() {
    vertices++;
    adjList.push_back(std::vector<int>()); // Add an empty vector for the new vertex
}

// Function to print the graph
void Graph::printGraph() {
    for (int v = 0; v < vertices; ++v) {
        std::cout << "vertex " << v << ": " << std::endl ;
        std::cout << "Adjacency list: ";
        for (const auto &neighbor : adjList[v]) {
            std::cout << neighbor << "(" << getEdgeBit(v,neighbor) << ")" << " ";
        }
        RGB color = getVertexColor(v);
        std::cout << std::endl << "Color (RGB): " << static_cast<int>(color.red) << ", " << static_cast<int>(color.green) << ", " << static_cast<int>(color.blue)
             << std::endl;
        
    }
    std::cout << "number of edges: " << countEdges() << std::endl;

}

void Graph::printEdgeBits(){
    for (int i = 0; i < edgeBits.size(); ++i) {
        std::cout << edgeBits[i] << " ";
    }
    std::cout << std::endl;
}

void Graph::printColorRegions(){
    for (const auto& color : regionColors) {
        std::cout << "RGB: " << static_cast<int>(color.red) << ", " << static_cast<int>(color.green) << ", " << static_cast<int>(color.blue) << std::endl;
    }
    
}

void Graph::printSize(){
    //std::cout << "size of regionColors " << "(" << regionColors.size() << " entries): " << sizeof(regionColors) * 8 << std::endl;
    //std::cout << sizeof(getVertexColor(0))<< std::endl;
    //std::cout << "size of regions vector: " << sizeof(regions) * 8 << std::endl;
    //std::cout << "total: " << sizeof(regionColors) * 8 + sizeof(regions) * 8 << std::endl;
    //std::cout << "size of one RGB value: " << sizeof(getVertexColor(0)) * 8 << std::endl;
    //RGB test;
    //std::cout << "size of one test RGB value: " << sizeof(test) * 8 << std::endl;
    //std::cout << imagePath << ": " << std::endl;
    //std::cout << "size of vertexRegions: " << vertexRegions.size() << std::endl;
    //std::cout << "size of vertexRegions in kBit: " << vertexRegions.size() * sizeof(int) * 8 / 1024 << std::endl;
    //std::cout << "size of regionColors in kBit: " << regionColors.size() * sizeof(RGB) * 8 / 1024 << std::endl;
    //std::cout << "size of regionColors vector: " << regionColors.size() << std::endl;
    //std::cout << "size of int: " << sizeof(int) << std::endl;
    //std::cout << "size of RGB: " << sizeof(RGB) << std::endl;
    //std::cout << "size of uint: " << sizeof(uint8_t) << std::endl;

    //std::cout << "\n edgebits size: " << 2*cols*rows-cols-rows << " bits\n";
    //std::cout << "regioncolors size: " << regionColors.size() << "\n";

    //std::cout << "compression rate: " << static_cast<double>(3*8*cols*rows) / (cols*rows + regionColors.size()*3*8 ) << std::endl;

    std::cout << "previous compression: " << static_cast<double>(3*8*cols*rows) / (2*cols*rows-cols-rows + regionColors.size()*3*8 ) << std::endl;
    std::cout << "current compression: " << static_cast<double>(3*8*cols*rows) / (3*std::count(edgeBits01.begin(), edgeBits01.end(), true) + 8*disconnectedComponents + regionColors.size()*3*8 ) << std::endl;
}

// Setter method to set the RGB value for a vertex
void Graph::setVertexColor(int v, int red, int green, int blue) {
    if (v >= 0 && v < vertices) {
        //std::cout << red << " - ";
        vertexColors[v].red = static_cast<std::uint8_t>(red);
        vertexColors[v].green = static_cast<std::uint8_t>(green);
        vertexColors[v].blue = static_cast<std::uint8_t>(blue);
        //std::cout << static_cast<int>(vertexColors[v].red) << ", ";
    } else {
        std::cout << "Invalid vertex index." << std::endl;
    }
}

// Getter method to access the RGB value for a vertex
RGB Graph::getVertexColor(int v) const {
    if (v >= 0 && v < vertices) {
        return vertexColors[v];
    } else {
        std::cout << "Invalid vertex index. Returning (0, 0, 0)." << std::endl;
        return {0, 0, 0};
    }
}


/**
 * @brief Compare two RGB colors for equality.
 *
 * This function compares two RGB colors by comparing their individual color components.
 *
 * @param color1 The first RGB color to compare.
 * @param color2 The second RGB color to compare.
 * @return true if the colors are equal, false otherwise.
 */
bool Graph::compareRGB(const RGB& color1, const RGB& color2) {
    return (color1.red == color2.red) && (color1.green == color2.green) && (color1.blue == color2.blue);
}

int Graph::countEdges() const {
    int edgeCount = 0;

    // Iterate through each vertex
    for (int v = 0; v < vertices; ++v) {
        // Increment the edge count by the number of adjacent vertices
        edgeCount += adjList[v].size();
    }

    // For an undirected graph, each edge is counted twice, so divide by 2
    return edgeCount / 2;
}

// Implement the setEdgeBit function
void Graph::setEdgeBit(int v, int w, bool value) {
    int edgeIndex = find(adjList[v].begin(), adjList[v].end(), w) - adjList[v].begin();
    
    if (edgeIndex < adjList[v].size()) {
        // If the edge exists, set the bit for the corresponding edge
        edgeBits[v][edgeIndex] = value; // changed from edgeBits[v][edgeIndex] = value;
    } else {
        // Handle the case where the edge does not exist
        std::cerr << "Edge does not exist." << std::endl;
    }
}

// Implement the getEdgeBit function
bool Graph::getEdgeBit(int v, int w) const {
    int edgeIndex = find(adjList[v].begin(), adjList[v].end(), w) - adjList[v].begin();

    if (edgeIndex < adjList[v].size()) {
        // If the edge exists, return the bit for the corresponding edge
        return edgeBits[v][edgeIndex];
    } else {
        // Handle the case where the edge does not exist
        std::cerr << "Edge does not exist." << std::endl;
        return false; // Or handle this case differently based on your requirements
    }
}

int Graph::getEdgeBitFromList(int v, int w, std::vector<bool>& edgebitsvector) const {
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


void Graph::setMulticut(){
    // Iterate through every pixel from top-left to bottom-right
    std::cout << "setting multicut\n";
    int pixel_index = 0;
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            // Access pixel value at position (x, y)
            cv::Vec3b pixel = img.at<cv::Vec3b>(y, x);

            // Access individual color channels
            uchar blue = pixel[0];
            uchar green = pixel[1];
            uchar red = pixel[2];


            //set rgb values for vertex
            //img_graph.setVertexColor(pixel_index, static_cast<int>(red), static_cast<int>(blue), static_cast<int>(green));
            setVertexColor(y*cols+x, static_cast<int>(red), static_cast<int>(green), static_cast<int>(blue));
            //std::cout << getVertexColor(y*cols+x).red << ", ";
            pixel_index++;
        }
        //printProgressBar(y, rows);

    }

    //std::cout << "\nsetting multicut bits\n";
    int regionIndex = 0;
    int edgeIndex = -1;
    // Check neighbors for every vertex and set multicut bits
    for (int v = 0; v < getVertices(); ++v) {
        RGB currentColor = getVertexColor(v);
        // get color of previous pixel (current pixel if there is no previous)
        RGB previousColor = getVertexColor(std::max(0,v-1));
        if (v == 0 || !compareRGB(currentColor, previousColor)) {
            //regionColors.push_back(currentColor);
            regionIndex++;
        }

        for (int offset : neighborsOffsets) {
            int neighbor = v + offset;
            if (neighbor >= 0 && neighbor < getVertices() && ((neighbor / cols == v / cols) || (abs(neighbor - v) > 1))) {
                addEdge(neighbor, v);
                if(offset == 1 || offset == cols){
                    //std::cout << edgeIndex << ", ";
                    edgeIndex++;
                    if(!compareRGB(currentColor, getVertexColor(neighbor))){
                        //set edgeBit01
                        edgeBits01[edgeIndex] = true;
                        //std::cout << "offset: " << offset << ", ";
                    }
                }

                //obsolete edgebit part 
                //TODO: delete weil obsolete?
                if(!compareRGB(currentColor, getVertexColor(neighbor))){
                    //std::cout << "comparing: " << static_cast<int>(currentColor.red) << " " << static_cast<int>(currentColor.blue) <<  " " << static_cast<int>(currentColor.green) <<std::endl;
                    //std::cout << "to: " << static_cast<int>(getVertexColor(neighbor).red) << " " << static_cast<int>(getVertexColor(neighbor).blue) << " " << static_cast<int>(getVertexColor(neighbor).green) << std::endl;
                    // ###### add multicut bit 
                    setEdgeBit(v,neighbor,1);
                }
            }
        }
        //printProgressBar(v, getVertices());
    }
    
    std::cout << "size edgebits01: " << edgeBits01.size() << std::endl;
    std::cout << "True edgebits: " << std::count(edgeBits01.begin(), edgeBits01.end(), true) << std::endl;
    std::cout << "percentage: " << 100*std::count(edgeBits01.begin(), edgeBits01.end(), true) / edgeBits01.size() << "%" << std::endl;
    /*
    std::cout << std::endl << "edge bits listed: ";
        for(bool edgebit : edgeBits01){
            std::cout << edgebit << ", ";
        }
        std::cout << "13th: " << edgeBits01[13] << std::endl;
    std::cout << "edge bit 7,2: " << getEdgeBitFromList(7,2) << std::endl;
    
    
    //std::cout << "edge bit between 17 and 18: " ;
    int edgeBit01Index = 0;
    */


    andres::Partition<int> multicutregion = getRegionsFromImage();
    //for(int i = 0; i<25; i++){
    //    std::cout << "region of index " << i << ": " << multicutregion.find(i) << ", ";
    //}

    std::vector<int> reps;
    multicutregion.representatives(std::back_inserter(reps));
    //std::cout << std::endl;
    for (int rep : reps) {
        //std::cout << rep << ", ";
        regionColors.push_back(getVertexColor(rep));
        //std::cout << static_cast<int>(repRGB.red) << ", ";
    }
    regionColors.resize(reps.size());
    /*
    for(RGB rgb : regionColors){
        std::cout << static_cast<int>(rgb.red) << " " << static_cast<int>(rgb.green) << " " << static_cast<int>(rgb.blue) << std::endl;
    }
    */
    
    /*
    labelRegions();
   for (int label : regionRepresentatives) {
    std::cout << label << " ";
    }
    std::cout << std::endl;
    */


   // DFS iterative
    //std::stringstream strstream = dfs_paths(4);
    //std::cout << strstream.str() << std::endl;


    // loop all edges in edgeBits01
    // if edge == multicut edge -> start dfs

    //visited.assign(visited.size(), false);
    int edgeI = 0;
    int dfsI = 0;
    
    for (bool edge : edgeBits01){
        // skip non-multicut edges or previously visited edges ||
        if(!edge){
            edgeI++;
            continue;
            }
        if(visited[edgeI]){
            //std::cout << "visited" << std::endl;
            edgeI++;
            continue;
        }
        //std::cout << std::endl << edgeI;

        // dir rausfinden
        Direction horizontalDir = Direction::DOWN;
        Direction verticalDir = Direction::RIGHT;

        int row = edgeI / (2*cols-1);
        int col = edgeI % (2*cols-1);

        if(edgeI>2*cols*rows-cols-rows-cols){
            verticalDir = Direction::UP;
        }else{
            verticalDir = Direction::DOWN;
        }
        if((edgeI+1) % (2*cols-1) == 0){
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
        directionVector = dfs_paths_iterative(edgeI, currentDir, visited);
        paths.emplace_back(edgeI, currentDir, directionVector);
        //std::cout << "path size: " << directionVector.size() << std::endl;
        
        //std::cout << std::endl;
        //for(bool v : visited){
        //    std::cout << v;
        //}
        //std::cout << std::endl;
        edgeI++;
    }

    //printPaths();
    
    /*
    for (bool edge : edgeBits01){
        std::cout << edge << " ";
    }
    */
    
    
    
    disconnectedComponents = dfsI;
    std::cout << "\nnumber of disconnected components: " << dfsI << std::endl;
    /*
    // DFS recursive
    Direction currentDir = Direction::RIGHT;
    //std::vector<bool> visited;
    //visited.resize(cols*rows, false);
    std::vector<bool> directionVector; 
    directionVector = dfs_paths_recursive(33841, visited, currentDir, directionVector);
    std::cout << std::endl;
    for(const auto& dir : directionVector){
        std::cout << dir;
    }    
    std::cout << std::endl << directionVector.size() << std::endl;
    
    std::cout << "mc edges count: " << std::count(edgeBits01.begin(), edgeBits01.end(), true) << std::endl;
    std::cout << "visited count: " << std::count(visited.begin(), visited.end(), true) << std::endl;
    */
    
    

}

std::vector<int> Graph::getConnectedPixels(int seed) {
    std::vector<int> connectedPixels;
    std::stack<int> stack;
    std::set<int> visited;

    stack.push(seed);
    
    RGB seedColor = getVertexColor(seed);

    while (!stack.empty()) {
        int current = stack.top();
        stack.pop();


        if (visited.count(current) == 0) {
            visited.insert(current);

            // Check if the current pixel has the same color as the seed
            if (compareRGB(seedColor, getVertexColor(current))) {
                connectedPixels.push_back(current);
                
                // Add unvisited neighbors to the stack
                for (int neighbor : adjList[current]) {
                    if (visited.count(neighbor) == 0) {
                        stack.push(neighbor);
                    }
                }
            }
        }
    }

    return connectedPixels;
}

void Graph::printConnectedPixels(int seed) {
    std::vector<int> connectedPixels = getConnectedPixels(seed);

    std::cout << "Connected pixels to seed pixel " << seed << " with the same color:" << std::endl;

    for (int pixel : connectedPixels) {
        RGB color = getVertexColor(pixel);
        std::cout << "Pixel " << pixel << ": RGB(" << color.red<< ", " << color.green << ", " << color.blue << ")" << std::endl;
    }
}

void Graph::findAllRegions() {
std::cout << "\nfinding regions \n";
    for (int v = 0; v < getVertices(); ++v) {
        if (v % 100 == 0) {
            printProgressBar(v, getVertices());
            //std::cout << "Current Region vertex: " << v << " / " << getVertices() << ", Progress: " << 100*v/getVertices() << "%" << std::endl;
        }
        // Check if the current pixel is already part of any region
        bool isInRegion = false;
        //std::unordered_set<int> visitedPixels;
                

        for (const auto& region : regions) {
            //visitedPixels.insert(region.begin(), region.end());
            if (std::find(region.begin(), region.end(), v) != region.end()) {
            //if (visitedPixels.find(v) != visitedPixels.end()){
                isInRegion = true;
                break;
            }
        }

        // If not, find connected pixels and create a new region
        if (!isInRegion) {
            std::vector<int> connectedPixels = getConnectedPixels(v);
            regions.push_back(connectedPixels);

            // Get the color of the current pixel and add it to regionColors
            regionColors.push_back(getVertexColor(v));
        }
    }
}

void Graph::assignRegions() {
    std::vector<int> visited(vertices, 0);
    int currentRegion = 0;

    for (int v = 0; v < vertices; ++v) {
        if (visited[v] == 0) {
            std::vector<int> connectedPixels = getConnectedPixels(v);

            // Assign the current region index to all connected pixels
            for (int pixel : connectedPixels) {
                vertexRegions[pixel] = currentRegion;
                visited[pixel] = 1; // Mark as visited to avoid redundant processing
            }

            RGB currentColor = getVertexColor(v);
            regionColors.push_back({currentColor});

            //printProgressBar(v, vertices);

            // Move to the next region
            currentRegion++;
        }
    }
    
    /*
    std::cout << "regions: "; 
    for (auto &reg : vertexRegions){
        std::cout << reg << ", ";
    }
    */
    

    /*
    std::set<int> uniqueRegions(vertexRegions.begin(), vertexRegions.end());
    std::cout << "Unique Region Indices: ";
    for (int region : uniqueRegions) {
        std::cout << region << " ";
    }
    std::cout << std::endl;
    std::cout << "number of pixels: " << getVertices() << std::endl;
    */
    
    
   
}

void Graph::printRegionVector(){
    // Print the regions
    std::cout << "region vector: \n";
    
    for (int i = 0; i < regions.size(); ++i) {
        std::cout << "Region " << i << "(size: " << sizeof(regions[i]) << "bit)" << "Pixels: [ ";
        for (int j = 0; j < regions[i].size(); ++j) {
            std::cout << regions[i][j];
            if (j < regions[i].size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << " ]" << std::endl;
    }
    
}

int Graph::findVectorIndex(const std::vector<std::vector<int>>& regions, int targetNumber) {
    auto it = std::find_if(regions.begin(), regions.end(),
        [targetNumber](const std::vector<int>& innerVector) {
            return std::find(innerVector.begin(), innerVector.end(), targetNumber) != innerVector.end();
        }
    );

    if (it != regions.end()) {
        // Calculate the index of the outer vector
        return std::distance(regions.begin(), it);
    } else {
        // Return -1 if the target number is not found in any vector
        return -1;
    }
}

bool Graph::areImagesIdentical(const cv::Mat& image1, const cv::Mat& image2) {
    // Check if the images have the same size and type
    if (image1.size() != image2.size() || image1.type() != image2.type()) {
        return false;
    }

    // Convert the images to grayscale
    cv::Mat grayImage1, grayImage2;
    cv::cvtColor(image1, grayImage1, cv::COLOR_BGR2GRAY);
    cv::cvtColor(image2, grayImage2, cv::COLOR_BGR2GRAY);

    // Check if there are any non-zero elements (differences) between the images
    return cv::countNonZero(grayImage1 != grayImage2) == 0;
}

cv::Mat Graph::generateRandomImage(int width, int height) {
    // Ensure random values are different in each run
    std::srand(std::time(0));

    // Create an empty image matrix
    cv::Mat randomImage(height, width, CV_8UC3);

    // Fill the image with random RGB values
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Generate random RGB values
            uchar blue = std::rand() % 256;
            uchar green = std::rand() % 256;
            uchar red = std::rand() % 256;

            // Set the pixel value
            randomImage.at<cv::Vec3b>(y, x) = cv::Vec3b(blue, green, red);
        }
    }

    return randomImage;
}

void Graph::printProgressBar(int progress, int total) {
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

void Graph::unionFindMulticut(){
    andres::Partition<> unionFind(10); 
    unionFind.merge(0,1);
    std::cout << "rep of 1: " << unionFind.find(1) << std::endl; 
}

/*
void Graph::reconstructImage(){
    cv::Mat image(rows, cols, CV_8UC3, cv::Scalar(0, 0, 0)); 
    std::cout << "\nreconstructing image..." << std::endl;
    for (int index = 0; index < rows * cols; ++index) {
        // Calculate row and column indices from the linear index
        int y = index / cols;
        int x = index % cols;
        if(index%100 == 0){
            //std::cout << "Current Rec Index: " << index << " / " << rows*cols << std::endl;
            printProgressBar(index, rows*cols);

        }

        // get color
        RGB col = regionColors[findVectorIndex(regions, index)];
        //RGB col = getVertexColor(0);
        //std::cout << col.green.to_ulong() << std::endl;
        // Set the color (BGR format)
        image.at<cv::Vec3b>(y, x) = cv::Vec3b(col.blue.to_ulong(), col.green.to_ulong(), col.red.to_ulong());  
    }
    cv::imshow("Original", img);
    cv::imshow("Reconstruction", image);
    cv::waitKey(0);
    std::cout << std::endl << areImagesIdentical(img, image) << std::endl;
    
    
    
    //printSize();
    //printColorRegions();
    //printSize();
    std::cout << "size of regioncolors vector: " << regionColors.size() << " entries (" << getVertices() << " total pixels) * 3 * 8 bit = " << regionColors.size()*24 << "bit\n";
    //printRegionVector();
    //std::cout << "size of regions vector: " << region
    //std::cout << "size of reconstruction: " << 
    //std::cout << findVectorIndex(regions, 17) << std::endl;
}
*/

void Graph::reconstructImage(){
    //std::cout << "\n reconstructing image\n";
    cv::Mat image(rows, cols, CV_8UC3, cv::Scalar(0, 0, 0)); 
    for (int index = 0; index < rows * cols; ++index) {
        // Calculate row and column indices from the linear index
        int y = index / cols;
        int x = index % cols;
        if(index%100 == 0){
            //printProgressBar(index, rows*cols);
        }
        // get color
        RGB col = regionColors[vertexRegions[index]];
        //RGB col = getVertexColor(0);
        //std::cout << col.green.to_ulong() << std::endl;
        // Set the color (BGR format)
        image.at<cv::Vec3b>(y, x) = cv::Vec3b(col.blue, col.green, col.red);  
    }
    printSize();
    cv::imshow("Original", img);
    cv::imshow("Reconstruction", image);
    cv::waitKey(0);
}

void Graph::labelRegions() {
    std::queue<int> pixelQueue;
    int regionNumber = 0;

    // Iterate over all pixels
    for (int index = 0; index < rows * cols; ++index) {
        if (regionRepresentatives[index] == -1) {
            // Found an unlabeled pixel
            pixelQueue.push(index);

            while (!pixelQueue.empty()) {
                int currentPixel = pixelQueue.front();
                pixelQueue.pop();

                // Label the current pixel with the region number
                regionRepresentatives[currentPixel] = regionNumber;

                // Get color of current pixel
                RGB currentColor = getVertexColor(currentPixel);

                // Check neighbors
                for (int offset : neighborsOffsets) {
                    int neighbor = currentPixel + offset;
                    if (isValidNeighbor(neighbor) && regionRepresentatives[neighbor] == -1) {
                        // Check if neighbor has same color as current pixel
                        //TODO: check if multicut runs between vertices/pixels 
                        RGB neighborColor = getVertexColor(neighbor);
                        if (compareRGB(currentColor, neighborColor)) {
                            // Enqueue neighbor for processing
                            pixelQueue.push(neighbor);
                        }
                    }
                }
            }

            // Increment region number for the next region
            regionNumber++;
        }
    }
}

bool Graph::isValidNeighbor(int neighbor) const {
    return neighbor >= 0 && neighbor < rows * cols;
}

andres::Partition<int> Graph::getRegions(std::vector<bool>& edgeBitsVector){
    andres::Partition<int> region(rows*cols);
    for (int index = 0; index < rows * cols; ++index) {
        for (int offset : neighborsOffsets) {
            int neighbor = index + offset;
            if (neighbor >= 0 && neighbor < getVertices() && ((neighbor / cols == index / cols) || (abs(neighbor - index) > 1))) {
                // check if neighbours are separated by multicut 
                // if no: merge 

                if(getEdgeBitFromList(index, neighbor, edgeBitsVector) == 0){
                    region.merge(index, neighbor);
                    //std::cout << reconstruction.find(index) << ", ";
                    //std::cout << "merged " << index << " and " << neighbor << std::endl;
                }

                
            }
        }
    }
    return region;
}

andres::Partition<int> Graph::getRegionsFromImage(){
    andres::Partition<int> region(rows*cols);
    for (int index = 0; index < rows * cols; ++index) {
        for (int offset : neighborsOffsets) {
            int neighbor = index + offset;
            if (neighbor >= 0 && neighbor < getVertices() && ((neighbor / cols == index / cols) || (abs(neighbor - index) > 1))) {
                // check if neighbours are separated by multicut 
                // if no: merge 
                //TODO: check WITHOUT edgeBits01
                if(compareRGB(getVertexColor(index), getVertexColor(neighbor))){
                    region.merge(index, neighbor);
                    //std::cout << reconstruction.find(index) << ", ";
                }

                
            }
        }
    }
    return region;
}


void Graph::dfs_multicut_path(int v, std::vector<bool>& visited, std::vector<int>& path, std::vector<std::pair<int, uint8_t>>& pathDirections) {
    visited[v] = true;
    path.push_back(v); // Add vertex to the current path

    // Check neighboring vertices connected by multicut edges
    int numVertices = visited.size();
    for (int u = 0; u < numVertices; ++u) {
        int edgeIndex = v * numVertices + u;
        if (edgeBits01[edgeIndex] && !visited[u]) {
            // u -> v edge direction  
            int delta = u - v;
            uint8_t dir;
            if (delta == -1) {
                dir = 0b11; 
            } else if (delta == 1) {
                dir = 0b01; 
            } else if (delta == -cols) {
                dir = 0b00; 
            } else if (delta == cols) {
                dir = 0b10;
            } else {
                // Handle invalid edge
                continue;
            }

            // Store the edge index and direction in the path
            pathDirections.emplace_back(v, dir);
            dfs_multicut_path(u, visited, path, pathDirections);
        }
    }
}


std::vector<std::vector<std::pair<int, uint8_t>>> Graph::extract_multicut_paths() {
    int numVertices = rows*cols;
    std::vector<bool> visited(numVertices, false); // Track visited vertices
    std::vector<std::vector<std::pair<int, uint8_t>>> paths; // Store the extracted paths

    // Iterate through all vertices and start DFS from unvisited vertices
    for (int v = 0; v < numVertices; ++v) {
        if (!visited[v]) {
            std::vector<int> path;
            std::vector<std::pair<int, uint8_t>> pathDirections;
            dfs_multicut_path(v, visited, path, pathDirections);
            if (!pathDirections.empty()) {
                paths.push_back(pathDirections); // Store the extracted path directions
            }
        }
    }

    return paths;
}



bool Graph::checkHorizontal(int current){
    // check if horizontal or vertical
    bool horizontal = false;
    int row = current/(2*cols-1);
    if(row % 2 == 0){
        horizontal = current % 2 == 0;
    }else{
        horizontal = (current+1) % 2 == 0;
    }
    return horizontal;
}

std::stringstream Graph::dfs_paths(int edge){
    Direction dir;

    dir = checkHorizontal(edge) ? Direction::RIGHT : Direction::DOWN;

    std::vector<bool> visited(edgeBits01.size(), false);

    std::stack<int> stack; 
    
    std::stringstream strstream;

    visited[edge] = true;

    stack.push(edge);

    std::vector<int> horizontalEdgeOffsets;
    std::vector<int> verticalEdgeOffsets;

    while(!stack.empty()){
        int current = stack.top();
        stack.pop();
        //strstream << "[" << current;

        //std::cout << "current: " << current /*<< " - " << edgeBits01[current] */ << std::endl;

        // get neighbors 
        // dimensions -> index -> neighbor index ausrechnen

        horizontalEdgeOffsets = {-1,-2,2*(cols-1), 1, 2, 2*(cols-1)+2}; 
        verticalEdgeOffsets = {-2*(cols-1), -2*(cols-1)-1, -2*(cols-1)-2, -1,1,2*cols-1};

        bool horizontal = checkHorizontal(current);

        //std::cout << "horizontal: " << horizontal << std::endl;

        // TODO: rand cases beachten 
        //if(!horizontal && current <= 2*cols-1){verticalEdgeOffsets={-1,1,2*cols-1};}
        //if(!horizontal && current > 2*cols-1){verticalEdgeOffsets={-1,1,2*cols-1};}
        // first column horizontal
        if(horizontal && current % (2*cols-1) == 0){horizontalEdgeOffsets = {1, 2, 2*(cols-1)+2};}
        // last column horizontal
        if(horizontal && current+1 % (2*cols-1) == 0){horizontalEdgeOffsets = {-1,-2,2*(cols-1)};}



        for (int offset : (horizontal ? horizontalEdgeOffsets : verticalEdgeOffsets)){
            int neighbor = offset + current;
            // zu queue hinzufügen, wenn noch nicht visited + wenn multicut edge 
            if(neighbor >= 0 && neighbor < edgeBits01.size() && edgeBits01[neighbor]){
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    stack.push(neighbor);
                    //std::cout << "pushed " << neighbor << " to stack." << std::endl;
                }
            }   
            
        }
        //strstream << "]";


    }
    
    std::cout << "stack empty" << std::endl;

    /*
    for (bool edge : edgeBits01){
        std::cout << edge << " ";
    }
    */
   //visited[edge] = true;
   //bool isVertical = 

    // check neighbors

    // horizontal or vertical? 
    // index -> column -> even number? -> 



    
    return strstream;
}

std::string Graph::directionToString(Direction dir) const {
    static const std::unordered_map<Direction, std::string> directionMap = {
        {Direction::UP, "UP"},
        {Direction::DOWN, "DOWN"},
        {Direction::LEFT, "LEFT"},
        {Direction::RIGHT, "RIGHT"}
    };
    
    auto it = directionMap.find(dir);
    if (it != directionMap.end()) {
        return it->second;
    } else {
        return "Unknown";
    }
}

std::vector<bool> Graph::dfs_paths_recursive(int currentEdge, std::vector<bool>& visited, Direction currentDir, std::vector<bool>& directionVector){
    //std::cout << currentEdge << std::endl;
    visited[currentEdge] = true; 
    //std::cout << currentEdge << ", ";

    //std::cout << "current: " << currentEdge << std::endl; 

    //TODO: segmentation fault bei A_House_in_California
    
    // immediately return for edge cases
    if((currentDir == Direction::LEFT && (currentEdge % (2*cols-1) == 0)) || 
    currentDir == Direction::RIGHT && ((currentEdge+1) % (2*cols-1) == 0)){
        //std::cout << "edge case return\n"; 
        directionVector.push_back(0);
        directionVector.push_back(0);
        directionVector.push_back(0);
        return directionVector;
    }
    /*
    if((currentDir == Direction::LEFT || currentDir == Direction::RIGHT) && 
    ((currentEdge / (2*cols - 1)) != ((currentEdge-1) / (2*cols - 1)) ||
    (currentEdge / (2*cols - 1)) != ((currentEdge+1) / (2*cols - 1)))){
        //std::cout << "edge case" << std::endl;
        return directionVector; 
    }
    */
    
    if((currentDir == Direction::UP) && 
    (currentEdge < 2*cols-1)){
        //std::cout << "edge case" << std::endl;
        directionVector.push_back(0);
        directionVector.push_back(0);
        directionVector.push_back(0);
        return directionVector; 
    }
    if((currentDir == Direction::DOWN) && 
    (currentEdge + cols-1 >= (2*cols*rows-cols-rows))){
        //std::cout << "edge case down: " << currentEdge << std::endl;
        directionVector.push_back(0);
        directionVector.push_back(0);
        directionVector.push_back(0);
        return directionVector; 
    }

    std::vector<int> edgeOffsets;

    std::bitset<3> directionBits;

    bool left, forward, right = false; 

    int row = currentEdge/(2*cols-1) + 1;
    int column = (currentEdge % (2*cols-1)) / 2;
    //std::cout << "row: " << row << std::endl;
    //std::cout << "column: " << column << std::endl;

    //based on current direction check next edges and set corresponding direction bits (e.g. 011 for paths to front and right)

    edgeOffsets = {getNeighbor(currentEdge, currentDir, 0)-currentEdge, getNeighbor(currentEdge, currentDir, 1)-currentEdge, getNeighbor(currentEdge, currentDir, 2)-currentEdge};
    

    // checken in welche richtungen es weiter geht 
    if(!visited[getNeighbor(currentEdge, currentDir, 0)]){
        left = edgeBits01[getNeighbor(currentEdge, currentDir, 0)];
        //std::cout << "left: " << currentEdge + edgeOffsets[0] << std::endl;
    }
    if(!visited[getNeighbor(currentEdge, currentDir, 1)]){
        forward = edgeBits01[getNeighbor(currentEdge, currentDir, 1)];
        //std::cout << "forward: " << currentEdge + edgeOffsets[1] << std::endl;
    }
    if(!visited[getNeighbor(currentEdge, currentDir, 2)]){
        right = edgeBits01[getNeighbor(currentEdge, currentDir, 2)];   
        //std::cout << "right: " << currentEdge + edgeOffsets[2] << std::endl;
    }


    

    directionVector.push_back(left);
    directionVector.push_back(forward);
    directionVector.push_back(right);

    //std::cout << "pushed direction: " << left << forward << right << std::endl;

    if(left){directionBits.set(2);}
    if(forward){directionBits.set(1);}
    if(right){directionBits.set(0);}

    //std::cout << "direction: " << directionBits.to_string() << std::endl;

    int edge012Index = 0;
    Direction nextDir;
    
    for (int offset : edgeOffsets){
            // TODO: neighbor anders berechnen wenn direction DOWN und neighbor auf letzter row
            int neighbor = offset + currentEdge;
            //std::cout << "current neighbor for " << currentEdge << ": " << neighbor << std::endl;
            // zu queue hinzufügen, wenn noch nicht visited + wenn multicut edge 
            if(neighbor >= 0 && neighbor < edgeBits01.size() && edgeBits01[neighbor]){
                if (!visited[neighbor]) {
                    //std::cout << "neighbour " << neighbor << " considered" << std::endl;
                    //which direction is the edge facing? 
                    // dependet on: current direction and current edgeOffset
                    //std::cout << "current dir: " << directionToString(currentDir) << std::endl;
                    if(edge012Index == 0){
                        nextDir = static_cast<Direction>((static_cast<int>(currentDir) + 3 ) % 4);
                        //std::cout << "dir calc0: " << directionToString(static_cast<Direction>((static_cast<int>(currentDir) + 3 ) % 4)) << std::endl;
                    }
                    if(edge012Index == 1){
                        nextDir = static_cast<Direction>(static_cast<int>(currentDir));
                        //std::cout << "dir calc1: " << static_cast<int>(currentDir) << std::endl;
                    }
                    if(edge012Index == 2){
                        nextDir = static_cast<Direction>((static_cast<int>(currentDir) + 1 ) % 4);
                        //std::cout << "dir calc2: " << (static_cast<int>(currentDir) + 1 ) % 4 << std::endl;

                    }
                    //std::cout << "next dir: " << directionToString(nextDir) << std::endl;
                    //std::cout << "next edge: " << neighbor << std::endl;

                    dfs_paths_recursive(neighbor, visited, nextDir, directionVector);
                }
            }   
            edge012Index++;
        }
    //std::cout << "finished edge offsets" << std::endl;

    return directionVector;
}

std::vector<bool> Graph::dfs_paths_iterative(int currentEdge, Direction currentDir, std::vector<bool>& visited){
    std::vector<bool> directionVector;
    //std::vector<bool> visited(edgeBits01.size(), false);
    std::stack<std::pair<int, Direction>> pendingEdges;
    while(!pendingEdges.empty()){
        pendingEdges.pop();
    }
    pendingEdges.push(std::make_pair(currentEdge, currentDir));
    visited[currentEdge] = true;
    bool left, front, right;
    while(!pendingEdges.empty()){
        left = false;
        front = false;
        right = false;
        std::tie(currentEdge, currentDir) = pendingEdges.top();
        //visited[currentEdge] = true;
        pendingEdges.pop();
        //std::cout << "current edge: " << currentEdge << std::endl;


        //TODO: push 000 und continue falls nächste edges nicht multicut oder schon visited 
        if(getNeighbor(currentEdge, currentDir, 0) == -1){
            directionVector.push_back(0);
            directionVector.push_back(0);
            directionVector.push_back(0);
            //std::cout << "000" << std::endl;
            continue;
        }


        if(!visited[getNeighbor(currentEdge, currentDir, 0)]){
            left = edgeBits01[getNeighbor(currentEdge, currentDir, 0)];
            directionVector.push_back(left);
        }else{
            directionVector.push_back(false);
        }
        if(!visited[getNeighbor(currentEdge, currentDir, 1)]){
            front = edgeBits01[getNeighbor(currentEdge, currentDir, 1)];
            directionVector.push_back(front);
        }
        else{
            directionVector.push_back(false);
        }
        if(!visited[getNeighbor(currentEdge, currentDir, 2)]){
            right = edgeBits01[getNeighbor(currentEdge, currentDir, 2)];
            directionVector.push_back(right);
        }
        else{
            directionVector.push_back(false);
        }

        //std::cout << left << front << right << std::endl;

        bool lastThreeAllFalse = false;
        if (directionVector.size() >= 3) {
            lastThreeAllFalse = !directionVector[directionVector.size() - 1] &&
                                !directionVector[directionVector.size() - 2] &&
                                !directionVector[directionVector.size() - 3];
        }

        if(!left && !front && !right && !lastThreeAllFalse){
            directionVector.push_back(0);
            directionVector.push_back(0);
            directionVector.push_back(0);
            continue;
        }

        if(right){
            if(!visited[getNeighbor(currentEdge, currentDir, 2)]){
                pendingEdges.push(std::make_pair(getNeighbor(currentEdge, currentDir, 2), nextDirection(currentDir)));
                visited[getNeighbor(currentEdge, currentDir, 2)] = true;
            }
        }
        if(front){
            if(!visited[getNeighbor(currentEdge, currentDir, 1)]){
                pendingEdges.push(std::make_pair(getNeighbor(currentEdge, currentDir, 1), currentDir));
                visited[getNeighbor(currentEdge, currentDir, 1)] = true;
            }
        }
        if(left){
            if(!visited[getNeighbor(currentEdge, currentDir, 0)]){
                pendingEdges.push(std::make_pair(getNeighbor(currentEdge, currentDir, 0), previousDirection(currentDir)));
                visited[getNeighbor(currentEdge, currentDir, 0)] = true;
            }
        }

    }
    return directionVector;
}

void Graph::printPaths() const {
    for (const auto& pathInfo : paths) {
        int startEdge = std::get<0>(pathInfo);
        Direction startDir = std::get<1>(pathInfo);
        const std::vector<bool>& directions = std::get<2>(pathInfo);

        std::cout << "Start Edge: " << startEdge << std::endl;
        std::cout << "Start Direction: " << directionToString(startDir) << std::endl;
        std::cout << "Directions: ";
        int i = 0;
        for (bool dir : directions) {
            if(i%3 == 0 && i != 0 && i != directions.size()){
                std::cout << " - ";
            }
            std::cout << dir;
            i++;
        }
        std::cout << std::endl;
        std::cout << "Path Length (/3): " << directions.size() / 3 << std::endl;
    }
}

void Graph::reconstruct_edgeBits_iterative(int currentEdge, Direction currentDir, std::vector<bool>& directionVector, std::vector<bool>& reconstructedEdgeBits){
    std::vector<bool> reconstruction(edgeBits01.size(), false);
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
            pendingEdges.push(std::make_pair(getNeighbor(currentEdge, currentDir, 2), nextDirection(currentDir)));
        }
        if(forward){
            pendingEdges.push(std::make_pair(getNeighbor(currentEdge, currentDir, 1), currentDir));
        }
        if(left){
            pendingEdges.push(std::make_pair(getNeighbor(currentEdge, currentDir, 0), previousDirection(currentDir)));
        }
    }
    return;
    //return reconstruction;
}




int Graph::getNeighbor(int currentEdge, Direction currentDir, int neighborIndex){
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
    return currentEdge + edgeOffsets[neighborIndex];
}

Direction Graph::nextDirection(Direction dir) {
    return static_cast<Direction>((static_cast<int>(dir) + 1) % 4);
}

Direction Graph::previousDirection(Direction dir) {
    int newDir = (static_cast<int>(dir) - 1) % 4;
    if (newDir < 0) newDir += 4;  // Handle negative wrap-around
    return static_cast<Direction>(newDir);
}

double Graph::reconstructMulticut(){
    cv::Mat image(rows, cols, CV_8UC3, cv::Scalar(0, 0, 0)); 
    int directionBitsSize;
    int numberOfPaths = paths.size();

    std::vector<bool> reconstructed_edgeBits(edgeBits01.size(), false);
    int i;
    for(PathInfo pathinfo : paths){
        //std::cout << directionToString(std::get<1>(pathinfo)) << std::endl;
        //printProgressBar(i , paths.size());
        //reconstructed_edgeBits = logical_or_vectors(reconstructed_edgeBits,reconstruct_edgeBits_iterative(std::get<0>(pathinfo), std::get<1>(pathinfo), std::get<2>(pathinfo)));
        //reconstructed_edgeBits = reconstruct_edgeBits_iterative(std::get<0>(pathinfo), std::get<1>(pathinfo), std::get<2>(pathinfo));
        reconstruct_edgeBits_iterative(std::get<0>(pathinfo), std::get<1>(pathinfo), std::get<2>(pathinfo), reconstructed_edgeBits);
        directionBitsSize += std::get<2>(pathinfo).size();
        //break;
        i++;
    }

    std::cout << "size of directionbits: " << directionBitsSize << std::endl;

    std::cout << "reconstruction same as original: " << (reconstructed_edgeBits == edgeBits01) << std::endl;

    std::cout << "\nreconstruction for edgebits01 finished" << std::endl;

    //reconstructed_edgeBits.assign(reconstructed_edgeBits.size(), false);

    andres::Partition<int> reconstruction = getRegions(reconstructed_edgeBits);
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
    
    printSize();
    
    /*
    cv::destroyAllWindows();
    cv::imshow("Original", img);
    cv::imshow("Reconstruction", image);
    cv::waitKey(0);
    */
    
    
    
    
    std::cout << "\nImages Are Identical: " << (areImagesIdentical(img, image) ? "YES" : "NO") << std::endl;
    
    
    // reset visited vector for reconstruction dfs
    visited.assign(visited.size(), false);

    //edgeBits01.assign(edgeBits01.size(), false);

    /*
    std::cout << "edgeBits01 before reconstruction:" << std::endl;
    for(bool edge : edgeBits01){
        if(edge){std::cout << "1";}else{std::cout << "0";}
    }
    
    */
    

    
    /*
    for (int edge = 0; edge <=39; edge++){
            for (Direction dir : {Direction::UP, Direction::RIGHT, Direction::DOWN, Direction::LEFT}){
                for(int neighborI = 0; neighborI <= 2; neighborI++){
                    std::cout << "neighbor " << neighborI << " of edge " << edge << ", " << directionToString(dir) << ": " << getNeighbor(edge, dir, neighborI) << std::endl;
                }
            }
        }
    */
    
    
    //std::cout << "init reconstruct" << std::endl;
    //std::cout << "UP - 1: " << directionToString(previousDirection(Direction::UP)) << std::endl;

    

    /*
    for(bool edge : old_edgeBits01){
            if(edge){std::cout << "1";}else{std::cout << "0";}
    }   
    std::cout << std::endl;
    for(bool edge : edgeBits01){
            if(edge){std::cout << "1";}else{std::cout << "0";}
    }   
    */
    
    
    
    
    
    
    //std::cout << std::endl << "reconstruction for '" << imagePath.substr(imagePath.find_last_of("/\\") + 1) << "' finished: " << (reconstructed_edgeBits == edgeBits01) << std::endl;

    
    

    

    /*
    std::vector<std::vector<std::pair<int, uint8_t>>> multicutPaths = extract_multicut_paths();

    // Display the extracted multicut paths
    for (const auto& path : multicutPaths) {
        std::cout << "Multicut Path: ";
        for (const auto& edge : path) {
            std::cout << "(" << edge.first << ", " << static_cast<int>(edge.second) << ") ";
        }
        std::cout << std::endl;
    }
    */
    
    // size of representation (edgeBits count + region colors vector size in bits)
    //std::cout << std::endl << ((2*cols*rows-cols-rows) + regionColors.size()*3*8 ) << std::endl;

    int edgeCount = 0;
    for (bool edge : edgeBits01){
        if(edge){edgeCount++;}
    }


    /*
    std::cout << "edges in multicut: " << edgeCount << std::endl;
    std::cout << "edges not in multicut: " << (2*cols*rows-cols-rows) - edgeCount << std::endl;
    std::cout << "size of regions array: " << regionColors.size() << std::endl;
    */
    
    // compression rate for paths representation 
    return static_cast<double>(3*8*cols*rows) / (directionBitsSize + numberOfPaths*(8 + 2) + regionColors.size()*3*8 );
    // compression rate for edgebit representation
    //return static_cast<double>(3*8*cols*rows) / ((2*cols*rows-cols-rows) + regionColors.size()*3*8 );

}

std::vector<bool> Graph::logical_or_vectors(const std::vector<bool>& vec1, const std::vector<bool>& vec2) {
    // Ensure the vectors are the same size
    if (vec1.size() != vec2.size()) {
        throw std::invalid_argument("Vectors must be the same size");
    }

    std::vector<bool> result(vec1.size());
    std::transform(vec1.begin(), vec1.end(), vec2.begin(), result.begin(), [](bool a, bool b) { return a || b; });

    return result;
}
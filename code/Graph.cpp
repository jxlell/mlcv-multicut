// Graph.cpp
#include "Graph.h"
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <stack>
#include <unordered_set>

Graph::Graph(const std::string& imagePath) {
    

    //img = cv::imread(imagePath);
    img = cv::imread(imagePath, /*cv::IMREAD_UNCHANGED*/ cv::IMREAD_COLOR);
    //img = generateRandomImage(39,29);

    if (img.empty()) {
        std::cerr << "Error: Could not read the image." << std::endl;
    }
    this->cols = img.cols;
    this->rows = img.rows;
    this->neighborsOffsets = {-cols, cols, -1, 1};
    vertices = rows*cols;

    this->imagePath = imagePath;
    adjList.resize(vertices);
    vertexValues.resize(vertices, 0);
    vertexColors.resize(vertices);
    //TODO: init size richtig
    edgeBits.resize(2*vertices, std::bitset<1>(0));
    edgeBitsRLE.resize(2*vertices, std::bitset<1>(0));
}

int Graph::getVertices() const{
    return vertices;
}

std::string Graph::getImagePath() const{
    return imagePath;
}


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
        std::cout << std::endl << "Color (RGB): " << color.red.to_ulong() << ", " << color.green.to_ulong() << ", " << color.blue.to_ulong()
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
        std::cout << "RGB: " << color.red.to_ulong() << ", " << color.green.to_ulong() << ", " << color.blue.to_ulong() << std::endl;
    }
    
}

void Graph::printSize(){
    std::cout << "size of regionColors " << "(" << regionColors.size() << " entries): " << sizeof(regionColors) * 8 << std::endl;
    //std::cout << sizeof(getVertexColor(0))<< std::endl;
    std::cout << "size of regions vector: " << sizeof(regions) * 8 << std::endl;
    std::cout << "total: " << sizeof(regionColors) * 8 + sizeof(regions) * 8 << std::endl;
    std::cout << "size of one RGB value: " << sizeof(getVertexColor(0)) * 8 << std::endl;
    RGB test;
    std::cout << "size of one test RGB value: " << sizeof(test) * 8 << std::endl;



}

// Setter method to set the RGB value for a vertex
void Graph::setVertexColor(int v, int red, int green, int blue) {
    if (v >= 0 && v < vertices) {
        vertexColors[v].red = std::bitset<8>(red);
        vertexColors[v].green = std::bitset<8>(green);
        vertexColors[v].blue = std::bitset<8>(blue);
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
        return {std::bitset<8>(0), std::bitset<8>(0), std::bitset<8>(0)};
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
        edgeBits[v] = value; // changed from edgeBits[v][edgeIndex] = value;
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

void Graph::setMulticut(){
    // Iterate through every pixel from top-left to bottom-right
    std::cout << "setting colors\n";
    int pixel_index = 0;
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            // Access pixel value at position (x, y)
            cv::Vec3b pixel = img.at<cv::Vec3b>(y, x);

            // Access individual color channels
            uchar blue = pixel[0];
            uchar green = pixel[1];
            uchar red = pixel[2];

            //std::cout << "Pixel at (" << x << ", " << y << "): ";
            //cout << "B: " << static_cast<int>(blue) << ", ";
            //cout << "G: " << static_cast<int>(green) << ", ";
            //cout << "R: " << static_cast<int>(red) << endl;

            //set rgb values for vertex
            //img_graph.setVertexColor(pixel_index, static_cast<int>(red), static_cast<int>(blue), static_cast<int>(green));
            setVertexColor(y*cols+x, static_cast<int>(red), static_cast<int>(green), static_cast<int>(blue));
            pixel_index++;
        }
        printProgressBar(y, rows);

    }

    std::cout << "\nsetting multicut bits\n";
    int regionIndex = 0;
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
                if(!compareRGB(currentColor, getVertexColor(neighbor))){
                // ###### add multicut bit 
                setEdgeBit(v,neighbor,1);
                }
            }
        }
        printProgressBar(v, getVertices());
    }

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
        std::cout << "Pixel " << pixel << ": RGB(" << color.red.to_ulong() << ", " << color.green.to_ulong() << ", " << color.blue.to_ulong() << ")" << std::endl;
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

void Graph::printRegionVector(){
    // Print the regions
    
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

void Graph::reconstructImage(/*std::vector<std::vector<int>> regions, std::vector<RGB> regionColors*/){
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


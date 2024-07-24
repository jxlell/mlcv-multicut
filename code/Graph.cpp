// Graph.cpp
#include "Graph.h"
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <stack>
#include <unordered_set>
#include "partition.hxx"
#include <cstdlib>
#include <queue>
#include <chrono>

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


    //this->imagePath = imagePath;
    adjList.resize(vertices);
    vertexValues.resize(vertices, 0);
    vertexColors.resize(vertices);
    edgeBits01.resize((cols-1)*rows + cols*(rows-1));
    visited.resize(edgeBits01.size());
    vertexRegions.resize(vertices, -1);

    regionRepresentatives.resize(cols*rows, -1);
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


// Function to add a vertex to the graph
void Graph::addVertex() {
    vertices++;
    adjList.push_back(std::vector<int>()); // Add an empty vector for the new vertex
}


void Graph::printColorRegions(){
    for (const auto& color : regionColors) {
        std::cout << "RGB: " << static_cast<int>(color.red) << ", " << static_cast<int>(color.green) << ", " << static_cast<int>(color.blue) << std::endl;
    }
    
}

void Graph::printSize(){
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
    //std::cout << "setting multicut\n";
    auto start = std::chrono::high_resolution_clock::now();
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
    auto end = std::chrono::high_resolution_clock::now();
    auto start_to_end = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "time to set vertex colors in ms: " << start_to_end << std::endl;

    start = std::chrono::high_resolution_clock::now();

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
                //addEdge(neighbor, v);
                if(offset == 1 || offset == cols){
                    //std::cout << edgeIndex << ", ";
                    edgeIndex++;
                    if(!compareRGB(currentColor, getVertexColor(neighbor))){
                        //set edgeBit01
                        edgeBits01[edgeIndex] = true;
                        //std::cout << "offset: " << offset << ", ";
                    }
                }
            }
        }
        //printProgressBar(v, getVertices());
    }
    end = std::chrono::high_resolution_clock::now();
    start_to_end = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "time to set edge bits in ms: " << start_to_end << std::endl;
    
    /*
    std::cout << "size edgebits01: " << edgeBits01.size() << std::endl;
    std::cout << "True edgebits: " << std::count(edgeBits01.begin(), edgeBits01.end(), true) << std::endl;
    std::cout << "percentage: " << 100*std::count(edgeBits01.begin(), edgeBits01.end(), true) / edgeBits01.size() << "%" << std::endl;
    
    */
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

    start = std::chrono::high_resolution_clock::now();

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

    end = std::chrono::high_resolution_clock::now();
    start_to_end = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "time to set regions in ms: " << start_to_end << std::endl;

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

    start = std::chrono::high_resolution_clock::now();
    
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
    //std::cout << "edgeI: " << edgeI << std::endl;
    //std::cout << "number of edges: " << 2*rows*cols - cols - rows << std::endl;
    end = std::chrono::high_resolution_clock::now();
    start_to_end = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "time to set paths in ms: " << start_to_end << std::endl;

    //printPaths();
    
    
    disconnectedComponents = dfsI;
    //std::cout << "\nnumber of disconnected components: " << dfsI << std::endl;

    

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

std::vector<bool> Graph::dfs_paths_iterative(int currentEdge, Direction currentDir, std::vector<bool>& visited){
    std::vector<bool> directionVector;
    //std::vector<bool> visited(edgeBits01.size(), false);
    std::stack<std::pair<int, Direction>> pendingEdges;
    int vecIndex = 0;
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

        int neighborLeft = getNeighbor(currentEdge, currentDir, 0);
        int neighborFront = getNeighbor(currentEdge, currentDir, 1);
        int neighborRight = getNeighbor(currentEdge, currentDir, 2);


        //TODO: push 000 und continue falls nächste edges nicht multicut oder schon visited 
        if(neighborLeft == -1 || neighborFront == -1 || neighborRight == -1 || neighborLeft >= edgeBits01.size() || neighborFront >= edgeBits01.size() || neighborRight >= edgeBits01.size()){
            directionVector.push_back(0);
            directionVector.push_back(0);
            directionVector.push_back(0);
            //directionVector[vecIndex++] = 0;
            //directionVector[vecIndex++] = 0;
            //directionVector[vecIndex++] = 0;
            //std::cout << "000" << std::endl;
            continue;
        }




        if(!visited[neighborLeft]){
            left = edgeBits01[neighborLeft];
            directionVector.push_back(left);
            //directionVector[vecIndex++] = left;
        }else{
            directionVector.push_back(false);
            //directionVector[vecIndex++] = false;
        }
        if(!visited[neighborFront]){
            front = edgeBits01[neighborFront];
            directionVector.push_back(front);
            //directionVector[vecIndex++] = front;
        }
        else{
            directionVector.push_back(false);
            //directionVector[vecIndex++] = false;

        }
        if(!visited[neighborRight]){
            right = edgeBits01[neighborRight];
            directionVector.push_back(right);
            //directionVector[vecIndex++] = right;

        }
        else{
            directionVector.push_back(false);
            //directionVector[vecIndex++] = false;
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
            //directionVector[vecIndex++] = 0;
            //directionVector[vecIndex++] = 0;
            //directionVector[vecIndex++] = 0;
            continue;
        }
        
        

        if(right){
            if(!visited[neighborRight]){
                pendingEdges.push(std::make_pair(neighborRight, nextDirection(currentDir)));
                visited[neighborRight] = true;
            }
        }
        if(front){
            if(!visited[neighborFront]){
                pendingEdges.push(std::make_pair(neighborFront, currentDir));
                visited[neighborFront] = true;
            }
        }
        if(left){
            if(!visited[neighborLeft]){
                pendingEdges.push(std::make_pair(neighborLeft, previousDirection(currentDir)));
                visited[neighborLeft] = true;
            }
        }

    }
    //directionVector.resize(vecIndex);
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
        //reconstructed_edgeBits = reconstruct_edgeBits_iterative(std::get<0>(pathinfo), std::get<1>(pathinfo), std::get<2>(pathinfo));
        reconstruct_edgeBits_iterative(std::get<0>(pathinfo), std::get<1>(pathinfo), std::get<2>(pathinfo), reconstructed_edgeBits);
        directionBitsSize += std::get<2>(pathinfo).size();
        //break;
        i++;
    }

    //std::cout << "size of directionbits: " << directionBitsSize << std::endl;


    //std::cout << "\nreconstruction for edgebits01 finished" << std::endl;

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
    
    //printSize();
    std::cout << "reconstruction and original identical: " << ((areImagesIdentical(img, image)) ? "YES" : "NO") << std::endl;
    
    /*
    cv::destroyAllWindows();
    cv::imshow("Original", img);
    cv::imshow("Reconstruction", image);
    cv::waitKey(0);
    */
    
    
    
    // reset visited vector for reconstruction dfs
    visited.assign(visited.size(), false);

    //edgeBits01.assign(edgeBits01.size(), false);

 
    // compression rate for paths representation 
    return static_cast<double>(3*8*cols*rows) / (directionBitsSize + numberOfPaths*(8 + 2) + regionColors.size()*3*8 );
    // compression rate for edgebit representation
    //return static_cast<double>(3*8*cols*rows) / ((2*cols*rows-cols-rows) + regionColors.size()*3*8 );

}

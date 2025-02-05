#include "Compressor.h"
#include "Multicut.h"
#include "Image.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
#include "partition.hxx"
#include "DirectionPath.h"
#include "Util.h"


/**
 * @class Compressor
 * @brief Compressor module handling the 
 * TODO: als klasse notwendig? 
 * 
 */
Compressor::Compressor(const std::string& imagePath, const std::string& volumePath) 
    : imagePath(imagePath), 
      img(cv::imread(imagePath, cv::IMREAD_COLOR)), 
      multicut(img),  // Initialize multicut with img
      vertices(img.rows * img.cols) 
{
    //std::cout << "image path: " << imagePath << std::endl;
    if (img.empty()) {
        std::cerr << "Error: Could not read the image: " << imagePath << std::endl;
    }
    neighborsOffsets = {img.cols, 1};
    this->volumePath = volumePath;
}


/**
 * @brief controlling the compression procedure 
 * @return tuple containing compressed image information in form of the color vector, path information as well as the original image for comparison
 */
std::tuple<std::vector<RGB>, PathInfoVector, cv::Mat, PathInfoVector, RLEVector> Compressor::compressImage(){
    auto start = std::chrono::high_resolution_clock::now();
    //setVertexColors();
    //auto end = std::chrono::high_resolution_clock::now();
    //auto start_to_end = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    //std::cout << "time to set vertex colors in ms: " << start_to_end << std::endl;
 
    //start = std::chrono::high_resolution_clock::now();
    setEdgeBits();
    //end = std::chrono::high_resolution_clock::now();
    //start_to_end = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    //std::cout << "time to set edge bits in ms: " << start_to_end << std::endl;

    //start = std::chrono::high_resolution_clock::now();
    setRegions();
    //end = std::chrono::high_resolution_clock::now();
    //start_to_end = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    //std::cout << "time to set regions in ms: " << start_to_end << std::endl;

    //start = std::chrono::high_resolution_clock::now();
    setPaths();
    auto end = std::chrono::high_resolution_clock::now();
    auto start_to_end = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    //std::cout << "time to set paths in ms: " << start_to_end << std::endl;
    std::string filename = imagePath.substr(imagePath.find_last_of("/\\") + 1);
    //std::cout << "time to compress " << filename << ": " << start_to_end << " ms" << std::endl;
    compressionTime = start_to_end;
    
    //FIXME: print
    /*
    for (bool a : std::get<2>(multicut.paths[0])){
        std::cout << a;
    }
    */
    set2BitPaths();
    return std::make_tuple(multicut.regionColors, multicut.paths, img, multicut.paths_2bit, rle_paths);
}

/**
 * @brief sets values of the vertex colors vector based on the image content
 * @see Multicut::setVertexColor(int v, int red, int green, int blue)
 */
void Compressor::setVertexColors(){
    // Iterate through every pixel from top-left to bottom-right
    int pixel_index = 0;
    for (int y = 0; y < img.rows; ++y) {
        for (int x = 0; x < img.cols; ++x) {
            // Access pixel value at position (x, y)
            cv::Vec3b pixel = img.at<cv::Vec3b>(y, x);

            // Access individual color channels
            uchar blue = pixel[0];
            uchar green = pixel[1];
            uchar red = pixel[2];


            //set rgb values for vertex
            //img_graph.setVertexColor(pixel_index, static_cast<int>(red), static_cast<int>(blue), static_cast<int>(green));
            //std::cout << y*img.cols+x << ", ";
            multicut.setVertexColor(y*img.cols+x, static_cast<int>(red), static_cast<int>(green), static_cast<int>(blue));
            //std::cout << getVertexColor(y*cols+x).red << ", ";
            pixel_index++;
        }
        //printProgressBar(y, rows);

    }
}

/**
 * @brief sets the edgebits vector in the multicut object based on the vertex colors from the 
 * multicut object 
 */
void Compressor::setEdgeBits(){
    //std::cout << "\nsetting multicut bits\n";
    int regionIndex = 0;
    int edgeIndex = -1;
    // Check neighbors for every vertex and set multicut bits
    for (int v = 0; v < vertices; ++v) {
        RGB currentColor = multicut.getVertexColor(v);
        // get color of previous pixel (current pixel if there is no previous)
        RGB previousColor = multicut.getVertexColor(std::max(0,v-1));
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
                    if(!compareRGB(currentColor, multicut.getVertexColor(neighbor))){
                        //set edgeBit01
                        multicut.edgeBits01[edgeIndex] = true;
                        //std::cout << "offset: " << offset << ", ";
                    }
                }
            }
        }
        //printProgressBar(v, getVertices());
    }
}

/**
 * @brief creates color partition object and obtains a continuous labeling for the region indices 
 * @see Multicut::getRegionsFromImage()
 */
void Compressor::setRegions(){
    andres::Partition<int> multicutregion = multicut.getRegionsFromImage();
    //for(int i = 0; i<25; i++){
    //    std::cout << "region of index " << i << ": " << multicutregion.find(i) << ", ";
    //}

    std::vector<int> reps;
    multicutregion.representatives(std::back_inserter(reps));
    //std::cout << std::endl;
    for (int rep : reps) {
        //std::cout << rep << ", ";
        multicut.regionColors.push_back(multicut.getVertexColor(rep));
        //std::cout << static_cast<int>(repRGB.red) << ", ";
    }
    multicut.regionColors.resize(reps.size());
}

/**
 * @brief iterates every edge in the image and starts depth first search to obtain path vectors 
 * @see Multicut::dfs_paths_iterative(int currentEdge, Direction currentDir, std::vector<bool>& visited)
 */
void Compressor::setPaths(){
    int edgeI = 0;
    int dfsI = 0;

    
    
    for (bool edge : multicut.edgeBits01){
        // skip non-multicut edges or previously visited edges ||
        if(!edge){
            edgeI++;
            continue;
            }
        if(multicut.visited[edgeI]){
            edgeI++;
            continue;
        }
        //std::cout << std::endl << edgeI;

        // dir rausfinden
        // get current direction (either down or right) from current index 
        Direction currentDir = getDirectionFromIndex(edgeI, img.rows, img.cols);

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
        directionVector = multicut.dfs_paths_iterative(edgeI, currentDir, multicut.visited);
        multicut.paths.emplace_back(intToBool(edgeI), currentDir, directionVector);
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

    //printPaths();

    // Calculate storage space for the path vector
    double totalBitsPathVector = 0;
    for (const auto& path : multicut.paths) {
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

    std::cout << "Total bits for path vector: " << totalBitsPathVector << std::endl;
    //std::cout << "bits needed per start point: " << ceil(log2(2 * img.rows * img.cols - img.rows - img.cols)) << std::endl;
    
    multicut.disconnectedComponents = dfsI;
    //std::cout << "number of disconnected components: " << dfsI << std::endl;

}

//TODO: 2bit paths 
// FIXME: vervollständigen 

void Compressor::set2BitPaths(){
    //TODO: beide vektoren als bitset? größe ja vorher bekannt 
    std::vector<bool> edgeBits01 = multicut.edgeBits01;
    //TODO: initialisiern mit größe, anstatt edgebits von multicut zu nehmen 
    std::vector<bool> visited(multicut.getEdges(), false);
    //visited.resize(multicut.getEdges(), false);
    //std::cout << imagePath << std::endl;

    for (int edgeI = 0; edgeI < multicut.getEdges(); edgeI++){
        int iter_i = 0;
        if(visited[edgeI]){continue;}
        if(!multicut.edgeBits01[edgeI]){
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
            std::tuple<std::vector<bool>, std::vector<uint16_t>, bool> rle = getRLE(directions2bits);
            std::vector<std::vector<bool>> rle_directions;
            for(uint16_t run : std::get<1>(rle)){
                rle_directions.push_back(intToBool(run));
            }
            rle_paths.emplace_back(intToBool(edgeI), std::get<0>(rle), rle_directions, std::get<2>(rle));
        }
        multicut.paths_2bit.emplace_back(intToBool(edgeI), startDir, directions2bits);
    }

    // Calculate storage space for the paths vector
    double totalBits2BitPaths = 0;
    int emptyPathsCount = 0;
    for (const auto& path : multicut.paths_2bit) {
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
    std::cout << "Number of 2-bit paths: " << multicut.paths_2bit.size() << std::endl;
    std::cout << "Total bits for 2-bit paths: " << totalBits2BitPaths << std::endl;
    std::cout << "Number of empty paths: " << emptyPathsCount << std::endl;

    // Identify the longest run of ones in the direction vectors
    int longestRun = 0;
    for (const auto& path : multicut.paths_2bit) {
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
    std::cout << "Longest run of ones in direction vectors: " << longestRun << std::endl;

    // Calculate bits needed for run length encoding
    double totalBitsRLE = 0;
    for (const auto& path : multicut.paths_2bit) {
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
    std::cout << "Total bits for run length encoding: " << totalBitsRLE << std::endl;


    // Count the runs of 0s in the 2-bit paths
    int totalRunsOfZeros = 0;
    for (const auto& path : multicut.paths_2bit) {
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
    return;
}

void Compressor::print2bitpaths(PathInfoVector paths){
    for (const auto& path : paths) {
        std::vector<bool> edgeI;
        Direction dir;
        std::vector<bool> directions2bits;

        std::tie(edgeI, dir, directions2bits) = path;

        std::cout << "Start Edge: " << boolVectorToInt(edgeI) << ", Direction: " << directionToString(dir) << ", Path: ";
        for (bool bit : directions2bits) {
            std::cout << bit;
        }
        std::cout << std::endl;
    }
}



/**
 * @brief returns multicut object 
 * @see Multicut::Multicut(cv::Mat img)
 * @return multicut object
 */
Multicut Compressor::getMulticut(){
    return multicut;
}

/**
 * @brief obtains color pixel partitions from multiple image slices
 * @see Compressor::getRegionsFromVolume(std::vector<Multicut> volume)
 */
void Compressor::compressVolume(){
    if(volumePath.empty()){
        std::cerr << "Error: Volume path is empty" << std::endl;
    }
    std::vector<Multicut> volume; // Vector to store the cv matrices of the images
    
    // Open the volume directory
    cv::String pattern = volumePath + "/*.png";
    std::vector<cv::String> fileNames;
    cv::glob(pattern, fileNames);
    
    // Iterate through each file in the volume directory
    for (const auto& fileName : fileNames) {
        cv::Mat image = cv::imread(fileName, cv::IMREAD_COLOR);
        if (image.empty()) {
            std::cerr << "Error: Could not read the image: " << fileName << std::endl;
        } else {
            Multicut mc(image);
            volume.push_back(mc);
        }
    }
    
    andres::Partition<int> regions = getRegionsFromVolume(volume);
}

/**
 * @brief get color pixel partitions from multiple multicut objects 
 * @param volume Vector of multicut objects / image slices  
 */
andres::Partition<int> Compressor::getRegionsFromVolume(std::vector<Multicut> volume){
    andres::Partition<int> region(img.rows*img.cols*volume.size());
    std::vector<int> volumeOffsets = {img.cols, 1, img.cols*img.rows};
    int slice_index;
    int vertexI;
    for (int index = 0; index < img.rows * img.cols * volume.size(); ++index) {
        vertexI = index % (img.cols*img.rows);
        if(index % img.cols * img.rows == 0){
            slice_index++;
        }
        int neighbor = vertexI + 1;
        if(neighbor >= 0 && neighbor < img.rows * img.cols * volume.size() && ((neighbor / img.cols == index / img.cols) || (abs(neighbor - index) > 1))) {
            if(compareRGB(volume[slice_index-1].getVertexColor(vertexI), volume[slice_index-1].getVertexColor(neighbor))){
                region.merge(index, neighbor);
            }
        neighbor = index + img.cols;
        neighbor = index + img.cols*img.rows;
        }
    }
    return region;
}

/**
 * @brief returns total time for compression
 * @return compression time in ms 
 */
long long Compressor::getCompressionTime(){
    return compressionTime;
}


/**
 * @brief calculates compression rate based on the bits needed for storing the path vector
 * as well as the color vector, compares to storing color for every pixel individually
 * @return factor of compression 
 */
double Compressor::getCompressionRate(){
    double totalBits = 0;

    // Calculate bits for color regions
    totalBits += multicut.regionColors.size() * 3 * 8;
    totalBits += 2; 
    if(multicut.paths.size() == 0){
            return 1;
        }
    for (const auto& path : multicut.paths) {
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
    double compressionRate = static_cast<double>(vertices) * 24.0 / totalBits;
    
    imgSize = totalBits;
    return compressionRate;
}

double Compressor::getRLECompressionRate(){
    double totalBitsRLE = 0;
    // Calculate bits for color regions
    totalBitsRLE += multicut.regionColors.size() * 3 * 8;

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
    std::cout << "Total bits for RLE: " << totalBitsRLE << std::endl;
    if(static_cast<double>(vertices) * 24.0 / totalBitsRLE  > 1000){
        std::cout << "high rate: " << imagePath << std::endl;
    }
    return static_cast<double>(vertices) * 24.0 / totalBitsRLE;
}


/**
 * @brief compression rate of method without multicut paths 
 * @return factor of compression
 */
double Compressor::getOldCompressionRates(){
    double compRate = static_cast<double>(vertices) * 24.0 / 
                      (static_cast<double>(multicut.edgeBits01.size()) + 
                       static_cast<double>(multicut.regionColors.size()) * 3.0 * 8.0);
    return compRate;
}

/**
 * @return percentage of edges connecting two different-colored pixels 
 */
double Compressor::getMulticutPercentage(){
    return 100*std::count(multicut.edgeBits01.begin(), multicut.edgeBits01.end(), true) / multicut.edgeBits01.size();
}

/**
 * @return number of disconnected paths
 */
double Compressor::getDisconnectedComponents(){
    return multicut.disconnectedComponents;
}

/**
 * @return size of the image in bits 
 */
int Compressor::getImgSize(){
    return imgSize;
}

/**
 * @return size of the image in kilobits
 */
int Compressor::getkBSize(){
    return imgSize / 1024;
}
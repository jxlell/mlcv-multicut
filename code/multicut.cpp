#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Graph.h"
#include <chrono>

using namespace std;

// g++ -std=c++11 -o multicut multicut.cpp Graph.cpp $(pkg-config --cflags --libs opencv4); ./multicut

int main() {
       

    //cv::Mat img = cv::imread("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV Project/code/tree5x5.png", 
    //cv::IMREAD_UNCHANGED /*cv::IMREAD_COLOR*/);

    //cv::Mat img = cv::imread("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV Project/code/tree5x5.jpg", 
    //cv::IMREAD_COLOR);

    /*

    
    if (img.empty()) {
        std::cerr << "Error: Could not read the image." << std::endl;
        return -1;
    }

    int rows = img.rows;
    int cols = img.cols;
    */
    //Graph img_graph("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV Project/code/tree5x5.png");
    //Graph img_graph("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV Project/code/tree461x207.png");
    //Graph img_graph("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV Project/code/images/icon_64/actions-address-book-new.png");
    //Graph img_graph("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV Project/code/tree5x5.png");
    Graph img_graph("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV Project/code/images/icon_64/actions-address-book-new.png");
    //Graph img_graph("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV Project/code/2colscreen.png");

    img_graph.setMulticut();
    

    //img_graph.printGraph();
    //img_graph.printEdgeBits();
    auto startTime = std::chrono::high_resolution_clock::now();
    img_graph.findAllRegions();
    // Record the end time
    auto endTime = std::chrono::high_resolution_clock::now();

    // Calculate the duration in milliseconds
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    // Print the duration
    std::cout << "\nFunction took " << duration.count() << " milliseconds." << std::endl;

    //img_graph.printColorRegions();
    //img_graph.printSize();
    img_graph.reconstructImage();

    //cv::imshow("Image", img);
    //cv::waitKey(3000);

    //img_graph.printConnectedPixels(20);

    return 0;
}
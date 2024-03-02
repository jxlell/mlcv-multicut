#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Graph.h"
#include <chrono>
#include <filesystem> 


using namespace std;

// g++ -std=c++11 -o multicut multicut.cpp Graph.cpp $(pkg-config --cflags --libs opencv4); ./multicut

int main() {
       

    //Graph img_graph("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV Project/code/tree5x5.png");
    //Graph img_graph("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV Project/code/tree461x207.png");
    //Graph img_graph("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV Project/code/images/icon_64/actions-address-book-new.png");
    //Graph img_graph("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV Project/code/tree5x5.png");
    //Graph img_graph("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV Project/code/images/icon_64/actions-address-book-new.png");
    
    string imgDir = "/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV Project/code/images/icon_512";
    std::filesystem::path p1 { imgDir };
    int count {};
    int i = 0;

    for (auto& p : std::filesystem::directory_iterator(p1))
    {
        ++count;
    }

    for (const auto& dirEntry : std::filesystem::directory_iterator(imgDir)){
        if(dirEntry.path().extension().string() != ".png"){
            continue;
        }

        Graph img_graph(dirEntry.path().string()); 
        //cout << dirEntry.path().filename().string() << ", ";
        //Graph img_graph("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV Project/code/images/photo_wikipedia/011.png");

        img_graph.setMulticut();
        

        //img_graph.printGraph();
        //img_graph.printEdgeBits();
        //auto startTime = std::chrono::high_resolution_clock::now();

        //img_graph.findAllRegions();
        //img_graph.assignRegions();


        //img_graph.printColorRegions();
        //img_graph.printSize();
        //img_graph.reconstructImage();

        //img_graph.unionFindMulticut();
        img_graph.reconstructMulticut();
        cout << dirEntry.path() << " reconstructed\n";

        //cv::imshow("Image", img);
        //cv::waitKey(3000);
        //img_graph.printColorRegions();
        //img_graph.printRegionVector();
        //break;

        //img_graph.printConnectedPixels(20);
        //img_graph.printProgressBar(i, count);
        ++i;

        //std::vector<int> regions = img_graph.getVertexRegions();

        //cout << "size in byte: " << sizeof(regions) * 8 << ", .size(): " << regions.size() * 8 << endl;

    }
    
    //cout << "size of int: " << sizeof(int) * 8 << " bit" << endl;
        

    return 0;
}
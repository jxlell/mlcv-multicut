#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Graph.h"
#include <chrono>
#include <filesystem> 
#include <fstream>
#include <numeric>
#include "Compressor.h"
#include "Decompressor.h"


using namespace std;

// g++ -std=c++11 -o multicut multicut.cpp Graph.cpp $(pkg-config --cflags --libs opencv4); ./multicut

void writeToOutput(std::filesystem::path p1, std::vector<double> compression_rates, int i){
    std::ofstream outputFile("output_files/output" + p1.filename().string() + ".csv");

    // Check if the file opened successfully
    if (!outputFile.is_open()) {
        std::cerr << "Error: Unable to open the file." << std::endl;
        return;
    }

    for (size_t i = 0; i < compression_rates.size(); ++i) {
        outputFile << compression_rates[i]; // Write the element

        // Add a comma if it's not the last element
        if (i != compression_rates.size() - 1) {
            outputFile << ",";
        }
    }
        
    outputFile.close();

    double total_compression_rates = std::accumulate(compression_rates.begin(), compression_rates.end(), 0.0);
    double avg_compression_rate = total_compression_rates / i; 
    std::cout << avg_compression_rate << endl;


}

int main() {
    
    string imgDir = "/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/5x5example";
    std::filesystem::path p1 { imgDir };
    int count {};
    int i = 0;

    vector<double> compression_rates;
    
    
    long long total_time_set_multicut = 0;
    long long total_time_reconstruct_multicut = 0;
    
    for (auto& p : std::filesystem::directory_iterator(p1))
    {
        ++count;
    }

    for (const auto& dirEntry : std::filesystem::directory_iterator(imgDir)){
        if(dirEntry.path().extension().string() != ".png"){
            continue;
        }

        Graph img_graph(dirEntry.path().string()); 
        cv::Mat img = cv::imread(dirEntry.path().string(), cv::IMREAD_COLOR);
        Compressor comp(dirEntry.path().string());
        std::pair<std::vector<RGB>, PathInfoVector> compressed_image = comp.compressImage();
        Decompressor decomp(compressed_image.first, compressed_image.second, comp.getMulticut().edgeBits01.size(), img.cols, img.rows, img);
        decomp.reconstructImage();
        std::cout << dirEntry.path() << endl;
        auto startTime = std::chrono::high_resolution_clock::now();


        //img_graph.setMulticut();

        auto end_set_multicut = std::chrono::high_resolution_clock::now();
        total_time_set_multicut += std::chrono::duration_cast<std::chrono::milliseconds>(end_set_multicut - startTime).count();


        auto start_reconstruct_multicut = std::chrono::high_resolution_clock::now();
        //compression_rates.push_back(img_graph.reconstructMulticut());
        auto end_reconstruct_multicut = std::chrono::high_resolution_clock::now();
        total_time_reconstruct_multicut += std::chrono::duration_cast<std::chrono::milliseconds>(end_reconstruct_multicut - start_reconstruct_multicut).count();

        //img_graph.printProgressBar(i, count);
        ++i;

    }
    

    // WRITE COMPRESSION RATES TO FILE
    //writeToOutput(p1, compression_rates, i);

    

    //std::cout << total_time_set_multicut/i << endl;
    //std::cout << total_time_reconstruct_multicut/i << endl;
    

    return 0;
}
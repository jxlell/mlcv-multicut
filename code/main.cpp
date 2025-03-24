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
#include "Util.h"
#include "compress.h"
#include "decompress.h"
#include <unordered_set>


using namespace std;

// g++ -std=c++11 -o multicut multicut.cpp Graph.cpp $(pkg-config --cflags --libs opencv4); ./multicut


/**
 * @brief main function loading image files and controlling compression and decompression procedure 
 * 
 */
int main() {

    bool compression_successful = true;

    vector<double> compression_rates;
    vector<double> old_compression_rates; 
    vector<double> rle_compression_rates;
    vector<double> straights_compress_rates;
    vector<double> straights_huffman_compress_rates;
    vector<double> newEdgeBitsCompressionRates;
    vector<long long> compression_times;
    vector<long long> decompression_times;
    vector<double> multicut_percentages;
    vector<int> disconnected_components;
    vector<int> pixel_sizes; 
    vector<int> twobit_paths_amounts;

    vector<double> compression_rates_total;
    vector<double> old_compression_rates_total; 
    vector<double> rle_compression_rates_total;
    vector<double> straights_compress_rates_total;
    vector<double> straights_huffman_compress_rates_total;
    vector<double> newEdgeBitsCompressionRates_total;
    vector<long long> compression_times_total;
    vector<long long> decompression_times_total;
    vector<double> multicut_percentages_total;
    vector<int> disconnected_components_total;
    vector<int> pixel_sizes_total; 

    vector<string> filenames;
    vector<string> categories;
    vector<double> kBSizes;


    std::unordered_set<std::string> category_set = {
        "icon_64",
        // "icon_512",
        // "photo_kodak",
        // "photo_tecnick",
        // "photo_wikipedia",
        // "pngimg",
        // "screenshot_web",
        // "screenshot_game",
        // "textures_photo",
        // "textures_pk",
        // "textures_pk01",
        // "textures_pk02",
        // "textures_plants"
    };

    std::ifstream inputFile("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/singlefile.txt");
    if (!inputFile.is_open()) {
        std::cerr << "Error: Unable to open file for reading." << std::endl;
        return -1;
    }

    std::string single_image_txt;
    if (std::getline(inputFile, single_image_txt)) {
        // Process the single line read from the file
        std::cout << single_image_txt << std::endl;
    }

    inputFile.close();

    // control parameters 
    bool single_image = false;
    string single_image_name = single_image_txt;
    bool showImg = single_image;
    // showImg = false;
    bool writeToFile = false;
    //writeToFile = !single_image;

    std::filesystem::path parentDir = "/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/code/images";
    int imgCount = countImgFiles(parentDir, category_set);
    int progress = 0;

    if(single_image){
        parentDir = "/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code";
        category_set = {"test_img"};
    }


    for (const auto& entry : std::filesystem::directory_iterator(parentDir)) {
        if (!entry.is_directory() 
        || category_set.find(entry.path().filename().string()) == category_set.end()
        ) {
            continue; 
        }
        for (const auto& dirEntry : std::filesystem::directory_iterator(entry)){
            if(dirEntry.path().extension().string() != ".png" //|| dirEntry.path().filename().string() != "blek_1.png"
            ){
                continue;
            }
            if(single_image && dirEntry.path().filename().string() != single_image_name){
                continue;
            }

            std::cout << "\nCompressing: " << dirEntry.path().filename().string() << "\nCategory: " << entry.path().filename().string() << std::endl;
            //returning color vector, path vector, original image

            //stateless approach
            auto start = std::chrono::high_resolution_clock::now();
            auto compImg = compress(dirEntry.path().string());
            auto end = std::chrono::high_resolution_clock::now();
            auto compression_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

            std::string filename = dirEntry.path().filename().string();
            // Remove comma if it exists in the filename
            filename.erase(std::remove(filename.begin(), filename.end(), ','), filename.end());
            filenames.push_back(filename);
            categories.push_back(entry.path().filename().string());

            compression_times.push_back(compression_time);
            //compression_times_total.push_back(comp.getCompressionTime());

            //double compression_rate = getCompressionRate(compImg.colorVector, compImg.paths, compImg.originalImage);
            //std::cout << "Compression Rate: " << compression_rate << std::endl;
            compression_rates.push_back(compImg.pathCompressionRate);
            //compression_rates_total.push_back(compression_rate);

            old_compression_rates.push_back(compImg.oldCompressionRate);
            newEdgeBitsCompressionRates.push_back(compImg.newEdgeBitsCompressionRate);
            //old_compression_rates_total.push_back(compImg.oldCompressionRate);

            rle_compression_rates.push_back(compImg.rleCompressionRate);

            straights_compress_rates.push_back(compImg.straightsCompressionRate);
            straights_huffman_compress_rates.push_back(compImg.straightsHuffmanCompressionRate);

            
            //cv::Mat img;
            //img = cv::imread(dirEntry.path().string(), cv::IMREAD_COLOR);
            
            //stateless approach
            start = std::chrono::high_resolution_clock::now();
            bool success = reconstructImage(compImg, showImg); 
            end = std::chrono::high_resolution_clock::now();
            auto decompression_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            decompression_times.push_back(decompression_time);

            progress++;
            //std::cout << progress << "/" << imgCount << std::endl;
            if(!single_image){
                printProgressBar(progress, imgCount);
            }

            if (!success) {
                compression_successful = false;
            }

        }

        std::cout << "\n\n------\n" << (compression_successful ? "✅✅✅" : "❌❌❌") << std::endl << "------\n";


        if(writeToFile){
            // WRITE COMPRESSION RATES TO FILE
            writeToOutput(entry, compression_rates, "compression_rates");
            //writeToOutput(entry, old_compression_rates, "old_compression_rates");
            writeToOutput(entry, compression_times, "compression_times");
            writeToOutput(entry, decompression_times, "decompression_times");
            //writeToOutput(entry, multicut_percentages, "multicut_percentages");
            //writeToOutput(entry, disconnected_components, "disconnected_components");
            //writeToOutput(entry, pixel_sizes, "pixel_sizes");
            writeToOutput(entry, rle_compression_rates, "rle_compression_rates");
            //writeToOutput(entry, twobit_paths_amounts, "twobit_paths_amounts");
            writeToOutput(entry, straights_compress_rates, "straights_compress_rates");
            writeToOutput(entry, straights_huffman_compress_rates, "straights_huffman_compression_rates");
        }


        // std::ofstream total_file("code/total_csv.csv");
        // if(total_file){
        //     total_file << 
        // }

        compression_rates_total.insert(compression_rates_total.end(), compression_rates.begin(), compression_rates.end());
        compression_rates.clear();

        old_compression_rates_total.insert(old_compression_rates_total.end(), old_compression_rates.begin(), old_compression_rates.end());
        old_compression_rates.clear();

        newEdgeBitsCompressionRates_total.insert(newEdgeBitsCompressionRates_total.end(), newEdgeBitsCompressionRates.begin(), newEdgeBitsCompressionRates.end());
        newEdgeBitsCompressionRates.clear();

        compression_times_total.insert(compression_times_total.end(), compression_times.begin(), compression_times.end());
        compression_times.clear();

        decompression_times_total.insert(decompression_times_total.end(), decompression_times.begin(), decompression_times.end());
        decompression_times.clear();

        rle_compression_rates_total.insert(rle_compression_rates_total.end(), rle_compression_rates.begin(), rle_compression_rates.end());
        rle_compression_rates.clear();

        straights_compress_rates_total.insert(straights_compress_rates_total.end(), straights_compress_rates.begin(), straights_compress_rates.end());
        straights_compress_rates.clear();

        straights_huffman_compress_rates_total.insert(straights_huffman_compress_rates_total.end(), straights_huffman_compress_rates.begin(), straights_huffman_compress_rates.end());
        straights_huffman_compress_rates.clear();
        // multicut_percentages.clear();
        // disconnected_components.clear();
        pixel_sizes.clear();
    }

    if(writeToFile){
        ofstream csvFile("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/output_files/mc_results.csv");
        if (!csvFile.is_open()) {
            cerr << "Error: Unable to open CSV file for writing." << endl;
            return -1;
        }

        // csvFile << "filename,category,pixel_size,encode_ms,decode_ms,size_kb,rate\n";
        // for (size_t i = 0; i < filenames.size(); ++i) {
        //     csvFile << filenames[i] << ","
        //             << categories[i] << ","
        //             << pixel_sizes_total[i] << ","
        //             << compression_times_total[i] << ","
        //             << decompression_times_total[i] << ","
        //             << kBSizes[i] << ","
        //             << compression_rates_total[i] << "\n";
        // }

        csvFile << "filename,category,old_rate,path_rate,rle_rate,straights_rate,straights_huffman_rate,new_edgebits_rate,comp_time,decomp_time\n";
        for (size_t i = 0; i < filenames.size(); ++i) {
            csvFile << filenames[i] << ","
                    << categories[i] << ","
                    // << pixel_sizes_total[i] << ","
                    << old_compression_rates_total[i] << ","
                    << compression_rates_total[i] << ","
                    << rle_compression_rates_total[i] << ","
                    << straights_compress_rates_total[i] << ","
                    << straights_huffman_compress_rates_total[i] << ","
                    << newEdgeBitsCompressionRates_total[i] << ","
                    << compression_times_total[i] << ","
                    << decompression_times_total[i] << "\n";
                    // << kBSizes[i] << ","
        }

        csvFile.close();
    }
    
    
    return 0;
}
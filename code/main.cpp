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

      auto completeStart = std::chrono::high_resolution_clock::now();

    bool compression_successful = true;

    vector<double> tree_compression_rates;
    vector<double> old_compression_rates; 
    vector<double> rle_compression_rates;
    vector<double> straights_compress_rates;
    vector<double> straights_huffman_compress_rates;
    vector<double> reducedEdgeBitsCompressionRates;
    vector<double> paths2bit_compression_rates;
    vector<long long> compression_times;
    vector<long long> decompression_times;
    vector<double> multicut_percentages;
    vector<int> disconnected_components;
    vector<int> pixel_sizes; 
    vector<int> twobit_paths_amounts;

    vector<double> tree_compression_rates_total;
    vector<double> old_compression_rates_total; 
    vector<double> rle_compression_rates_total;
    vector<double> straights_compression_rates_total;
    vector<double> straights_huffman_compression_rates_total;
    vector<double> reduced_edgebits_compression_rates_total;
    vector<double> paths2bit_compression_rates_total;

    vector<long long> compression_times_total;
    vector<long long> decompression_times_total;
    vector<double> multicut_percentages_total;
    vector<int> disconnected_components_total;
    vector<int> pixel_sizes_total; 

    vector<long long> tree_compression_times;
    vector<long long> tree_compression_times_total;
    vector<long long> old_compression_times;
    vector<long long> old_compression_times_total;
    vector<long long> rle_compression_times;
    vector<long long> rle_compression_times_total;
    vector<long long> straights_compress_times;
    vector<long long> straights_compress_times_total;
    vector<long long> straights_huffman_compression_times;
    vector<long long> straights_huffman_compression_times_total;
    vector<long long> reduced_edgebits_compression_times;
    vector<long long> reduced_edgebits_compression_times_total;
      vector<long long> paths_2bits_compression_times;
      vector<long long> paths_2bits_compression_times_total;

    vector<long long> tree_decompression_times;
    vector<long long> tree_decompression_times_total;
    vector<long long> old_decompression_times;
    vector<long long> old_decompression_times_total;
    vector<long long> rle_decompression_times;
    vector<long long> rle_decompression_times_total;
    vector<long long> straights_decompress_times;
    vector<long long> straights_decompress_times_total;
    vector<long long> straights_huffman_decompression_times;
    vector<long long> straights_huffman_decompression_times_total;
    vector<long long> reduced_edgebits_decompression_times;
    vector<long long> reduced_edgebits_decompression_times_total;
      vector<long long> paths_2bits_decompression_times;
      vector<long long> paths_2bits_decompression_times_total;

    vector<string> filenames;
    vector<string> categories;
    vector<double> kBSizes;


    std::unordered_set<std::string> category_set = {
        "icon_64",
      //   "icon_512",
      //   "photo_kodak",
      //   "photo_tecnick",
      //   "photo_wikipedia",
      //   "pngimg",
      //   "screenshot_web",
        "screenshot_game",
      //   "textures_photo",
      //   "textures_pk",
      //   "textures_pk01",
      //   "textures_pk02",
      //   "textures_plants"
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
    //showImg = false;
    bool writeToFile = true;
    writeToFile = !single_image;
    writeToFile = false;

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

            tree_compression_times.push_back(compImg.tree_compression_time);
            old_compression_times.push_back(compImg.old_compression_time);
            rle_compression_times.push_back(compImg.rle_compression_time);
            straights_compress_times.push_back(compImg.straights_compression_time);
            straights_huffman_compression_times.push_back(compImg.straights_huffman_compression_time);
            reduced_edgebits_compression_times.push_back(compImg.reduced_edgebits_compression_time);
            paths_2bits_compression_times.push_back(compImg.paths2bit_compression_time);
            compression_times.push_back(compression_time);
            //compression_times_total.push_back(comp.getCompressionTime());

            //double compression_rate = getCompressionRate(compImg.colorVector, compImg.paths, compImg.originalImage);
            //std::cout << "Compression Rate: " << compression_rate << std::endl;
            tree_compression_rates.push_back(compImg.treeCompressionRate);
            //compression_rates_total.push_back(compression_rate);

            old_compression_rates.push_back(compImg.oldCompressionRate);
            reducedEdgeBitsCompressionRates.push_back(compImg.reducedEdgeBitsCompressionRate);
            //old_compression_rates_total.push_back(compImg.oldCompressionRate);

            rle_compression_rates.push_back(compImg.rleCompressionRate);

            straights_compress_rates.push_back(compImg.straightsCompressionRate);
            straights_huffman_compress_rates.push_back(compImg.straightsHuffmanCompressionRate);

            paths2bit_compression_rates.push_back(compImg.paths2bit_compression_rate);
            
            //cv::Mat img;
            //img = cv::imread(dirEntry.path().string(), cv::IMREAD_COLOR);
            
            //stateless approach
            start = std::chrono::high_resolution_clock::now();
            decompInfo decomp_info = reconstructImage(compImg, showImg); 
            bool success = decomp_info.success;
            end = std::chrono::high_resolution_clock::now();
            auto decompression_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            decompression_times.push_back(decompression_time);

            tree_decompression_times.push_back(decomp_info.tree_decompression_time);
            old_decompression_times.push_back(decomp_info.old_decompression_time);
            rle_decompression_times.push_back(decomp_info.rle_decompression_time);
            straights_decompress_times.push_back(decomp_info.straights_decompression_time);
            straights_huffman_decompression_times.push_back(decomp_info.straights_huffman_decompression_time);
            reduced_edgebits_decompression_times.push_back(decomp_info.reduced_edgebits_decompression_time);
            paths_2bits_decompression_times.push_back(decomp_info.path2bits_decompression_time);

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


        // if(writeToFile){
        //     // WRITE COMPRESSION RATES TO FILE
        //     writeToOutput(entry, tree_compression_rates, "compression_rates");
        //     //writeToOutput(entry, old_compression_rates, "old_compression_rates");
        //     writeToOutput(entry, compression_times, "compression_times");
        //     writeToOutput(entry, decompression_times, "decompression_times");
        //     //writeToOutput(entry, multicut_percentages, "multicut_percentages");
        //     //writeToOutput(entry, disconnected_components, "disconnected_components");
        //     //writeToOutput(entry, pixel_sizes, "pixel_sizes");
        //     writeToOutput(entry, rle_compression_rates, "rle_compression_rates");
        //     //writeToOutput(entry, twobit_paths_amounts, "twobit_paths_amounts");
        //     writeToOutput(entry, straights_compress_rates, "straights_compress_rates");
        //     writeToOutput(entry, straights_huffman_compress_rates, "straights_huffman_compression_rates");
        // }


        // std::ofstream total_file("code/total_csv.csv");
        // if(total_file){
        //     total_file << 
        // }

        tree_compression_rates_total.insert(tree_compression_rates_total.end(), tree_compression_rates.begin(), tree_compression_rates.end());
        tree_compression_rates.clear();

        old_compression_rates_total.insert(old_compression_rates_total.end(), old_compression_rates.begin(), old_compression_rates.end());
        old_compression_rates.clear();

        reduced_edgebits_compression_rates_total.insert(reduced_edgebits_compression_rates_total.end(), reducedEdgeBitsCompressionRates.begin(), reducedEdgeBitsCompressionRates.end());
        reducedEdgeBitsCompressionRates.clear();

        compression_times_total.insert(compression_times_total.end(), compression_times.begin(), compression_times.end());
        compression_times.clear();

        decompression_times_total.insert(decompression_times_total.end(), decompression_times.begin(), decompression_times.end());
        decompression_times.clear();

        rle_compression_rates_total.insert(rle_compression_rates_total.end(), rle_compression_rates.begin(), rle_compression_rates.end());
        rle_compression_rates.clear();

        straights_compression_rates_total.insert(straights_compression_rates_total.end(), straights_compress_rates.begin(), straights_compress_rates.end());
        straights_compress_rates.clear();

        straights_huffman_compression_rates_total.insert(straights_huffman_compression_rates_total.end(), straights_huffman_compress_rates.begin(), straights_huffman_compress_rates.end());
        straights_huffman_compress_rates.clear();

        paths2bit_compression_rates_total.insert(paths2bit_compression_rates_total.end(), paths2bit_compression_rates.begin(), paths2bit_compression_rates.end());
        paths2bit_compression_rates.clear();

        tree_compression_times_total.insert(tree_compression_times_total.end(), tree_compression_times.begin(), tree_compression_times.end());
        tree_compression_times.clear();

        old_compression_times_total.insert(old_compression_times_total.end(), old_compression_times.begin(), old_compression_times.end());
        old_compression_times.clear();

        rle_compression_times_total.insert(rle_compression_times_total.end(), rle_compression_times.begin(), rle_compression_times.end());
        rle_compression_times.clear();

        straights_compress_times_total.insert(straights_compress_times_total.end(), straights_compress_times.begin(), straights_compress_times.end());
        straights_compress_times.clear();

        straights_huffman_compression_times_total.insert(straights_huffman_compression_times_total.end(), straights_huffman_compression_times.begin(), straights_huffman_compression_times.end());
        straights_huffman_compression_times.clear();

        reduced_edgebits_compression_times_total.insert(reduced_edgebits_compression_times_total.end(), reduced_edgebits_compression_times.begin(), reduced_edgebits_compression_times.end());
        reduced_edgebits_compression_times.clear();

        paths_2bits_compression_times_total.insert(paths_2bits_compression_times_total.end(), paths_2bits_compression_times.begin(), paths_2bits_compression_times.end());
        paths_2bits_compression_times.clear();

        tree_decompression_times_total.insert(tree_decompression_times_total.end(), tree_decompression_times.begin(), tree_decompression_times.end());
        tree_decompression_times.clear();

        old_decompression_times_total.insert(old_decompression_times_total.end(), old_decompression_times.begin(), old_decompression_times.end());
        old_decompression_times.clear();

        rle_decompression_times_total.insert(rle_decompression_times_total.end(), rle_decompression_times.begin(), rle_decompression_times.end());
        rle_decompression_times.clear();

        straights_decompress_times_total.insert(straights_decompress_times_total.end(), straights_decompress_times.begin(), straights_decompress_times.end());
        straights_decompress_times.clear();

        straights_huffman_decompression_times_total.insert(straights_huffman_decompression_times_total.end(), straights_huffman_decompression_times.begin(), straights_huffman_decompression_times.end());
        straights_huffman_decompression_times.clear();

        reduced_edgebits_decompression_times_total.insert(reduced_edgebits_decompression_times_total.end(), reduced_edgebits_decompression_times.begin(), reduced_edgebits_decompression_times.end());
        reduced_edgebits_decompression_times.clear();

      paths_2bits_decompression_times_total.insert(paths_2bits_decompression_times_total.end(), paths_2bits_decompression_times.begin(), paths_2bits_decompression_times.end());
      paths_2bits_decompression_times.clear();


        // multicut_percentages.clear();
        // disconnected_components.clear();
        pixel_sizes.clear();
    }

if(writeToFile){
      ofstream csvFile;
      if(single_image){
            csvFile.open("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/output_files/mc_results_single.csv");
      } else {
            csvFile.open("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/output_files/mc_results.csv");
      }
      if (!csvFile.is_open()) {
            cerr << "Error: Unable to open CSV file for writing." << endl;
            return -1;
      }

      if (!(filenames.size() == categories.size() &&
        filenames.size() == old_compression_rates_total.size() &&
        filenames.size() == tree_compression_rates_total.size() &&
        filenames.size() == rle_compression_rates_total.size() &&
        filenames.size() == straights_huffman_compression_rates_total.size() &&
        filenames.size() == reduced_edgebits_compression_rates_total.size() &&
        filenames.size() == paths2bit_compression_rates_total.size() &&
        filenames.size() == tree_compression_times_total.size() &&
        filenames.size() == tree_decompression_times_total.size() &&
        filenames.size() == old_compression_times_total.size() &&
        filenames.size() == old_decompression_times_total.size() &&
        filenames.size() == rle_compression_times_total.size() &&
        filenames.size() == rle_decompression_times_total.size() &&
        filenames.size() == straights_huffman_compression_times_total.size() &&
        filenames.size() == straights_huffman_decompression_times_total.size() &&
        filenames.size() == reduced_edgebits_compression_times_total.size() &&
        filenames.size() == reduced_edgebits_decompression_times_total.size() &&
        filenames.size() == paths_2bits_compression_times_total.size() &&
        filenames.size() == paths_2bits_decompression_times_total.size())) {
      std::cerr << "Error: Vector size mismatch." << std::endl;

      if (filenames.size() != categories.size()) {
      std::cerr << "Mismatch: filenames.size() = " << filenames.size() 
              << ", categories.size() = " << categories.size() << std::endl;
      }
      if (filenames.size() != old_compression_rates_total.size()) {
      std::cerr << "Mismatch: filenames.size() = " << filenames.size() 
              << ", old_compression_rates_total.size() = " << old_compression_rates_total.size() << std::endl;
      }
      if (filenames.size() != tree_compression_rates_total.size()) {
      std::cerr << "Mismatch: filenames.size() = " << filenames.size() 
              << ", tree_compression_rates_total.size() = " << tree_compression_rates_total.size() << std::endl;
      }
      if (filenames.size() != rle_compression_rates_total.size()) {
      std::cerr << "Mismatch: filenames.size() = " << filenames.size() 
              << ", rle_compression_rates_total.size() = " << rle_compression_rates_total.size() << std::endl;
      }
      if (filenames.size() != straights_huffman_compression_rates_total.size()) {
      std::cerr << "Mismatch: filenames.size() = " << filenames.size() 
              << ", straights_huffman_compression_rates_total.size() = " << straights_huffman_compression_rates_total.size() << std::endl;
      }
      if (filenames.size() != reduced_edgebits_compression_rates_total.size()) {
      std::cerr << "Mismatch: filenames.size() = " << filenames.size() 
              << ", reduced_edgebits_compression_rates_total.size() = " << reduced_edgebits_compression_rates_total.size() << std::endl;
      }
      if (filenames.size() != paths2bit_compression_rates_total.size()) {
      std::cerr << "Mismatch: filenames.size() = " << filenames.size() 
              << ", paths2bit_compression_rates_total.size() = " << paths2bit_compression_rates_total.size() << std::endl;
      }
      if (filenames.size() != tree_compression_times_total.size()) {
      std::cerr << "Mismatch: filenames.size() = " << filenames.size() 
              << ", tree_compression_times_total.size() = " << tree_compression_times_total.size() << std::endl;
      }
      if (filenames.size() != tree_decompression_times_total.size()) {
      std::cerr << "Mismatch: filenames.size() = " << filenames.size() 
              << ", tree_decompression_times_total.size() = " << tree_decompression_times_total.size() << std::endl;
      }
      if (filenames.size() != old_compression_times_total.size()) {
      std::cerr << "Mismatch: filenames.size() = " << filenames.size() 
              << ", old_compression_times_total.size() = " << old_compression_times_total.size() << std::endl;
      }
      if (filenames.size() != old_decompression_times_total.size()) {
      std::cerr << "Mismatch: filenames.size() = " << filenames.size() 
              << ", old_decompression_times_total.size() = " << old_decompression_times_total.size() << std::endl;
      }
      if (filenames.size() != rle_compression_times_total.size()) {
      std::cerr << "Mismatch: filenames.size() = " << filenames.size() 
              << ", rle_compression_times_total.size() = " << rle_compression_times_total.size() << std::endl;
      }
      if (filenames.size() != rle_decompression_times_total.size()) {
      std::cerr << "Mismatch: filenames.size() = " << filenames.size() 
              << ", rle_decompression_times_total.size() = " << rle_decompression_times_total.size() << std::endl;
      }
      if (filenames.size() != straights_huffman_compression_times_total.size()) {
      std::cerr << "Mismatch: filenames.size() = " << filenames.size() 
              << ", straights_huffman_compression_times_total.size() = " << straights_huffman_compression_times_total.size() << std::endl;
      }
      if (filenames.size() != straights_huffman_decompression_times_total.size()) {
      std::cerr << "Mismatch: filenames.size() = " << filenames.size() 
              << ", straights_huffman_decompression_times_total.size() = " << straights_huffman_decompression_times_total.size() << std::endl;
      }
      if (filenames.size() != reduced_edgebits_compression_times_total.size()) {
      std::cerr << "Mismatch: filenames.size() = " << filenames.size() 
              << ", reduced_edgebits_compression_times_total.size() = " << reduced_edgebits_compression_times_total.size() << std::endl;
      }
      if (filenames.size() != reduced_edgebits_decompression_times_total.size()) {
      std::cerr << "Mismatch: filenames.size() = " << filenames.size() 
              << ", reduced_edgebits_decompression_times_total.size() = " << reduced_edgebits_decompression_times_total.size() << std::endl;
      }
      if (filenames.size() != paths_2bits_compression_times_total.size()) {
      std::cerr << "Mismatch: filenames.size() = " << filenames.size() 
              << ", paths_2bits_compression_times_total.size() = " << paths_2bits_compression_times_total.size() << std::endl;
      }
      if (filenames.size() != paths_2bits_decompression_times_total.size()) {
      std::cerr << "Mismatch: filenames.size() = " << filenames.size() 
              << ", paths_2bits_decompression_times_total.size() = " << paths_2bits_decompression_times_total.size() << std::endl;
      }

      return -1;
}


        csvFile << "filename,category,old_rate,tree_rate,rle_rate,straights_rate,straights_huffman_rate,reduced_edgebits_rate,paths_2bit_rate,tree_comp_time,tree_decomp_time,old_comp_time, old_decomp_time,rle_comp_time,rle_decomp_time,straights_comp_time,straights_decomp_time,huffman_comp_time,huffman_decomp_time,reduced_comp_time,reduced_decomp_time,paths_2bit_comp_time,paths_2bit_decomp_time\n";
        for (size_t i = 0; i < filenames.size(); ++i) {
            csvFile << filenames[i] << ","
                    << categories[i] << ","
                    // << pixel_sizes_total[i] << ","
                    << old_compression_rates_total[i] << ","
                    << tree_compression_rates_total[i] << ","
                    << rle_compression_rates_total[i] << ","
                     << straights_compression_rates_total[i] << ","
                    << straights_huffman_compression_rates_total[i] << ","
                    << reduced_edgebits_compression_rates_total[i] << ","
                    << paths2bit_compression_rates_total[i] << ","
                    << tree_compression_times_total[i] << ","
                    << tree_decompression_times_total[i] << ","
                    << old_compression_times_total[i] << ","
                    << old_decompression_times_total[i] << ","
                    << rle_compression_times_total[i] << ","
                    << rle_decompression_times_total[i] << ","
                    << straights_compress_times_total[i] << ","
                    << straights_decompress_times_total[i] << ","
                    << straights_huffman_compression_times_total[i] << ","
                    << straights_huffman_decompression_times_total[i] << ","
                    << reduced_edgebits_compression_times_total[i] << ","
                    << reduced_edgebits_decompression_times_total[i] << ","
                    << paths_2bits_compression_times_total[i] << ","
                    << paths_2bits_decompression_times_total[i] << "\n";


                    //<< decompression_times_total[i] << "\n";
        }

        csvFile.close();
    }

    auto completeEnd = std::chrono::high_resolution_clock::now();
    auto completeTime = std::chrono::duration_cast<std::chrono::milliseconds>(completeEnd - completeStart).count();
    std::cout << "Total time: " << completeTime << " ms" << std::endl;

    return 0;
}
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


using namespace std;

// g++ -std=c++11 -o multicut multicut.cpp Graph.cpp $(pkg-config --cflags --libs opencv4); ./multicut

/**
 * @brief writes values of arbitrary type into a csv file 
 * @param p1 filepath
 * @param values values to be added to the file comma-separated 
 * @param category image category for allocating the correct output folder 
 */
template<typename T>
void writeToOutput(const std::filesystem::path& p1, const std::vector<T>& values, const std::string category) {
    // Create the output file path using the provided path p1
    std::filesystem::create_directories("2ndoutput/" + category);
    std::ofstream outputFile("2ndoutput/" + category + "/output_" + p1.filename().string() + ".csv");

    // Check if the file opened successfully
    if (!outputFile.is_open()) {
        std::cerr << "Error: Unable to open the file." << std::endl;
        return;
    }

    // Iterate over the vector and write its elements to the file
    for (size_t i = 0; i < values.size(); ++i) {
        outputFile << values[i]; // Write the element

        // Add a comma if it's not the last element
        if (i != values.size() - 1) {
            outputFile << ",";
        }
    }
        
    outputFile.close();
}

/**
 * @brief counts number of JPG and PNG files in a directory 
 * @param parentDir directory path 
 * @return number of images (.jpg and .png)
 */
int countImgFiles(const std::filesystem::path& parentDir) {
    int count = 0;
    for (const auto& entry : std::filesystem::directory_iterator(parentDir)) {
        if (!entry.is_directory()) {
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


struct CompressedImage{
    std::vector<RGB> colorVector;
    PathInfoVector paths;
    cv::Mat originalImage;
    PathInfoVector pathInfoVector2bit;
    RLEVector rleVector;
    Straights straights;
    std::vector<bool> regionColorBitString;
};

/**
 * @brief main function loading image files and controlling compression and decompression procedure 
 * 
 */
int main() {
    vector<double> compression_rates;
    vector<double> old_compression_rates; 
    vector<double> rle_compression_rates;
    vector<double> straights_compress_rates;
    vector<long long> compression_times;
    vector<long long> decompression_times;
    vector<double> multicut_percentages;
    vector<int> disconnected_components;
    vector<int> pixel_sizes; 
    vector<int> twobit_paths_amounts;

    vector<double> compression_rates_total;
    vector<double> old_compression_rates_total; 
    vector<long long> compression_times_total;
    vector<long long> decompression_times_total;
    vector<double> multicut_percentages_total;
    vector<int> disconnected_components_total;
    vector<int> pixel_sizes_total; 

    vector<string> filenames;
    vector<string> categories;
    vector<double> kBSizes;

    // auto rle_result = getRLE({true, true, true, true, true, true , true, true});
    // if(std::get<0>(rle_result).empty()) {
    //     std::cerr << "Error: The result at tuple index 0 is empty." << std::endl;
    // }
    // if (std::get<1>(rle_result).empty()) {
    //     std::cerr << "Error: The result at tuple index 1 is empty." << std::endl;
    // }
    // auto reconstructed = reconstructRLE(std::get<0>(rle_result), std::get<1>(rle_result), std::get<2>(rle_result));


    // cv::Mat milk_img = cv::imread("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/code/images/pngimg/macaron_PNG35.png", cv::IMREAD_UNCHANGED);
    // if (milk_img.empty()) {
    //     std::cerr << "Error: Unable to load image." << std::endl;
    //     return -1;
    // }
    // cv::namedWindow("Milk Image", cv::WINDOW_AUTOSIZE);
    // cv::imshow("Milk Image", milk_img);
    // cv::waitKey(0);

    
    string imgDir = "/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/5x5example";
    imgDir = "/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/code/images/icon_512";
    std::filesystem::path parentDir = "/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/code/images";
    //parentDir = "/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code";
    int imgCount = countImgFiles(parentDir);
    int progress = 0;

    std::filesystem::path p1 { imgDir };
    int count {};
    int i = 0;

    
    
    long long total_time_set_multicut = 0;
    long long total_time_reconstruct_multicut = 0;
    
    for (auto& p : std::filesystem::directory_iterator(p1))
    {
        ++count;
    }

    /*
    Compressor testcomp("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/test_img/A_House_in_California.png");
    testcomp.compressImage();
    std::cout << "comp rate: " << testcomp.getCompressionRate() << std::endl;
    //std::cout << "total bits: " << testcomp.getImgSize() << std::endl;
    return 0;
    */

    //Compressor volcomp("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/5x5example/tree5x5.png", imgDir);
    //volcomp.compressVolume();

    std::filesystem::path category = "screenshot_game";
    std::filesystem::path categoryPath = parentDir / category;
    imgCount = countDirectImgFiles(categoryPath);

    for (const auto& entry : std::filesystem::directory_iterator(parentDir)) {
        if (!entry.is_directory() || entry.path().filename().string() != category.string()) {
            continue; 
        }
        for (const auto& dirEntry : std::filesystem::directory_iterator(entry)){
            if(dirEntry.path().extension().string() != ".png" || dirEntry.path().filename().string() != "A_House_in_California.png"
            ){
                continue;
            }

            Compressor comp(dirEntry.path().string());
            std::cout << "Compressing: " << dirEntry.path().filename().string() << std::endl;
            //returning color vector, path vector, original image

            //stateful approach
            //auto compressed_image = comp.compressImage();

            //stateless approach
            auto compressed_image = compress(dirEntry.path().string());
            CompressedImage compImg {
                std::get<0>(compressed_image), // color vector
                std::get<1>(compressed_image), // path vector
                std::get<2>(compressed_image), // original image
                std::get<3>(compressed_image), // path vector 2bit
                std::get<4>(compressed_image), // rle vector
                std::get<5>(compressed_image), // straights
                std::get<6>(compressed_image)  // region color bit string
            };


            std::string filename = dirEntry.path().filename().string();
            // Remove comma if it exists in the filename
            filename.erase(std::remove(filename.begin(), filename.end(), ','), filename.end());
            filenames.push_back(filename);
            categories.push_back(entry.path().filename().string());

            compression_times.push_back(comp.getCompressionTime());
            compression_times_total.push_back(comp.getCompressionTime());

            double compression_rate = getCompressionRate(compImg.colorVector, compImg.paths, compImg.originalImage);
            //std::cout << "Compression Rate: " << compression_rate << std::endl;
            compression_rates.push_back(compression_rate);
            compression_rates_total.push_back(compression_rate);


            //double old_compression_rate = getOldCompressionRate(compImg.originalImage, compImg.colorVector);            
            //old_compression_rates.push_back(old_compression_rate);
            //old_compression_rates_total.push_back(old_compression_rate);

            //double rle_comp_rate = comp.getRLECompressionRate();
            //double rle_comp_rate = getRLECompressionRate(compImg.colorVector, compImg.rleVector, compImg.originalImage);
            //std::cout << "RLE Compression Rate: " << rle_comp_rate << std::endl;
            //rle_compression_rates.push_back(rle_comp_rate);

            //double straights_comp_rate = comp.getStraightsCompressionRate();
            //double straights_comp_rate = getStraightsCompressionRate(compImg.colorVector, compImg.straights, compImg.originalImage);
            //std::cout << "Straights Compression Rate: " << straights_comp_rate << std::endl;
            //straights_compress_rates.push_back(straights_comp_rate);

            std::vector<bool> edgeBits01;
            //TODO: edgeBits01 is being computed again 
            //double multicut_percentage = getMulticutPercentage(setEdgeBits(compImg.originalImage, edgeBits01,{compImg.originalImage.cols,1}));
            //multicut_percentages.push_back(multicut_percentage);
            //multicut_percentages_total.push_back(multicut_percentage);
            //std::cout << "Multicut Percentage: " << comp.getMulticutPercentage() << std::endl;
            

            //disconnected_components.push_back(comp.getDisconnectedComponents());
            //disconnected_components_total.push_back(comp.getDisconnectedComponents());
            //std::cout << "Disconnected Components: " << comp.getDisconnectedComponents() << std::endl;

            pixel_sizes.push_back(comp.getMulticut().getVertices());
            pixel_sizes_total.push_back(comp.getMulticut().getVertices());

            kBSizes.push_back(comp.getkBSize());

            // get the number of the 2-bit paths 
            twobit_paths_amounts.push_back(std::get<4>(compressed_image).size());
            
            cv::Mat img;// = std::get<2>(compressed_image);
            img = cv::imread(dirEntry.path().string(), cv::IMREAD_COLOR);
            Decompressor decomp(std::get<0>(compressed_image), std::get<1>(compressed_image), comp.getMulticut().edgeBits01.size(), img.cols, img.rows, img, std::get<3>(compressed_image), std::get<4>(compressed_image), std::get<5>(compressed_image), std::get<6>(compressed_image));
            decomp.reconstructImage();
            decompression_times.push_back(decomp.getDecompressionTime());
            decompression_times_total.push_back(decomp.getDecompressionTime());
            
        

            progress++;
            //std::cout << progress << "/" << imgCount << std::endl;
            //printProgressBar(progress, imgCount);
            ++i;

        }

    //writeToOutput(p1, compression_times);
    //writeToOutput(p1, compression_rates);

    /*
    // Print compression times
    cout << "Compression Times:" << endl;
    for (const auto& time : compression_times) {
        cout << time << " milliseconds" << endl;
    }

    // Print decompression times
    cout << "Decompression Times:" << endl;
    for (const auto& time : decompression_times) {
        cout << time << " milliseconds" << endl;
    }
    */ 

    // WRITE COMPRESSION RATES TO FILE
    writeToOutput(entry, compression_rates, "compression_rates");
    //writeToOutput(entry, old_compression_rates, "old_compression_rates");
    writeToOutput(entry, compression_times, "compression_times");
    //writeToOutput(entry, decompression_times, "decompression_times");
    //writeToOutput(entry, multicut_percentages, "multicut_percentages");
    //writeToOutput(entry, disconnected_components, "disconnected_components");
    //writeToOutput(entry, pixel_sizes, "pixel_sizes");
    writeToOutput(entry, rle_compression_rates, "rle_compression_rates");
    writeToOutput(entry, twobit_paths_amounts, "twobit_paths_amounts");
    writeToOutput(entry, straights_compress_rates, "straights_compress_rates");

    compression_rates.clear();
    old_compression_rates.clear();
    compression_times.clear();
    decompression_times.clear();
    multicut_percentages.clear();
    disconnected_components.clear();
    pixel_sizes.clear();
    /*
    std::cout << entry.path().filename().string() << std::endl;

    double mean = std::accumulate(compression_rates.begin(), compression_rates.end(), 0.0) / compression_rates.size();
    double min = *std::min_element(compression_rates.begin(), compression_rates.end());
    double max = *std::max_element(compression_rates.begin(), compression_rates.end());
    std::cout << "\nMean Compression Rate: " << mean << std::endl;
    std::cout << "Min Compression Rate: " << min << std::endl;
    std::cout << "Max Compression Rate: " << max << std::endl;

    mean = std::accumulate(old_compression_rates.begin(), old_compression_rates.end(), 0.0) / old_compression_rates.size();
    min = *std::min_element(old_compression_rates.begin(), old_compression_rates.end());
    max = *std::max_element(old_compression_rates.begin(), old_compression_rates.end());
    std::cout << "Mean Old Compression Rate: " << mean << std::endl;
    std::cout << "Min Old Compression Rate: " << min << std::endl;
    std::cout << "Max Old Compression Rate: " << max << std::endl;
    */

    //std::cout << total_time_set_multicut/i << endl;
    //std::cout << total_time_reconstruct_multicut/i << endl;
    }

    /*

    ofstream csvFile("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV-Project/mlcv-multicut/code/output_files/mc_results.csv");
    if (!csvFile.is_open()) {
        cerr << "Error: Unable to open CSV file for writing." << endl;
        return -1;
    }

    csvFile << "filename,category,pixel_size,encode_ms,decode_ms,size_kb,rate\n";
    for (size_t i = 0; i < filenames.size(); ++i) {
        csvFile << filenames[i] << ","
                << categories[i] << ","
                << pixel_sizes_total[i] << ","
                << compression_times_total[i] << ","
                << decompression_times_total[i] << ","
                << kBSizes[i] << ","
                << compression_rates_total[i] << "\n";
    }

    csvFile.close();
    */
    return 0;
}
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
// #include "Graph.h"
#include <chrono>
#include <filesystem> 
#include <fstream>
#include <numeric>
// #include "Compressor.h"
// #include "Decompressor.h"
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
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <img_file>\n";
        return 1;
    }

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

    vector<double> tree_compression_times;
    vector<double> tree_compression_times_total;
    vector<double> old_compression_times;
    vector<double> old_compression_times_total;
    vector<double> rle_compression_times;
    vector<double> rle_compression_times_total;
    vector<double> straights_compress_times;
    vector<double> straights_compress_times_total;
    vector<double> straights_huffman_compression_times;
    vector<double> straights_huffman_compression_times_total;
    vector<double> reduced_edgebits_compression_times;
    vector<double> reduced_edgebits_compression_times_total;
      vector<double> paths_2bits_compression_times;
      vector<double> paths_2bits_compression_times_total;

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


      // test values
      vector<int> tree_3bit_bits;
      vector<int> tree_current_bits;
      vector<int> tree_start_bits;
      vector<int> paths_2bit_bits;
      vector<int> paths_2bit_start_bits;
      vector<int> paths_2bit_components;
      vector<int> paths_2bit_crossings;
      vector<int> regions;
      vector<int> rle_direction_bits;
      vector<int> region_colors_bits;
      vector<int> dpcm_huffman_bits;
      vector<int> deflate_bits;
      vector<int> deflate_bits_unseparated;
      vector<int> treeMCBits;
      vector<int> edgeBitsMCBits;
      vector<int> straights_MCBits;

      vector<double> setEdgeBitsTime;
      vector<double> read_img_time;
      vector<double> region_color_dfs_time;
      vector<double> region_color_UF_time;
      vector<double> dpcm_huffman_time;
      vector<double> dpcm_huffman_bitstring_time;
      vector<double> tree_construction_time;
      vector<double> tree_bitstring_time;
        vector<double> dec_construction_time;
        vector<double> dec_bitstring_time;
        vector<double> sls_construction_time;
        vector<double> sls_bitstring_time;
        vector<double> rcmv_construction_time;
        vector<double> rcmv_bitstring_time;
        vector<double> deflate_edgebits_times;
        vector<double> inflate_edgebits_times;

      vector<double> rebuild_dpcm_huffman_time;
      vector<double> decode_colors_time;
      vector<double> assemble_tree_paths_time;
      vector<double> reconstruct_tree_edgebits_time;
      vector<double> dfs_reconstruction_time;
      // vector<int> UF_reconstruction_time;
      vector<double> reconstruct_rcmv_time;
        vector<double> reconstruct_rcmv_cmv_time;
        vector<double> dec_reconstruction_time;
        vector<double> dec_cmv_reconstruction_time;
        vector<double> sls_reconstruction_time;
        vector<double> sls_cmv_reconstruction_time;

    vector<double> write_times;
    vector<double> read_times;

      vector<long long> total_tree_bits;
      vector<int> new2bitDirectionBits;
      vector<int> edgebits;
      vector<int> rcmv_bits;
      vector <int> huffman_MCBits;
      vector<int> dpcm_deflate_bits;
        vector<int> dpcm_deflate_bits_no_inter;

        vector<int> bitsfortransferingcodes;
        vector<int> bitsfortransferingfrequencymap;

        vector<double> tree_bpp;
        vector<double> avg_straight_length;

        vector<double> edgebits_deflate_rates;


    vector<string> filenames;
    vector<string> categories;
    vector<double> kBSizes;
    


    std::unordered_set<std::string> category_set = {
        // "icon_64",
        // "icon_512",
        // "photo_kodak",
        // "photo_tecnick",
        // "photo_wikipedia",
        // "pngimg",
        "screenshot_web",
        "screenshot_game",
        // "textures_photo",
        // "textures_pk",
        // "textures_pk01",
        // "textures_pk02",
        // "textures_plants",

        // "sample_images",
        // "screenshot_game_reduced2"
    };

    // std::string single_image_txt;
    // if (std::getline(inputFile, single_image_txt)) {
    //     // Process the single line read from the file
    //     std::cout << single_image_txt << std::endl;
    // }

    // inputFile.close();

    // control parameters 
    bool single_image = true;
    // string single_image_name = single_image_txt;
    string single_image_name = argv[1];
    bool showImg = single_image;
    //showImg = false;
    bool writeToFile = true;
    writeToFile = !single_image;
    writeToFile = false;
    
    // bool test_mode = (category_set.count("screenshot_game_reduced") > 0) && 
    //                  (category_set.size() == 1);
    bool test_mode = true;

    // std::filesystem::path parentDir = "/Users/jalell/Documents/images";
    // int imgCount = countImgFiles(parentDir, category_set);
    // int progress = 0;    

    std::filesystem::path input_image = single_image_name;

    if (!std::filesystem::exists(input_image)) {
        std::cerr << "Error: Image file not found: " << input_image << std::endl;
        return 1;
    }



    // for (const auto& entry : std::filesystem::directory_iterator(parentDir)) {
    //     if (!entry.is_directory() 
    //     || category_set.find(entry.path().filename().string()) == category_set.end()
    //     ) {
    //         continue; 
    //     }
    //     for (const auto& dirEntry : std::filesystem::directory_iterator(entry)){
            // if(dirEntry.path().extension().string() != ".png" //|| dirEntry.path().filename().string() != "blek_1.png"
            // ){
            //     continue;
            // }
            // if(single_image && dirEntry.path().filename().string() != single_image_name){
            //     continue;
            // }

            

            // std::cout << "\nCompressing: " << dirEntry.path().filename().string() << "\nCategory: " << entry.path().filename().string() << std::endl;
            //returning color vector, path vector, original image

            //stateless approach
            auto start = std::chrono::high_resolution_clock::now();
            auto compImg = compress(input_image.string());
            auto end = std::chrono::high_resolution_clock::now();
            auto compression_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

            std::string filename = input_image.string();
            // Remove comma if it exists in the filename
            filename.erase(std::remove(filename.begin(), filename.end(), ','), filename.end());
            filenames.push_back(filename);
            categories.push_back(input_image.string());

            start = std::chrono::high_resolution_clock::now();
            std::string bin_output = "compressed_" + input_image.stem().string() + ".bin";
            std::ofstream outFile(bin_output, std::ios::binary);
            if (outFile.is_open()) {
                uint8_t byte = 0;
                int count = 0;
                for (bool bit : compImg.pathsBitString) {
                    byte = (byte << 1) | bit;
                    count++;
                    if (count == 8) {
                        outFile.put(byte);
                        byte = 0;
                        count = 0;
                    }
                }
                if (count > 0) {  // pad remaining bits
                    byte <<= (8 - count);
                    outFile.put(byte);
                }
                outFile.close();
            }
            // std::cout << "pathsBitString size: " << compImg.pathsBitString.size() << std::endl;
            end = std::chrono::high_resolution_clock::now();
            // std::cout << "Time to write pathsBitString to file: " 
            //           << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() 
            //           << " ms\n";
            double write_time = std::chrono::duration<double, std::milli>(end - start).count();
            write_times.push_back(write_time);

            tree_compression_times.push_back(compImg.tree_compression_time);
            old_compression_times.push_back(compImg.old_compression_time);
            rle_compression_times.push_back(compImg.rle_compression_time);
            straights_compress_times.push_back(compImg.straights_compression_time);
            straights_huffman_compression_times.push_back(compImg.straights_huffman_compression_time);
            reduced_edgebits_compression_times.push_back(compImg.reduced_edgebits_compression_time);
            paths_2bits_compression_times.push_back(compImg.paths2bit_compression_time);
            compression_times.push_back(compression_time);
            //compression_times_total.push_back(comp.getCompressionTime());
            // edgebits_deflate_rates.push_back(compImg.oldCompressionRate);
            huffman_MCBits.push_back(compImg.huffman_MCBits);
            deflate_edgebits_times.push_back(compImg.deflate_edgebits_time);

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
            
            tree_3bit_bits.push_back(compImg.threeBitCount);
            tree_current_bits.push_back(compImg.currentTreeDirectionBits);
            tree_start_bits.push_back(compImg.tree_start_bits);
            disconnected_components.push_back(compImg.disconnectedComponents);
            regions.push_back(compImg.regionColors.size());
            paths_2bit_bits.push_back(compImg.paths2bit_direction_bits);
            paths_2bit_start_bits.push_back(compImg.paths2bit_start_bits);
            paths_2bit_components.push_back(compImg.paths2bit_components);
            rle_direction_bits.push_back(compImg.rle_direction_bits);
            region_colors_bits.push_back(compImg.region_colors_bits);
            dpcm_huffman_bits.push_back(compImg.dpcm_huffman_bits);
            deflate_bits.push_back(compImg.deflate_bits);
                deflate_bits_unseparated.push_back(compImg.deflate_bits_unseparated);
            region_color_dfs_time.push_back(compImg.region_color_dfs_time);
            region_color_UF_time.push_back(compImg.region_color_UF_time);
            dpcm_huffman_time.push_back(compImg.dpcm_huffman_time);
            dpcm_huffman_bitstring_time.push_back(compImg.dpcm_huffman_bitstring_time);
            tree_construction_time.push_back(compImg.tree_construction_time);
            tree_bitstring_time.push_back(compImg.tree_bitstring_time);
            pixel_sizes.push_back(compImg.originalImage.cols * compImg.originalImage.rows);
            multicut_percentages.push_back(compImg.multicutPercentage);
            total_tree_bits.push_back(compImg.total_tree_bits);
            new2bitDirectionBits.push_back(compImg.new2bitDirectionBits);
            paths_2bit_crossings.push_back(compImg.paths2bit_components - compImg.disconnectedComponents);
            read_img_time.push_back(compImg.read_img_time);
            setEdgeBitsTime.push_back(compImg.setEdgeBitsTime);
            bitsfortransferingcodes.push_back(compImg.bitsfortransferingcodes);
            bitsfortransferingfrequencymap.push_back(compImg.bitsfortransferingfrequencymap);
            tree_bpp.push_back(compImg.tree_bpp);
                avg_straight_length.push_back(compImg.avg_straight_length);
            dec_construction_time.push_back(compImg.dec_construction_time);
                dec_bitstring_time.push_back(compImg.dec_bitstring_time);
                sls_construction_time.push_back(compImg.sls_construction_time);
                sls_bitstring_time.push_back(compImg.sls_bitstring_time);
                rcmv_construction_time.push_back(compImg.rcmv_construction_time);
                rcmv_bitstring_time.push_back(compImg.rcmv_bitstring_time);
            edgebits.push_back(compImg.edgeBits01.size());
            rcmv_bits.push_back(compImg.rcmv_bits);
            treeMCBits.push_back(compImg.treeMCBits);
            dpcm_deflate_bits.push_back(compImg.dpcm_deflate_bits);
            dpcm_deflate_bits_no_inter.push_back(compImg.dpcm_deflate_bits_no_inter);
            edgeBitsMCBits.push_back(compImg.edgeBitsMCBits);
            straights_MCBits.push_back(compImg.noHuffman_MCBits);
            //stateless approach
            start = std::chrono::high_resolution_clock::now();
            decompInfo decomp_info = reconstructImage(compImg, showImg, bin_output); 
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

            rebuild_dpcm_huffman_time.push_back(decomp_info.rebuild_dpcm_huffman_time);
            decode_colors_time.push_back(decomp_info.decode_colors_time);
            assemble_tree_paths_time.push_back(decomp_info.assemble_tree_paths_time);
            reconstruct_tree_edgebits_time.push_back(decomp_info.reconstruct_tree_edgebits_time);
            dfs_reconstruction_time.push_back(decomp_info.dfs_reconstruction_time);
            reconstruct_rcmv_time.push_back(decomp_info.reconstruct_rcmv_time);
            reconstruct_rcmv_cmv_time.push_back(decomp_info.reconstruct_rcmv_cmv_time);
            dec_reconstruction_time.push_back(decomp_info.dec_reconstruction_time);
            dec_cmv_reconstruction_time.push_back(decomp_info.dec_cmv_reconstruction_time);
            sls_reconstruction_time.push_back(decomp_info.sls_reconstruction_time);
            sls_cmv_reconstruction_time.push_back(decomp_info.sls_cmv_reconstruction_time);
            read_times.push_back(decomp_info.read_time);
            inflate_edgebits_times.push_back(decomp_info.inflate_edgebits_time);

            // progress++;
            //std::cout << progress << "/" << imgCount << std::endl;
            // if(!single_image){
            //     printProgressBar(progress, imgCount);
            // }

            if (!success) {
                compression_successful = false;
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
        // pixel_sizes.clear();
    

    return 0;
}

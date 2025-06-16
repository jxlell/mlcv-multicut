#include <opencv2/opencv.hpp>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdlib>
#include <sstream>

namespace fs = std::filesystem;

bool run_command(const std::string& command) {
    int result = std::system(command.c_str());
    return result == 0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <img_folder>\n";
        return 1;
    }

    std::string folder_path = argv[1];
    std::ofstream csv("compression_results_lossless_all.csv");
    csv << "filename,category,png_size,jxl_size,original_size,png_cr,jxl_cr,png_time_ms,png_dec_time,jxl_time_ms,djxl_time_ms\n";

    std::vector<fs::directory_entry> png_files;
    for (const auto& entry : fs::recursive_directory_iterator(folder_path)) {
        if (entry.is_regular_file() && entry.path().extension() == ".png") {
            png_files.push_back(entry);
        }
    }

    size_t total_files = png_files.size();
    size_t current = 0;

    for (const auto& entry : png_files) {
        current++;
        float progress = static_cast<float>(current) / total_files;
        int barWidth = 40;

        std::cout << "\r[";
        int pos = barWidth * progress;
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << int(progress * 100.0) << "% " << std::flush;

        std::string input_path = entry.path().string();
        std::string filename = entry.path().filename().string(); // includes extension
        filename.erase(std::remove(filename.begin(), filename.end(), ','), filename.end());
        std::string category = entry.path().parent_path().filename().string();

        cv::Mat img_raw = cv::imread(input_path, cv::IMREAD_UNCHANGED);
        if (img_raw.empty()) {
            std::cerr << "\nFailed to load image: " << input_path << "\n";
            continue;
        }

        cv::Mat img;
        if (img_raw.channels() == 4) {
            cv::cvtColor(img_raw, img, cv::COLOR_BGRA2BGR);  // Drop alpha
        } else if (img_raw.channels() == 1) {
            cv::cvtColor(img_raw, img, cv::COLOR_GRAY2BGR);  // Expand to 3 channels
        } else {
            img = img_raw;  // Already 3 channels
        }

        if (img.type() != CV_8UC3) {
            std::cerr << "\nImage is not 8-bit 3-channel RGB: " << input_path << "\n";
            continue;
        }

        if (img.empty()) {
            std::cerr << "\nFailed to load image: " << input_path << "\n";
            continue;
        }

        size_t original_size = img.total() * img.elemSize();

        std::string temp_png = "temp_" + filename;
        auto png_start = std::chrono::high_resolution_clock::now();
        std::vector<uchar> png_buffer;
        std::vector<int> png_params = {cv::IMWRITE_PNG_COMPRESSION, 9};
        bool png_success = cv::imencode(".png", img, png_buffer, png_params);

        if (!png_success) {
            std::cerr << "Failed to encode PNG for " << filename << "\n";
            continue;
        }

        std::ofstream out(temp_png, std::ios::binary);
        out.write(reinterpret_cast<const char*>(png_buffer.data()), png_buffer.size());
        out.close();
        auto png_end = std::chrono::high_resolution_clock::now();

        if (!fs::exists(temp_png)) {
            std::cerr << "\nFailed to write PNG for " << filename << "\n";
            continue;
        }

        size_t png_size = fs::file_size(temp_png);
        auto png_duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(png_end - png_start).count();
        double png_cr = static_cast<double>(original_size) / png_size;

        std::string temp_jxl = "temp_" + filename + ".jxl";
        auto jxl_start = std::chrono::high_resolution_clock::now();
        std::string cmd = "cjxl --distance=0 \"" + temp_png + "\" \"" + temp_jxl + "\" > /dev/null 2>&1";
        bool jxl_success = run_command(cmd);
        auto jxl_end = std::chrono::high_resolution_clock::now();

        if (!jxl_success || !fs::exists(temp_jxl)) {
            std::cerr << "\nJXL compression failed for " << filename << "\n";
            fs::remove(temp_png);
            continue;
        }

        size_t jxl_size = fs::file_size(temp_jxl);
        auto jxl_duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(jxl_end - jxl_start).count();
        double jxl_cr = static_cast<double>(original_size) / jxl_size;

        // Decode back
        std::string temp_dec_png = "dec_" + filename;
        auto djxl_start = std::chrono::high_resolution_clock::now();
        std::string djxl_cmd = "djxl \"" + temp_jxl + "\" \"" + temp_dec_png + "\" > /dev/null 2>&1";
        bool djxl_success = run_command(djxl_cmd);
        auto djxl_end = std::chrono::high_resolution_clock::now();
        auto djxl_duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(djxl_end - djxl_start).count();

        // Measure PNG decompression time
        auto png_dec_start = std::chrono::high_resolution_clock::now();
        cv::Mat decompressed_png = cv::imread(temp_png, cv::IMREAD_UNCHANGED);
        auto png_dec_end = std::chrono::high_resolution_clock::now();
        auto png_dec_duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(png_dec_end - png_dec_start).count();


        fs::remove(temp_png);
        fs::remove(temp_jxl);
        fs::remove(temp_dec_png);

        csv << filename << "," << category << "," << png_size << "," << jxl_size << "," << original_size << ","
            << std::fixed << std::setprecision(4) << png_cr << "," << jxl_cr << ","
            << png_duration_ms << "," << png_dec_duration_ms << "," << jxl_duration_ms << "," << djxl_duration_ms << "\n";
    }

    std::cout << "\nDone.\n";
    csv.close();
    return 0;
}

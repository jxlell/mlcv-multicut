#include <opencv2/opencv.hpp>
#include <iostream>
#include <map>
#include "quantization.h"

using namespace cv;
using namespace std;



Quantizer::Quantizer(const std::string& imagePath) {
    // Read the image
    try{
        image = cv::imread(imagePath);

    if (image.empty()) {
        std::cerr << "Error: Could not read the image." << std::endl;
    }
    }
    catch (const cv::Exception& e) {
        std::cerr << "OpenCV Exception: " << e.what() << std::endl;
        // Additional information, if needed
        std::cerr << "Function: " << e.func << ", File: " << e.file << ", Line: " << e.line << std::endl;
    } 
    
    
}

std::map<cv::Vec<unsigned char, 3>, cv::Vec<unsigned char, 3>, VecComparator> Quantizer::performQuantization(){


    // Reshape the image to a 2D matrix of pixels (rows x cols) and 3 color channels
    Mat reshapedImage = image.reshape(1, image.rows * image.cols);

    // Convert the image to a matrix of floats
    reshapedImage.convertTo(reshapedImage, CV_32F);

    // Specify the number of clusters (k)
    int k = 8;

    // Apply k-means clustering
    TermCriteria criteria(TermCriteria::EPS + TermCriteria::COUNT, 100, 0.2);
    Mat labels, centers;
    kmeans(reshapedImage, k, labels, criteria, 1, KMEANS_RANDOM_CENTERS, centers);

    // Convert the centers to 8-bit RGB values
    centers.convertTo(centers, CV_8U);

    // Map each pixel to its cluster center (quantization)
    std::cout << image.size().height << std::endl;
    Mat quantizedImage(image.size(), image.type());
    quantizedImage = Mat(image.size(), image.type());

    // Create a map with a custom comparator
    map<Vec<unsigned char, 3>, Vec<unsigned char, 3>, VecComparator> colorMapping;

    for (int i = 0; i < reshapedImage.rows; ++i) {
        int row = i / image.cols;
        int col = i % image.cols;

        // Get the original color
        Vec<unsigned char, 3> originalColor = image.at<Vec3b>(row, col);

        // Get the quantized color from the cluster center
        Vec<unsigned char, 3> quantizedColor = centers.row(labels.at<int>(i));

        // Store the mapping in the map
        colorMapping[originalColor] = quantizedColor;

        // Assign the quantized color to the corresponding pixel in the quantized image
        quantizedImage.at<Vec3b>(row, col) = quantizedColor;

        
    }


    // Print the color mapping
    /*
    for (const auto& entry : colorMapping) {
        cout << "Original Color: " << entry.first << " -> Quantized Color: " << entry.second << endl;
    }
    */

   
    imshow("Original Image", image);
    imshow("Quantized Image", quantizedImage);
    waitKey(0);

    return colorMapping;

}

void Quantizer::displayResults(){
    // Display the original and quantized images
    imshow("Original Image", image);
    imshow("Quantized Image", quantizedImage);

    // Print the color mapping
    /*
    for (const auto& entry : colorMapping) {
        cout << "Original Color: " << entry.first << " -> Quantized Color: " << entry.second << endl;
    }
    */
    

    // Wait for a key event to close the windows
    waitKey(0);
}

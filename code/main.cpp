#include "quantization.h"
//#include "multicut.h"
#include "Graph.h"

int main() {
    // Specify the path to your image
    std::string imagePath = "/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/Bilder/stolpen/_GH50868.jpg";

    // Create a Quantizer instance (image is read and processed in the constructor)
    Quantizer quantizer(imagePath);

    // Perform quantization
    std::map<cv::Vec<unsigned char, 3>, cv::Vec<unsigned char, 3>, VecComparator> mapping = quantizer.performQuantization();

    // Display results
    //quantizer.displayResults();

    // Print the color mapping  
    int count  = 0;
    for (const auto& entry : mapping) {
        if(count % 100 == 0){
            std::cout << "Original Color: " << entry.first << " -> Quantized Color: " << entry.second << std::endl;
        }
        count++;
    }
    


    return 0;
}
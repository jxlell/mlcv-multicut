#ifndef IMAGE_H
#define IMAGE_H

#include <cstdint>

struct RGB {

   
    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;
    
};

bool compareRGB(const RGB& color1, const RGB& color2);

#endif // IMAGE_H
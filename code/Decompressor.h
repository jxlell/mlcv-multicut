#ifndef DECOMPRESSOR_H
#define DECOMPRESSOR_H

#include <iostream>
#include "DirectionPath.h"
#include "Image.h"


class Decompressor {
    public:
        Decompressor(std::vector<RGB> regionColors, PathInfoVector paths, int edgeBitsSize, int rows, int cols);
        void reconstructImage();
        void reconstruct_edgeBits_iterative(int startEdge, Direction currentDir, std::vector<bool>& directionVector, std::vector<bool>& reconstructedEdgeBits);
    private:
        std::string imagePath;
        std::vector<int> neighborsOffsets;
        std::vector<RGB> regionColors;
        PathInfoVector paths;
        std::vector<bool> reconstructed_edgeBits;
        int edgeBitsSize;
        int rows;
        int cols;
};

#endif // DECOMPRESSOR_H
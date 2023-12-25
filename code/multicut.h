#ifndef MULTICUT_H
#define MULTICUT_H

#include <vector>
#include <bitset>

struct RGB {
    std::bitset<8> red;
    std::bitset<8> green;
    std::bitset<8> blue;
};

class MulticutGraph {
public:
    MulticutGraph(int vertices);
    void setEdgeBit(int v, int w, bool value);
    bool getEdgeBit(int v, int w) const;
    void setRGB(int v, const RGB& color);
    RGB getRGB(int v) const;

private:
    std::vector<std::bitset<1>> edgeBits;
    std::vector<RGB> vertexColors;
};

#endif // MULTICUT_H

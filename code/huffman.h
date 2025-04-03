#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <string>

using namespace std;

// Node structure for Huffman Tree
struct HuffmanNode {
    int data;
    int freq;
    HuffmanNode *left, *right;

    HuffmanNode(int data, int freq) : data(data), freq(freq), left(nullptr), right(nullptr) {}
};

// Comparator for priority queue
struct Compare {
    bool operator()(HuffmanNode* left, HuffmanNode* right) {
        return left->freq > right->freq;
    }
};

// Node structure for Huffman Tree
struct RGBHuffmanNode {
    uint8_t data;
    int freq;
    RGBHuffmanNode *left, *right;

    RGBHuffmanNode(uint8_t data, int freq) : data(data), freq(freq), left(nullptr), right(nullptr) {}
};

// Comparator for priority queue
struct RGBCompare {
    bool operator()(RGBHuffmanNode* left, RGBHuffmanNode* right) {
        return left->freq > right->freq;
    }
};

void generateCode(HuffmanNode* root, string code, map<int, string>& huffmanCodes);
std::tuple<map<int, string>, HuffmanNode*> buildCodes(map<int,int> frequencyMap);
void deleteHuffmanTree(HuffmanNode* root);
std::tuple<string, std::vector<int>> decodeHuffman(HuffmanNode* root, const string& encodedStr);
bool areHuffmanTreesEqual(HuffmanNode* root1, HuffmanNode* root2);

void generateRGBCode(RGBHuffmanNode* root, std::string code, std::map<uint8_t, std::string>& huffmanCodes);
std::tuple<std::map<uint8_t, std::string>, RGBHuffmanNode*> buildRGBCodes(std::map<uint8_t, int> frequencyMap);
std::tuple<std::string, std::vector<uint8_t>> decodeRGBHuffman(RGBHuffmanNode* root, const std::string& encodedStr);
bool areRGBHuffmanTreesEqual(RGBHuffmanNode* root1, RGBHuffmanNode* root2);
void deleteRGBHuffmanTree(RGBHuffmanNode* root);


#endif // HUFFMAN_H
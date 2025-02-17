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

void generateCode(HuffmanNode* root, string code, map<int, string>& huffmanCodes);
map<int, string> buildCodes(map<int,int> frequencyMap);



#endif // HUFFMAN_H
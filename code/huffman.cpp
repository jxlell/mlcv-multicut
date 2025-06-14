
#include <iostream>
#include <map>
#include "huffman.h"

using namespace std;

// adapted from Ayush Agrawal, https://github.com/mr-ayush-agrawal/Hacktoberfest/blob/9e5d9c36121664d988d2072cd8eafc476c008c9e/Most_Useful_Algorithms/HuffmanCoding.cpp
// accessed 20-02-2025


void generateCode(HuffmanNode* root, string code, map<int, string>& huffmanCodes){
    if(!root){
        return;
    }
    if(!root->left && !root->right){
        huffmanCodes[root->data] = code;
    }

    generateCode(root->left, code + "0", huffmanCodes);
    generateCode(root->right, code + "1", huffmanCodes);
}

std::tuple<map<int, string>, HuffmanNode*> buildCodes(map<int,int> frequencyMap){
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, Compare> pq;

    if (frequencyMap.empty()) {
        std::cout << "Frequency map is empty, skipping Huffman code generation." << std::endl;
        // Return an empty Huffman tree in case of empty frequency map
        return {std::map<int, string>(), nullptr};
    }

    for(auto& pair : frequencyMap){
        pq.push(new HuffmanNode(pair.first, pair.second));
    }

    if(frequencyMap.size() == 0){
        std::cout << "Frequency map is empty" << std::endl;
    }

    while(pq.size() > 1){
        HuffmanNode* left = pq.top();
        pq.pop();
        HuffmanNode* right = pq.top();
        pq.pop();

        HuffmanNode* newNode = new HuffmanNode(-1, left->freq + right->freq);
        newNode->left = left;
        newNode->right = right;

        pq.push(newNode);

    }

    map<int,string> huffmanCodes;
    generateCode(pq.top(), "", huffmanCodes);
    return {huffmanCodes, pq.top()};

}

void deleteHuffmanTree(HuffmanNode* root) {
    if (!root) return;
    deleteHuffmanTree(root->left);
    deleteHuffmanTree(root->right);
    delete root;
}

std::tuple<string, std::vector<int>> decodeHuffman(HuffmanNode* root, const string& encodedStr) {
    string decodedStr = "";
    HuffmanNode* currentNode = root;
    std::vector<int> lengths;

    for (char bit : encodedStr) {
        if (bit == '0') {
            if(currentNode->left){
                currentNode = currentNode->left;
            }
        } else {
            if(currentNode->right){
                currentNode = currentNode->right;
            }
        }

        // leaf node reached
        // add to length vector
        if (!currentNode->left && !currentNode->right) {
            decodedStr += char(currentNode->data);
            lengths.push_back(currentNode->data);
            currentNode = root;
        }
    }

    return {decodedStr, lengths};
}

bool areHuffmanTreesEqual(HuffmanNode* root1, HuffmanNode* root2) {
    // Both nodes are null, so they are equal
    if (!root1 && !root2) return true;

    // If one is null and the other is not, they are not equal
    if (!root1 || !root2) return false;

    // Check if the current nodes have the same data and frequency
    if (root1->data != root2->data || root1->freq != root2->freq){
        std::cout << "data1 " << root1->data << std::endl;
        std::cout << "freq1 " << root1->freq << std::endl;
        std::cout << "data2 " << root2->data << std::endl;
        std::cout << "freq2 " << root2->freq << std::endl;
        return false;
    } 

    // Recursively check left and right subtrees
    return areHuffmanTreesEqual(root1->left, root2->left) &&
           areHuffmanTreesEqual(root1->right, root2->right);
}


void generateRGBCode(RGBHuffmanNode* root, std::string code, std::map<uint8_t, std::string>& huffmanCodes) {
    if (!root) {
        return;
    }
    if (!root->left && !root->right) {
        huffmanCodes[root->data] = code;
    }

    generateRGBCode(root->left, code + "0", huffmanCodes);
    generateRGBCode(root->right, code + "1", huffmanCodes);
}

// Build Huffman Tree and generate codes for RGB values
std::tuple<std::map<uint8_t, std::string>, RGBHuffmanNode*> buildRGBCodes(std::map<uint8_t, int> frequencyMap) {
    std::priority_queue<RGBHuffmanNode*, std::vector<RGBHuffmanNode*>, RGBCompare> pq;

    if (frequencyMap.empty()) {
        std::cout << "RGB Frequency map is empty, skipping Huffman code generation." << std::endl;
        return {std::map<uint8_t, std::string>(), nullptr};
    }

    for (auto& pair : frequencyMap) {
        pq.push(new RGBHuffmanNode(pair.first, pair.second));
    }

     // Special case: only one unique value
     if (pq.size() == 1) {
        RGBHuffmanNode* soleNode = pq.top();
        std::map<uint8_t, std::string> huffmanCodes;
        huffmanCodes[soleNode->data] = "0";  // Assign a dummy code
        return {huffmanCodes, soleNode};
    }

    while (pq.size() > 1) {
        RGBHuffmanNode* left = pq.top();
        pq.pop();
        RGBHuffmanNode* right = pq.top();
        pq.pop();

        RGBHuffmanNode* newNode = new RGBHuffmanNode(0, left->freq + right->freq); // Using '0' as dummy data for internal nodes
        newNode->left = left;
        newNode->right = right;

        pq.push(newNode);
    }

    std::map<uint8_t, std::string> huffmanCodes;
    generateRGBCode(pq.top(), "", huffmanCodes);
    return {huffmanCodes, pq.top()};
}

// Decode a Huffman-encoded string for RGB values
std::tuple<std::string, std::vector<uint8_t>> decodeRGBHuffman(RGBHuffmanNode* root, const std::string& encodedStr) {
    std::string decodedStr = "";
    RGBHuffmanNode* currentNode = root;
    std::vector<uint8_t> values;

    for (char bit : encodedStr) {
        currentNode = (bit == '0') ? currentNode->left : currentNode->right;

        if (!currentNode->left && !currentNode->right) { // Leaf node reached
            decodedStr += char(currentNode->data);
            values.push_back(currentNode->data);
            currentNode = root;
        }
    }

    return {decodedStr, values};
}

// Compare two RGB Huffman trees
bool areRGBHuffmanTreesEqual(RGBHuffmanNode* root1, RGBHuffmanNode* root2) {
    if (!root1 && !root2) return true;
    if (!root1 || !root2) return false;

    if (root1->data != root2->data || root1->freq != root2->freq) return false;

    return areRGBHuffmanTreesEqual(root1->left, root2->left) &&
           areRGBHuffmanTreesEqual(root1->right, root2->right);
}

// Delete RGB Huffman Tree
void deleteRGBHuffmanTree(RGBHuffmanNode* root) {
    if (!root) return;
    deleteRGBHuffmanTree(root->left);
    deleteRGBHuffmanTree(root->right);
    delete root;
}



// int main(){
//     map<int,int> frequencyMap = {
//         {1, 5},
//         {2, 10},
//         {3, 15},
//         {4, 25},
//         {5, 45}
//     };

//     map<int,string> huffmanCodes;
//     HuffmanNode* root;
//     std::tie(huffmanCodes, root) = buildCodes(frequencyMap);

//     cout << "Huffman Codes are: " << endl;
//     for(auto& pair : huffmanCodes){
//         cout << pair.first << " : " << pair.second << endl;
//     }

//     return 0;
// }
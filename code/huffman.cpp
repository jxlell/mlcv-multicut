
#include <iostream>
#include <map>

using namespace std;

struct HuffmanNode{
    int data;
    int freq;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(int data, int freq) : data(data), freq(freq), left(nullptr), right(nullptr) {}
};

struct Compare{
    bool operator()(HuffmanNode* l, HuffmanNode* r){
        return l->freq > r->freq;
    }
};

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

    for(auto& pair : frequencyMap){
        pq.push(new HuffmanNode(pair.first, pair.second));
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
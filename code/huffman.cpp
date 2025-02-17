
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

map<int, string> buildCodes(map<int,int> frequencyMap){
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
    return huffmanCodes;

}

// int main(){
//     map<int,int> frequencyMap = {
//         {1, 5},
//         {2, 10},
//         {3, 15},
//         {4, 25},
//         {5, 45}
//     };

//     map<int,string> huffmanCodes = buildCodes(frequencyMap);

//     cout << "Huffman Codes are: " << endl;
//     for(auto& pair : huffmanCodes){
//         cout << pair.first << " : " << pair.second << endl;
//     }

//     return 0;
// }
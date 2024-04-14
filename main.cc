#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <queue>

//Huffman tree node
struct TreeNode {
    char data;
    unsigned int frequency;

    TreeNode *left;
    TreeNode *right;
};

struct compare {
    bool operator()(TreeNode* a, TreeNode* b) {
        return a->frequency > b->frequency;
    }
};

unsigned sizeOfTree = 0;

TreeNode* newNode(char data, unsigned int frequency, TreeNode* left, TreeNode* right) {
    TreeNode* node = new TreeNode();
    node->data = data;
    node->frequency = frequency;
    node->left = left;
    node->right = right;

    return node;
}

void encode(TreeNode* node, std::string code, std::unordered_map<char, std::string> &huffmanCodes) {
    if(node == nullptr)
        return;

    if(node->left == nullptr && node->right == nullptr) {
        huffmanCodes[node->data] = code;
        sizeOfTree += sizeof(node) * 8;
        return;
    }

    encode(node->left, code + '0', huffmanCodes);
    encode(node->right, code + '1', huffmanCodes);
}

void decode(TreeNode* node, int &index, std::string text) {
    if(node == nullptr)
        return;

    if(node->left == nullptr && node->right == nullptr) {
        std::cout << node->data;
        return;
    }

    index++;

    if(text[index] == '0')
        decode(node->left, index, text);
    else
        decode(node->right, index, text);
}

void freeMemory(TreeNode* root) {
    if(root == nullptr) 
        return;

    if(root->left == nullptr && root->right == nullptr) {
        free(root);
        return;
    }

    freeMemory(root->left);
    freeMemory(root->right);
    
}

int main(int argc, char** argv) {
    if(argc != 2) {
        std::cout << "Usage: ./compression filename.txt\n"; 
        exit(1);
    }

    unsigned int fileSize = 0;
    char character;
    std::ifstream file(argv[1]);
    std::unordered_map<char, int> frequencies;

    while(file.get(character)) {
        frequencies[character]++;
        fileSize += 8;
    }
    file.clear();
    file.seekg(0);

    std::priority_queue<TreeNode*, std::vector<TreeNode*>, compare> min_heap;

    for(auto pair : frequencies) {
        min_heap.push(newNode(pair.first, pair.second, nullptr, nullptr));
    }

    while(min_heap.size() != 1) {
        TreeNode* left = min_heap.top();
        min_heap.pop();

        TreeNode* right = min_heap.top();
        min_heap.pop();

        TreeNode* sumNode = newNode('\0', left->frequency+right->frequency, left, right);
        min_heap.push(sumNode); 
    }

    TreeNode* root = min_heap.top();

    std::unordered_map<char, std::string> huffmanCodes;
    encode(root, "", huffmanCodes);

    for(auto pair : huffmanCodes) {
        if(int(pair.first) == 10) 
            std::cout << "\\n" << ": " << pair.second << '\n';
        else
            std::cout << pair.first << ": " << pair.second << '\n';
    }

    std::string encoded = "";

    while(file.get(character)) {
        encoded += huffmanCodes[character];
    } 

    int index = -1;
    while(index < (int) encoded.size() - 2) {
        decode(root, index, encoded);
    }

    unsigned compressedFileSize = encoded.size() + (sizeof(encoded) + sizeof(huffmanCodes)*8) + sizeOfTree;

    std::cout << "\n\nOriginal file size: " << fileSize << '\n';
    std::cout << "Compressed file size: " << compressedFileSize << '\n';
    std::cout << ((float) compressedFileSize / (float) fileSize) * 100 << "%\n";

    file.close();
    freeMemory(root);

    return 0;
}
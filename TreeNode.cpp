#include "TreeNode.h"
TreeNode::TreeNode(void){}

TreeNode::~TreeNode(void){}
void TreeNode::init(const int* board, int d, unsigned long long key){
    memcpy(Board, board, sizeof(int)*32);
    depth = 0;
    hashKey = key;
}


void TreeNode::copyRemain(std::vector<int> remain[14]){
    for(int i=0;i<14;i++){
        for(int j=0;j<remain[i].size();j++){
            remainChess[i].push_back(remain[i][j]);
        }
    }
}
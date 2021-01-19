#include "TreeNode.h"
TreeNode::TreeNode(void){}

TreeNode::~TreeNode(void){}
void TreeNode::init(const int* board, int d, unsigned long long key){
    memcpy(Board, board, sizeof(int)*32);
    depth = 0;
    hashKey = key;
}



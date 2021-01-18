#ifndef TREENODE_INCLUDED
#define TREENODE_INCLUDED 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <algorithm>
#include <unordered_set>
#include <vector>

class TreeNode{
public:
    TreeNode(void);
    ~TreeNode(void);
    void init(const int* board, int d, unsigned long long key);
    unsigned long long hashKey;    
    int Board[32];
    std::vector<int> remainChess[14];
    int move;
    int depth;
    double bestValue;
    int bestMove;


};

#endif
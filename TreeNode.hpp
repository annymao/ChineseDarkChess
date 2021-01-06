#ifndef TREENODE_INCLUDED
#define TREENODE_INCLUDED 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <algorithm>
#include <unordered_set>
#include "ZobristHashTable.hpp"

class TreeNode{
public:
    TreeNode(void);
    ~TreeNode(void);
    void init(int* board, int* move, int red_chess_num, int black_chess_num, int* cover_chess, int color, int depth, int remain_depth,double alpha, double beta,bool silence);
    unsigned long long hasKey;    
    int Board[32];
    int move;
    int red_chess_num;
    int black_chess_num;
    int cover_chess[14];
    int color;
    int depth;
    int remain_depth;
    double aplha;
    double beta;
    bool silence;

};

#endif
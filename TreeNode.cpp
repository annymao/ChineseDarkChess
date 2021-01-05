#include "TreeNode.hpp"

int init(int* board, int* m, int red_num, int black_num, int* cover, int c, int d, int r_depth,double a, double b,bool s){
    memcpy(Board, board, sizeof(int)*32);
    move = *m;
    red_chess_num = red_num;
    black_chess_num = black_num;
	memcpy(cover_chess, cover, sizeof(int)*14);
    color = c;
    depth = d;
    remain_depth = r;
    alpha = a;
    beta = b;
    silence = s;
    //TODO:Hash Key
}
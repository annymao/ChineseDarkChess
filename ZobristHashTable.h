#ifndef ZOBRISTHASHTABLE_INCLUDED
#define ZOBRISTHASHTABLE_INCLUDED 
#include <random> 
#include <limits.h>

#define MAX_NODE_NUM 4096 //2^12


static std::random_device rd;    
static std::mt19937_64 random_generator(rd());
static std::uniform_int_distribution<unsigned long long> num(0, ULLONG_MAX);

extern unsigned long long randomColorValue[2];
extern unsigned long long randomMoveValue[15][32];

void InitRandomValue();

unsigned long long InitHashKey(int* board,int color);

unsigned long long UpdateHashForEat(unsigned long long oldValue,  int chessFrom, char chessFromPos, int chessEaten, int chessEatenPos);

unsigned long long UpdateHashForMove(unsigned long long oldValue, int chessFrom, int chessFromPos, int chessToPos);

unsigned long long UpdateHashForFlip(unsigned long long oldValue, int chessFlip, int chessPos);
bool compareBoard(int* board1, int* board2);


#endif

#include <random> 
#include <limits.h>
#include "ZobristHashTable.h"

unsigned long long randomColorValue[2];
unsigned long long randomMoveValue[15][32];


#define COVER 14
void InitRandomValue(){
    for(int i = 0 ; i < 15 ; ++i){
        for(int j = 0 ; j < 32 ; ++j){
            randomMoveValue[i][j] = num(random_generator);
        }
    }
    randomColorValue[0] = num(random_generator);
    randomColorValue[1] = num(random_generator);

}

unsigned long long InitHashKey(int* board,int color){
    unsigned long long value = 1234;
    //color
    value ^= randomColorValue[0];
    
    //board
    for(int i = 0 ; i < 32 ; ++i){
             value ^= randomMoveValue[COVER][i];
        
    }
    return value;
}

unsigned long long UpdateHashForEat(unsigned long long oldValue,  int chessFrom, char chessFromPos, int chessEaten, int chessEatenPos){
    return oldValue^randomColorValue[0]^randomColorValue[1]
            ^randomMoveValue[chessFrom][chessFromPos]
            ^randomMoveValue[chessFrom][chessEatenPos]
            ^randomMoveValue[chessEaten][chessEatenPos];
}

unsigned long long UpdateHashForMove(unsigned long long oldValue, int chessFrom, int chessFromPos, int chessToPos){
    return oldValue^randomColorValue[0]^randomColorValue[1]
        ^randomMoveValue[chessFrom][chessFromPos]
        ^randomMoveValue[chessFrom][chessToPos];
} 

unsigned long long UpdateHashForFlip(unsigned long long oldValue, int chessFlip, int chessPos){
    return oldValue^randomColorValue[0]^randomColorValue[1]
        ^randomMoveValue[COVER][chessPos]
        ^randomMoveValue[chessFlip][chessPos];
}

bool compareBoard(int* board1, int* board2){
    for(int i=0;i<32;i++){
        if(board1[i]!=board2[i])return false;
    }
    return true;
}
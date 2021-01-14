
#include <random> 
#include <limits.h>
#include "ZobristHashTable.h"

unsigned long long randomColorValue[2];
unsigned long long randomMoveValue[16][32];

void InitRandomValue(){
    for(int i = 0 ; i < 16 ; ++i){
        for(int j = 0 ; j < 32 ; ++j){
            randomMoveValue[i][j] = num(random_generator);
        }
    }
    randomColorValue[0] = num(random_generator);
    randomColorValue[1] = num(random_generator);

}

unsigned long long InitHashKey(int* board,int color){
    unsigned long long value = 0;
    //color
    //value ^= randomColorValue[int(color)];
    
    //board
    for(int i = 0 ; i < 32 ; ++i){
        if(board[i] != -1 && board[i] != -2){
            value ^= randomMoveValue[board[i]][i];
        }
        else if(board[i] == -1){
             value ^= randomMoveValue[15][i];
        }
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
        ^randomMoveValue[15][chessFromPos]
        ^randomMoveValue[chessFrom][chessToPos];
} 

unsigned long long UpdateHashForFlip(unsigned long long oldValue, int chessFlip, int chessPos){
    return oldValue^randomColorValue[0]^randomColorValue[1]
        ^randomMoveValue[15][chessPos]
        ^randomMoveValue[chessFlip][chessPos];
}


#ifndef MYAI_INCLUDED
#define MYAI_INCLUDED 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unordered_map>
#include "TreeNode.h"
#include "ZobristHashTable.h"

#define RED 0
#define BLACK 1
#define CHESS_COVER -1
#define CHESS_EMPTY -2
#define COMMAND_NUM 18
#define TIME_LIMIT 5
extern TreeNode* allHashNode[MAX_NODE_NUM];
class MyAI  
{
	const char* commands_name[COMMAND_NUM] = {
		"protocol_version",
		"name",
		"version",
		"known_command",
		"list_commands",
		"quit",
		"boardsize",
		"reset_board",
		"num_repetition",
		"num_moves_to_draw",
		"move",
		"flip",
		"genmove",
		"game_over",
		"ready",
		"time_settings",
		"time_left",
  	"showboard"
	};
public:
	MyAI(void);
	~MyAI(void);

	// commands
	bool protocol_version(const char* data[], char* response);// 0
	bool name(const char* data[], char* response);// 1
	bool version(const char* data[], char* response);// 2
	bool known_command(const char* data[], char* response);// 3
	bool list_commands(const char* data[], char* response);// 4
	bool quit(const char* data[], char* response);// 5
	bool boardsize(const char* data[], char* response);// 6
	bool reset_board(const char* data[], char* response);// 7
	bool num_repetition(const char* data[], char* response);// 8
	bool num_moves_to_draw(const char* data[], char* response);// 9
	bool move(const char* data[], char* response);// 10
	bool flip(const char* data[], char* response);// 11
	bool genmove(const char* data[], char* response);// 12
	bool game_over(const char* data[], char* response);// 13
	bool ready(const char* data[], char* response);// 14
	bool time_settings(const char* data[], char* response);// 15
	bool time_left(const char* data[], char* response);// 16
	bool showboard(const char* data[], char* response);// 17
	void generateInitHash();

private:
	int Color;
	int Red_Time, Black_Time;
	int Board[32];
	int CoverChess[14];
	int Red_Chess_Num, Black_Chess_Num;
	int node;
	unsigned long long hashKey;
	//TODO: HashMap
	//std::unordered_map<>
	// Utils
	int GetFin(char c);
	int ConvertChessNo(int input);

	// Board
	void initBoardState();
	void generateMove(char move[6]);
	void MakeMove(TreeNode* CDCNode, int* red_chess_num, int* black_chess_num, int* cover_chess, const int move, const int chess);
	void MakeMove(int* board, int* red_chess_num, int* black_chess_num, int* cover_chess, const int move, const int chess);
	void MakeMove(int* board, int* red_chess_num, int* black_chess_num, int* cover_chess, const char move[6]);
	bool Referee(const int* board, const int Startoint, const int EndPoint, const int color);
	int RefereeEat(const int* board, const int Startoint, const int EndPoint, const int color);
	int Expand(const int* board, int red_chess_num, int black_chess_num,const int* cover_chess, const int color,int *ResultEat, int* ResultMove,int* saveEat,int &saveEatCount);
	int ExpandEat(const int* board, const int color, int *Result);
	int ExpandFlip(const int* board, const int color,std::unordered_set<int> &Result);
	double myEvaluate(const int* board);
	double Nega_Scout(const TreeNode* board, int* move, const int red_chess_num, const int black_chess_num, const int* cover_chess, const int color, const int depth, const int remain_depth,double alpha,double beta,bool silence,double exchangeCount,struct timespec start,int &isSaveEat);
	double F_3(const int* board, double alpha, double beta,const int red_chess_num, const int black_chess_num,const int* cover_chess, const int color,int remain_depth,struct timespec start);
	double G_3(const int* board, double alpha, double beta,const int red_chess_num, const int black_chess_num,const int* cover_chess, const int color,int remain_depth,struct timespec start);
	double Star0_F_3(const int* board, double alpha, double beta,const int red_chess_num, const int black_chess_num, const int* cover_chess, int* Chess, int current_move,int remain_count, int remain_total,const int color,int remain_depth,struct timespec start);


	// Display
	void Pirnf_Chess(int chess_no,char *Result);
	void Pirnf_Chessboard();
	
};

#endif


#include "float.h"
#include "MyAI.h"

#define DEPTH_LIMIT 3

MyAI::MyAI(void){}

MyAI::~MyAI(void){}

bool MyAI::protocol_version(const char* data[], char* response){
	strcpy(response, "1.0.0");
  return 0;
}

bool MyAI::name(const char* data[], char* response){
	strcpy(response, "MyAI");
	return 0;
}

bool MyAI::version(const char* data[], char* response){
	strcpy(response, "1.0.0");
	return 0;
}

bool MyAI::known_command(const char* data[], char* response){
  for(int i = 0; i < COMMAND_NUM; i++){
		if(!strcmp(data[0], commands_name[i])){
			strcpy(response, "true");
			return 0;
		}
	}
	strcpy(response, "false");
	return 0;
}

bool MyAI::list_commands(const char* data[], char* response){
  for(int i = 0; i < COMMAND_NUM; i++){
		strcat(response, commands_name[i]);
		if(i < COMMAND_NUM - 1){
			strcat(response, "\n");
		}
	}
	return 0;
}

bool MyAI::quit(const char* data[], char* response){
  fprintf(stderr, "Bye\n"); 
	return 0;
}

bool MyAI::boardsize(const char* data[], char* response){
  fprintf(stderr, "BoardSize: %s x %s\n", data[0], data[1]); 
	return 0;
}

bool MyAI::reset_board(const char* data[], char* response){
	this->Red_Time = -1; // unknown
	this->Black_Time = -1; // unknown
	this->initBoardState();
	return 0;
}

bool MyAI::num_repetition(const char* data[], char* response){
  return 0;
}

bool MyAI::num_moves_to_draw(const char* data[], char* response){
  return 0;
}

bool MyAI::move(const char* data[], char* response){
  char move[6];
	sprintf(move, "%s-%s", data[0], data[1]);
	this->MakeMove(this->Board, &this->Red_Chess_Num, &this->Black_Chess_Num, this->CoverChess, move);
	return 0;
}

bool MyAI::flip(const char* data[], char* response){
  char move[6];
	sprintf(move, "%s(%s)", data[0], data[1]);
	this->MakeMove(this->Board, &this->Red_Chess_Num, &this->Black_Chess_Num, this->CoverChess, move);
	return 0;
}

bool MyAI::genmove(const char* data[], char* response){
	// set color
	if(!strcmp(data[0], "red")){
		this->Color = RED;
	}else if(!strcmp(data[0], "black")){
		this->Color = BLACK;
	}else{
		this->Color = 2;
	}
	// genmove
  char move[6];
	this->generateMove(move);
	sprintf(response, "%c%c %c%c", move[0], move[1], move[3], move[4]);
	return 0;
}

bool MyAI::game_over(const char* data[], char* response){
  fprintf(stderr, "Game Result: %s\n", data[0]); 
	return 0;
}

bool MyAI::ready(const char* data[], char* response){
  return 0;
}

bool MyAI::time_settings(const char* data[], char* response){
  return 0;
}

bool MyAI::time_left(const char* data[], char* response){
  if(!strcmp(data[0], "red")){
		sscanf(data[1], "%d", &(this->Red_Time));
	}else{
		sscanf(data[1], "%d", &(this->Black_Time));
	}
	fprintf(stderr, "Time Left(%s): %s\n", data[0], data[1]); 
	return 0;
}

bool MyAI::showboard(const char* data[], char* response){
  Pirnf_Chessboard();
	return 0;
}


// *********************** AI FUNCTION *********************** //

int MyAI::GetFin(char c)
{
	static const char skind[]={'-','K','G','M','R','N','C','P','X','k','g','m','r','n','c','p'};
	for(int f=0;f<16;f++)if(c==skind[f])return f;
	return -1;
}

int MyAI::ConvertChessNo(int input)
{
	switch(input)
	{
	case 0:
		return CHESS_EMPTY;
		break;
	case 8:
		return CHESS_COVER;
		break;
	case 1:
		return 6;
		break;
	case 2:
		return 5;
		break;
	case 3:
		return 4;
		break;
	case 4:
		return 3;
		break;
	case 5:
		return 2;
		break;
	case 6:
		return 1;
		break;
	case 7:
		return 0;
		break;
	case 9:
		return 13;
		break;
	case 10:
		return 12;
		break;
	case 11:
		return 11;
		break;
	case 12:
		return 10;
		break;
	case 13:
		return 9;
		break;
	case 14:
		return 8;
		break;
	case 15:
		return 7;
		break;
	}
	return -1;
}


void MyAI::initBoardState()
{	
	int iPieceCount[14] = {5, 2, 2, 2, 2, 2, 1, 5, 2, 2, 2, 2, 2, 1};
	memcpy(this->CoverChess,iPieceCount,sizeof(int)*14);
	Red_Chess_Num = 16; Black_Chess_Num = 16;

	//convert to my format
	int Index = 0;
	for(int i=0;i<8;i++)
	{
		for(int j=0;j<4;j++)
		{
			this->Board[Index] = CHESS_COVER;
			Index++;
		}
	}
	this->Pirnf_Chessboard();
}

void MyAI::generateMove(char move[6])
{
	/* generateMove Call by reference: change src,dst */
	this->node = 0;
	int startPoint = 0;
	int EndPoint = 0;

	int best_move = 0;
	double t = Nega_Scout(this->Board, &best_move, this->Red_Chess_Num, this->Black_Chess_Num, this->CoverChess, this->Color, 0, DEPTH_LIMIT,-DBL_MAX,DBL_MAX,true);
	
	startPoint = best_move/100;
	EndPoint   = best_move%100;
	sprintf(move, "%c%c-%c%c",'a'+(startPoint%4),'1'+(7-startPoint/4),'a'+(EndPoint%4),'1'+(7-EndPoint/4));

	char chess_Start[4]="";
	char chess_End[4]="";
	Pirnf_Chess(Board[startPoint],chess_Start);
	Pirnf_Chess(Board[EndPoint],chess_End);
	printf("My result: \n--------------------------\n");
	printf("Nega max: %lf (node: %d)\n", t, this->node);
	printf("(%d) -> (%d)\n",startPoint,EndPoint);
	printf("<%s> -> <%s>\n",chess_Start,chess_End);
	printf("move:%s\n",move);
	printf("--------------------------\n");
	this->Pirnf_Chessboard();
}

void MyAI::MakeMove(int* board, int* red_chess_num, int* black_chess_num, int* cover_chess, const int move, const int chess){
	int src = move/100, dst = move%100;
	if(src == dst){ 
		board[src] = chess;
		cover_chess[chess]--;
	}else { 
		if(board[dst] != CHESS_EMPTY){
			if(board[dst] / 7 == 0){
				(*red_chess_num)--;
			}else{
				(*black_chess_num)--;
			}
		}
		board[dst] = board[src];
		board[src] = CHESS_EMPTY;
	}
}

void MyAI::MakeMove(int* board, int* red_chess_num, int* black_chess_num, int* cover_chess, const char move[6])
{ 
	int src, dst;
	src = ('8'-move[1])*4+(move[0]-'a');
	if(move[2]=='('){ 
		printf("# call flip(): flip(%d,%d) = %d\n",src, src, GetFin(move[3])); 
		board[src] = ConvertChessNo(GetFin(move[3]));
		cover_chess[ConvertChessNo(GetFin(move[3]))]--;
		Pirnf_Chessboard();
	}else { 
		dst = ('8'-move[4])*4+(move[3]-'a');
		printf("# call move(): move : %d-%d \n",src,dst); 
		if(board[dst] != CHESS_EMPTY){
			if(board[dst] / 7 == 0){
				(*red_chess_num)--;
			}else{
				(*black_chess_num)--;
			}
		}
		board[dst] = board[src];
		board[src] = CHESS_EMPTY;
		Pirnf_Chessboard();
	}
	/* init time */
}

int MyAI::Expand(const int* board, const int color,int *Result)
{
	int ResultCount = 0;
	for(int i=0;i<32;i++)
	{
		if(board[i] >= 0 && board[i]/7 == color)
		{
			//Gun
			if(board[i] % 7 == 1)
			{
				int row = i/4;
				int col = i%4;
				for(int rowCount=row*4;rowCount<(row+1)*4;rowCount++)
				{
					if(Referee(board,i,rowCount,color))
					{
						Result[ResultCount] = i*100+rowCount;
						ResultCount++;
					}
				}
				for(int colCount=col; colCount<32;colCount += 4)
				{
				
					if(Referee(board,i,colCount,color))
					{
						Result[ResultCount] = i*100+colCount;
						ResultCount++;
					}
				}
			}
			else
			{
				int Move[4] = {i-4,i+1,i+4,i-1};
				for(int k=0; k<4;k++)
				{
					
					if(Move[k] >= 0 && Move[k] < 32 && Referee(board,i,Move[k],color))
					{
						Result[ResultCount] = i*100+Move[k];
						ResultCount++;
					}
				}
			}
		
		};
	}
	return ResultCount;
}


int MyAI::ExpandEat(const int* board, const int color,int *Result)
{
	int ResultCount = 0;
	for(int i=0;i<32;i++)
	{
		if(board[i] >= 0 && board[i]/7 == color)
		{
			//Gun
			if(board[i] % 7 == 1)
			{
				int row = i/4;
				int col = i%4;
				for(int rowCount=row*4;rowCount<(row+1)*4;rowCount++)
				{
					if(RefereeEat(board,i,rowCount,color))
					{
						Result[ResultCount] = i*100+rowCount;
						ResultCount++;
					}
				}
				for(int colCount=col; colCount<32;colCount += 4)
				{
				
					if(RefereeEat(board,i,colCount,color))
					{
						Result[ResultCount] = i*100+colCount;
						ResultCount++;
					}
				}
			}
			else
			{
				int Move[4] = {i-4,i+1,i+4,i-1};
				for(int k=0; k<4;k++)
				{
					
					if(Move[k] >= 0 && Move[k] < 32 && RefereeEat(board,i,Move[k],color))
					{
						Result[ResultCount] = i*100+Move[k];
						ResultCount++;
					}
				}
			}
		
		};
	}
	return ResultCount;
}
int MyAI::ExpandFlip(const int* board, const int color,std::unordered_set<int> &Result)
{
	int ResultCount = 0;
	for(int i=0;i<32;i++)
	{
		if(board[i] >= 0 && board[i]/7 != color)
		{
			//Gun
			if(board[i] % 7 == 1)
			{
				int row = i/4;
				int col = i%4;

				int Move[4] = {-4,-1,1,4};
				for(int k=0; k<4;k++)
				{
					int currentMove = i+Move[k];
					if(currentMove >= 0 && currentMove < 32 && board[currentMove]==CHESS_COVER)
					{
						bool canFlip = true;
						for(int h =0;h<4;h++){
							int tmp = currentMove + Move[h];
							if(tmp >= 0 && tmp < 32 && board[tmp]!=CHESS_COVER && board[tmp]%7>1 &&board[i]/7 != color ){
								canFlip = false;
								break;
							}
						}
						if(canFlip){
							Result.insert(currentMove*100+currentMove);
							ResultCount++;
						}
						
					}
				}
				
			}
			else
			{
				for(int k=1; k<4;k++)
				{
					//if(k==0) continue;
					int currentMove = i+k;
					int middleCount = 0;
					if(currentMove >= 0 && currentMove < 32 && currentMove/4 == i/4){//same row
						if(middleCount==1 && board[currentMove]==CHESS_COVER){
							bool canFlip = true;
							int Move[4] = {-4,-1,1,4};
							for(int h =0;h<4;h++){
								int tmp = currentMove + Move[h];
								if(tmp >= 0 && tmp < 32 && board[tmp]!=CHESS_COVER && board[tmp]%7>1){
									canFlip = false;
									break;
								}
							}
							if(canFlip){
								Result.insert(currentMove*100+currentMove);
								ResultCount++;
							}
							break;
						}
						else if(board[currentMove]!=CHESS_EMPTY){
							middleCount++;
							if(middleCount >1)break;
						}
					}
			
				}
				for(int k=-1; k>-4;k--)
				{
					//if(k==0) continue;
					int currentMove = i+k;
					int middleCount = 0;
					if(currentMove >= 0 && currentMove < 32 && currentMove/4 == i/4){//same row
						if(middleCount==1 && board[currentMove]==CHESS_COVER){
							bool canFlip = true;
							int Move[4] = {-4,-1,1,4};
							for(int h =0;h<4;h++){
								int tmp = currentMove + Move[h];
								if(tmp >= 0 && tmp < 32 && board[tmp]!=CHESS_COVER && board[tmp]%7>1){
									canFlip = false;
									break;
								}
							}
							if(canFlip){
								Result.insert(currentMove*100+currentMove);
								ResultCount++;
							}
							break;
						}
						else if(board[currentMove]!=CHESS_EMPTY){
							middleCount++;
							if(middleCount >1)break;
						}
					}
			
				}
				for(int k=4; k<32;k+=4)
				{
					//if(k==0) continue;
					int currentMove = i+k;
					int middleCount = 0;
					if(currentMove >= 0 && currentMove < 32){
						if(middleCount==1 && board[currentMove]==CHESS_COVER){
							bool canFlip = true;
							int Move[4] = {-4,-1,1,4};
							for(int h =0;h<4;h++){
								int tmp = currentMove + Move[h];
								if(tmp >= 0 && tmp < 32 && board[tmp]!=CHESS_COVER && board[tmp]%7>1){
									canFlip = false;
									break;
								}
							}
							if(canFlip){
								Result.insert(currentMove*100+currentMove);
								ResultCount++;
							}
							break;
						}
						else if(board[currentMove]!=CHESS_EMPTY){
							middleCount++;
							if(middleCount >1)break;
						}
					}
			
				}
				for(int k=-4; k>-32;k-=4)
				{
					//if(k==0) continue;
					int currentMove = i+k;
					int middleCount = 0;
					if(currentMove >= 0 && currentMove < 32){
						if(middleCount==1 && board[currentMove]==CHESS_COVER){
							bool canFlip = true;
							int Move[4] = {-4,-1,1,4};
							for(int h =0;h<4;h++){
								int tmp = currentMove + Move[h];
								if(tmp >= 0 && tmp < 32 && board[tmp]!=CHESS_COVER && board[tmp]%7>1){
									canFlip = false;
									break;
								}
							}
							if(canFlip){
								Result.insert(currentMove*100+currentMove);
								ResultCount++;
							}
							break;
						}
						else if(board[currentMove]!=CHESS_EMPTY){
							middleCount++;
							if(middleCount >1)break;
						}
					}
			
				}
			}
		
		}
	}
	return ResultCount;
}
bool MyAI::RefereeEat(const int* chess, const int from_location_no, const int to_location_no, const int UserId)
{
	int MessageNo = 0;
	bool IsCurrent = false;
	int from_chess_no = chess[from_location_no];
	int to_chess_no = chess[to_location_no];
	int from_row = from_location_no / 4;
	int to_row = to_location_no / 4;
	int from_col = from_location_no % 4;
	int to_col = to_location_no % 4;

	if(from_chess_no < 0 || ( to_chess_no < 0 && to_chess_no != CHESS_EMPTY) )
	{  
		MessageNo = 1;
		//strcat(Message,"**no chess can move**");
		//strcat(Message,"**can't move darkchess**");
		IsCurrent = false;
	}
	else if (from_chess_no >= 0 && from_chess_no / 7 != UserId)
	{
		MessageNo = 2;
		//strcat(Message,"**not my chess**");
		IsCurrent = false;
	}
	else if((from_chess_no / 7 == to_chess_no / 7) && to_chess_no >= 0)
	{
		MessageNo = 3;
		//strcat(Message,"**can't eat my self**");
		IsCurrent = false;
	}
	//check attack
	else if(to_chess_no == CHESS_EMPTY && abs(from_row-to_row) + abs(from_col-to_col)  == 1)//legal move
	{
		IsCurrent = false;
	}	
	else if(from_chess_no % 7 == 1 ) //judge gun
	{
		int row_gap = from_row-to_row;
		int col_gap = from_col-to_col;
		int between_Count = 0;
		//slant
		if(from_row-to_row == 0 || from_col-to_col == 0)
		{
			//row
			if(row_gap == 0) 
			{
				for(int i=1;i<abs(col_gap);i++)
				{
					int between_chess;
					if(col_gap>0)
						between_chess = chess[from_location_no-i] ;
					else
						between_chess = chess[from_location_no+i] ;
					if(between_chess != CHESS_EMPTY)
						between_Count++;
				}
			}
			//column
			else
			{
				for(int i=1;i<abs(row_gap);i++)
				{
					int between_chess;
					if(row_gap > 0)
						between_chess = chess[from_location_no-4*i] ;
					else
						between_chess = chess[from_location_no+4*i] ;
					if(between_chess != CHESS_EMPTY)
						between_Count++;
				}
				
			}
			
			if(between_Count != 1 )
			{
				MessageNo = 4;
				//strcat(Message,"**gun can't eat opp without between one piece**");
				IsCurrent = false;
			}
			else if(to_chess_no == CHESS_EMPTY)
			{
				MessageNo = 5;
				//strcat(Message,"**gun can't eat opp without between one piece**");
				IsCurrent = false;
			}
		}
		//slide
		else
		{
			MessageNo = 6;
			//strcat(Message,"**cant slide**");
			IsCurrent = false;
		}
	}
	else // non gun
	{
		//judge pawn or king

		//distance
		if( abs(from_row-to_row) + abs(from_col-to_col)  > 1)
		{
			MessageNo = 7;
			//strcat(Message,"**cant eat**");
			IsCurrent = false;
		}
		//judge pawn
		else if(from_chess_no % 7 == 0)
		{
			if(to_chess_no % 7 != 0 && to_chess_no % 7 != 6)
			{
				MessageNo = 8;
				//strcat(Message,"**pawn only eat pawn and king**");
				IsCurrent = false;
			}
		}
		//judge king
		else if(from_chess_no % 7 == 6 && to_chess_no % 7 == 0)
		{
			MessageNo = 9;
			//strcat(Message,"**king can't eat pawn**");
			IsCurrent = false;
		}
		else if(from_chess_no % 7 < to_chess_no% 7)
		{
			MessageNo = 10;
			//strcat(Message,"**cant eat**");
			IsCurrent = false;
		}
	}
	return IsCurrent;
}
// Referee
bool MyAI::Referee(const int* chess, const int from_location_no, const int to_location_no, const int UserId)
{
	int MessageNo = 0;
	bool IsCurrent = true;
	int from_chess_no = chess[from_location_no];
	int to_chess_no = chess[to_location_no];
	int from_row = from_location_no / 4;
	int to_row = to_location_no / 4;
	int from_col = from_location_no % 4;
	int to_col = to_location_no % 4;

	if(from_chess_no < 0 || ( to_chess_no < 0 && to_chess_no != CHESS_EMPTY) )
	{  
		MessageNo = 1;
		//strcat(Message,"**no chess can move**");
		//strcat(Message,"**can't move darkchess**");
		IsCurrent = false;
	}
	else if (from_chess_no >= 0 && from_chess_no / 7 != UserId)
	{
		MessageNo = 2;
		//strcat(Message,"**not my chess**");
		IsCurrent = false;
	}
	else if((from_chess_no / 7 == to_chess_no / 7) && to_chess_no >= 0)
	{
		MessageNo = 3;
		//strcat(Message,"**can't eat my self**");
		IsCurrent = false;
	}
	//check attack
	else if(to_chess_no == CHESS_EMPTY && abs(from_row-to_row) + abs(from_col-to_col)  == 1)//legal move
	{
		IsCurrent = true;
	}	
	else if(from_chess_no % 7 == 1 ) //judge gun
	{
		int row_gap = from_row-to_row;
		int col_gap = from_col-to_col;
		int between_Count = 0;
		//slant
		if(from_row-to_row == 0 || from_col-to_col == 0)
		{
			//row
			if(row_gap == 0) 
			{
				for(int i=1;i<abs(col_gap);i++)
				{
					int between_chess;
					if(col_gap>0)
						between_chess = chess[from_location_no-i] ;
					else
						between_chess = chess[from_location_no+i] ;
					if(between_chess != CHESS_EMPTY)
						between_Count++;
				}
			}
			//column
			else
			{
				for(int i=1;i<abs(row_gap);i++)
				{
					int between_chess;
					if(row_gap > 0)
						between_chess = chess[from_location_no-4*i] ;
					else
						between_chess = chess[from_location_no+4*i] ;
					if(between_chess != CHESS_EMPTY)
						between_Count++;
				}
				
			}
			
			if(between_Count != 1 )
			{
				MessageNo = 4;
				//strcat(Message,"**gun can't eat opp without between one piece**");
				IsCurrent = false;
			}
			else if(to_chess_no == CHESS_EMPTY)
			{
				MessageNo = 5;
				//strcat(Message,"**gun can't eat opp without between one piece**");
				IsCurrent = false;
			}
		}
		//slide
		else
		{
			MessageNo = 6;
			//strcat(Message,"**cant slide**");
			IsCurrent = false;
		}
	}
	else // non gun
	{
		//judge pawn or king

		//distance
		if( abs(from_row-to_row) + abs(from_col-to_col)  > 1)
		{
			MessageNo = 7;
			//strcat(Message,"**cant eat**");
			IsCurrent = false;
		}
		//judge pawn
		else if(from_chess_no % 7 == 0)
		{
			if(to_chess_no % 7 != 0 && to_chess_no % 7 != 6)
			{
				MessageNo = 8;
				//strcat(Message,"**pawn only eat pawn and king**");
				IsCurrent = false;
			}
		}
		//judge king
		else if(from_chess_no % 7 == 6 && to_chess_no % 7 == 0)
		{
			MessageNo = 9;
			//strcat(Message,"**king can't eat pawn**");
			IsCurrent = false;
		}
		else if(from_chess_no % 7 < to_chess_no% 7)
		{
			MessageNo = 10;
			//strcat(Message,"**cant eat**");
			IsCurrent = false;
		}
	}
	return IsCurrent;
}

// always use my point of view, so use this->Color
double MyAI::Evaluate(const int* board){
	// total score
	double score = 1943; // 1*5+180*2+6*2+18*2+90*2+270*2+810*1
	// static material values
	// cover and empty are both zero
	static const double values[14] = {1,180,6,18,90,270,810,1,180,6,18,90,270,810};

	for(int i = 0; i < 32; i++){
		if(!(board[i] == CHESS_EMPTY || board[i] == CHESS_COVER)){
			if(board[i] / 7 == this->Color){
				score += values[board[i]];
			}else{
				score -= values[board[i]];
			}
		}
	}

	return score;
}
double MyAI::myEvaluate(const int* board){
	// total score
	double score = 350; // 1*5+180*2+6*2+18*2+90*2+270*2+810*1
	// static material values
	// cover and empty are both zero
	static const double values[14] = {15,29,2,5,15,48,80,15,29,2,5,15,48,80};
	int pawnCount=0,gunCount = 0;
	int opPawnCount=0,opGunCount = 0;
	bool opHasKing = false;
	bool hasKing = false;
	for(int i = 0; i < 32; i++){
		if(!(board[i] == CHESS_EMPTY || board[i] == CHESS_COVER)){
			if(board[i] / 7 == this->Color){
				score += values[board[i]];
				if(board[i]%7 == 0){
					pawnCount++;
				}
				else if(board[i]%7 == 1){
					gunCount++;
				}
				else if(board[i]%7 == 6){
					hasKing = true;
				}
				
			}else{
				score -= values[board[i]];
				if(board[i]%7 == 6){
					opHasKing = true;
				}
				else if(board[i]%7 == 0){
					opPawnCount++;
				}
				else if(board[i]%7 == 1){
					opGunCount++;
				}
			}
		}
	}
	if(opHasKing){
		score += pawnCount*15+gunCount*10;
	}
	else{
		score +=100;
	}
	if(hasKing){
		score -= opGunCount*10+opPawnCount*15;

	}
	return score;
}

double MyAI::Nega_max(const int* board, int* move, const int red_chess_num, const int black_chess_num, const int* cover_chess, const int color, const int depth, const int remain_depth){
	if(remain_depth == 0){ // reach limit of depth
		this->node++;
		return Evaluate(board) * (2*((depth&1)^1)-1); // odd: *-1, even: *1
	}else if(red_chess_num == 0 || black_chess_num == 0){ // terminal node (no chess type)
		this->node++;
		return Evaluate(board) * (2*((depth&1)^1)-1);
	}

	int Result[1024];
	// Moves[] = {move} U {flip}, Chess[] = {remain chess}
	int Moves[2048], Chess[2048];
	int flip_count = 0, remain_count = 0, remain_total = 0;
	int move_count = 0;

	// move
	move_count = Expand(board, color, Result);
	memcpy(Moves, Result, sizeof(int)*move_count);
	// flip
	for(int j = 0; j < 14; j++){ // find remain chess
		if(cover_chess[j] > 0){
			Chess[remain_count] = j;
			remain_count++;
			remain_total += cover_chess[j];
		}
	}
	for(int i = 0; i < 32; i++){ // find cover
		if(board[i] == CHESS_COVER){
			Moves[move_count + flip_count] = i*100+i;
			flip_count++;
		}
	}

	if(move_count + flip_count == 0){ // terminal node (no move type)
		this->node++;
		return Evaluate(board) * (2*((depth&1)^1)-1);
	}else{
		double m = -DBL_MAX;
		int new_board[32], new_cover[14], new_move, new_red, new_black;
		// search deeper
		for(int i = 0; i < move_count; i++){ // move
			new_red = red_chess_num, new_black = black_chess_num;
			memcpy(new_board, board, sizeof(int)*32);
			memcpy(new_cover, cover_chess, sizeof(int)*14);
			
			MakeMove(new_board, &new_red, &new_black, new_cover, Moves[i], -1); // -1: NULL
			double t = -Nega_max(new_board, &new_move, new_red, new_black, new_cover, color^1, depth+1, remain_depth-1);
			if(t > m){ 
				m = t;
				*move = Moves[i];
			}else if(t == m){
				bool r = rand()%2;
				if(r) *move = Moves[i];
			}
		}
		for(int i = move_count; i < move_count + flip_count; i++){ // flip
			double total = 0;
			for(int k = 0; k < remain_count; k++){
				new_red = red_chess_num, new_black = black_chess_num;
				memcpy(new_board, board, sizeof(int)*32);
				memcpy(new_cover, cover_chess, sizeof(int)*14);
				
				MakeMove(new_board, &new_red, &new_black, new_cover, Moves[i], Chess[k]);
				double t = -Nega_max(new_board, &new_move, new_red, new_black, new_cover, color^1, depth+1, remain_depth-1);
				total += cover_chess[Chess[k]] * t;
			}

			double E_score = (total / remain_total); // calculate the expect value of flip
			if(E_score > m){ 
				m = E_score;
				*move = Moves[i];
			}else if(E_score == m){
				bool r = rand()%2;
				if(r) *move = Moves[i];
			}
		}
		return m;
	}
}
int chessCorner[4] = {10,11,21,22};
double MyAI::Nega_Scout(const int* board, int* move, const int red_chess_num, const int black_chess_num, const int* cover_chess, const int color, const int depth, const int remain_depth,double alpha, double beta,bool silence){
	int eatCount = 0;
	int Result[1024];
	//bool silence = false;
	if(remain_depth <= 0){ // reach limit of depth
		eatCount = ExpandEat(board, color, Result);
		if(eatCount==0){
			this->node++;
			return myEvaluate(board) * (2*((depth&1)^1)-1); // odd: *-1, even: *1
		}
		else{
			silence = false;
		}
	}else if(red_chess_num == 0 || black_chess_num == 0){ // terminal node (no chess type)
		this->node++;
		return myEvaluate(board) * (2*((depth&1)^1)-1);
	}

	//int Result[1024];
	// Moves[] = {move} U {flip}, Chess[] = {remain chess}
	int Moves[2048], Chess[2048];
	int flip_count = 0, remain_count = 0, remain_total = 0;
	int move_count = 0;

	// move
	if(silence)
		move_count = Expand(board, color, Result);
	else
	{
		move_count = eatCount;
	}
	
	memcpy(Moves, Result, sizeof(int)*move_count);
	// flip
	if(silence){
		for(int j = 0; j < 14; j++){ // find remain chess
			if(cover_chess[j] > 0){
				Chess[remain_count] = j;
				remain_count++;
				remain_total += cover_chess[j];
			}
		}
		std::unordered_set<int> tmpFlipResult;
		int tmpFlip = ExpandFlip(board, color,tmpFlipResult);
		if(tmpFlip == 0){
			for(int i = 0; i < 32; i++){ // find cover
				if(board[i] == CHESS_COVER){
					Moves[move_count + flip_count] = i*100+i;
					flip_count++;
				}
			}			
		}
		else{
			for ( auto it = tmpFlipResult.begin(); it != tmpFlipResult.end(); ++it ){
				Moves[move_count + flip_count] = *it;
				flip_count++;
			}
		}
		
	}


	if(move_count + flip_count == 0){ // terminal node (no move type)
		this->node++;
		return myEvaluate(board) * (2*((depth&1)^1)-1);
	}
	// else if(depth !=0 && move_count ==0){
	// 	this->node++;
	// 	return Evaluate(board) * (2*((depth&1)^1)-1);
	// }
	else if(flip_count == 32){
		int r = rand()%4;
		*move = 100*chessCorner[r]+chessCorner[r];
		return 0;

	}
	else{
		double m = -DBL_MAX;
		double n = beta;
		int new_board[32], new_cover[14], new_move, new_red, new_black;
		// search deeper
		for(int i = 0; i < move_count; i++){ // move
			new_red = red_chess_num, new_black = black_chess_num;
			memcpy(new_board, board, sizeof(int)*32);
			memcpy(new_cover, cover_chess, sizeof(int)*14);
			
			MakeMove(new_board, &new_red, &new_black, new_cover, Moves[i], -1); // -1: NULL
			double t = -Nega_Scout(new_board, &new_move, new_red, new_black, new_cover, color^1, depth+1, remain_depth-1,-n,-std::max(alpha,m),silence);
			if(t > m){ 
				if(n==beta || t>=beta || remain_depth<3){
					m=t;
				}
				else{
					m = -Nega_Scout(new_board, &new_move, new_red, new_black, new_cover, color^1, depth+1, remain_depth-1,-beta,-t,silence);
				}
				// m = t;
				*move = Moves[i];
			}else if(t == m){
				bool r = rand()%2;
				if(r) *move = Moves[i];
			}
			if(m>=beta) return m; 
		}
		if(silence){
			for(int i = move_count; i < move_count + flip_count; i++){ // flip
				double total = 0;
				for(int k = 0; k < remain_count; k++){
					new_red = red_chess_num, new_black = black_chess_num;
					memcpy(new_board, board, sizeof(int)*32);
					memcpy(new_cover, cover_chess, sizeof(int)*14);
					
					MakeMove(new_board, &new_red, &new_black, new_cover, Moves[i], Chess[k]);
					double t = -Nega_Scout(new_board, &new_move, new_red, new_black, new_cover, color^1, depth+1, remain_depth-1,-n,-std::max(alpha,m),silence);
					total += cover_chess[Chess[k]] * t;
				}

				double E_score = (total / remain_total); // calculate the expect value of flip
				if(E_score > m){ 
					m = E_score;
					*move = Moves[i];
				}else if(E_score == m){
					bool r = rand()%2;
					if(r) *move = Moves[i];
				}
			}

		}

		

		return m;
	}
}


//Display chess board
void MyAI::Pirnf_Chessboard()
{	
	char Mes[1024]="";
	char temp[1024];
	char myColor[10]="";
	if(Color == -99)
		strcpy(myColor,"Unknown");
	else if(this->Color == RED)
		strcpy(myColor,"Red");
	else
		strcpy(myColor,"Black");
	sprintf(temp,"------------%s-------------\n",myColor);
	strcat(Mes,temp);
	strcat(Mes,"<8> ");
	for(int i=0;i<32;i++)
	{
		if(i != 0 && i % 4 == 0)
		{
			sprintf(temp,"\n<%d> ",8-(i/4));
			strcat(Mes,temp);
		}
		char chess_name[4]="";
		Pirnf_Chess(this->Board[i],chess_name);
		sprintf(temp,"%5s", chess_name);
		strcat(Mes,temp);
	}
	strcat(Mes,"\n\n     ");
	for(int i=0;i<4;i++)
	{
		sprintf(temp," <%c> ",'a'+i);
		strcat(Mes,temp);
	}
	strcat(Mes,"\n\n");
	printf("%s",Mes);
}


//Print chess
void  MyAI::Pirnf_Chess(int chess_no,char *Result)
{
		// XX -> Empty
		if(chess_no == CHESS_EMPTY)
		{	
			strcat(Result, " - ");
			return;
		}
		//OO -> DarkChess
		else if(chess_no == CHESS_COVER)
		{
			strcat(Result, " X ");
			return;
		}
		/*if(Color == RED )
				strcat(Result, "R");
		else
				strcat(Result, "B");*/
		switch(chess_no)
		{
		case 0:
				strcat(Result, " P ");
				break;
		case 1:
				strcat(Result, " C ");
				break;
		case 2:
				strcat(Result, " N ");
				break;
		case 3:
				strcat(Result, " R ");
				break;
		case 4:
				strcat(Result, " M ");
				break;
		case 5:
				strcat(Result, " G ");
				break;
		case 6:
				strcat(Result, " K ");
				break;
		case 7:
				strcat(Result, " p ");
				break;
		case 8:
				strcat(Result, " c ");
				break;
		case 9:
				strcat(Result, " n ");
				break;
		case 10:
				strcat(Result, " r ");
				break;
		case 11:
				strcat(Result, " m ");
				break;
		case 12:
				strcat(Result, " g ");
				break;
		case 13:
				strcat(Result, " k ");
				break;
	}
}



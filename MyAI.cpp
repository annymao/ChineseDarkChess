#include "float.h"
#include "MyAI.h"

#define DEPTH_LIMIT 3
TreeNode* allHashNode[MAX_NODE_NUM]={NULL};
//std::vector<TreeNode*> nodePool;
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
	fprintf(stderr,"START!!\n\n");
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
	this->generateInitHash();
	this->Pirnf_Chessboard();
}
void MyAI::generateInitHash(){
	InitRandomValue();
	hashKey = InitHashKey(Board,Color);
	for(int i=0;i<MAX_NODE_NUM;i++){
		allHashNode[i] = NULL;
	}
}
void MyAI::generateMove(char move[6])
{
	
	/* generateMove Call by reference: change src,dst */
	this->node = 0;
	int startPoint = 0;
	int EndPoint = 0;

	int best_move = 0;
	double t;
	int thresholdDepth = DEPTH_LIMIT;
	struct timespec start, end;
	clock_gettime(CLOCK_REALTIME, &start);
	double wall_clock_in_seconds;
	int remainChess = this->Red_Chess_Num+ this->Black_Chess_Num;
	
	int hashIndex = hashKey%MAX_NODE_NUM;
	int isSaveEat=0;
	int returnDepth=0;
	if(remainChess < 28 && remainChess>8){
		bool isHit = false;
		if(allHashNode[hashIndex]!=NULL){
			if(allHashNode[hashIndex]->hashKey == hashKey &&compareBoard(Board,allHashNode[hashIndex]->Board)){
				isHit = true;
			}
			else{
				allHashNode[hashIndex]->init(this->Board,0,hashKey);
			}
		}
		else{
			allHashNode[hashIndex] = new TreeNode();
			allHashNode[hashIndex]->init(this->Board,0,hashKey);
		}
		while(wall_clock_in_seconds < TIME_LIMIT){
			thresholdDepth+=1;
			isSaveEat=0;
			if(isHit&&allHashNode[hashIndex]->depth>=thresholdDepth){
				best_move = allHashNode[hashIndex]->bestMove;
				t = allHashNode[hashIndex]->bestValue;
				thresholdDepth = allHashNode[hashIndex]->depth;
				isHit = false;
				fprintf(stderr,"HIT: %d\n",best_move);
				if(thresholdDepth>15)break;
				continue;
			} 
			allHashNode[hashIndex]->isSearch=true;
			t = Nega_Scout(allHashNode[hashIndex], &best_move, this->Red_Chess_Num, this->Black_Chess_Num, this->CoverChess, this->Color, 0, thresholdDepth,-DBL_MAX,DBL_MAX,true,0,start,isSaveEat,returnDepth);
			allHashNode[hashIndex]->isSearch=false;
			if(isSaveEat==1)break;
			clock_gettime(CLOCK_REALTIME, &end);
			wall_clock_in_seconds =(double)((end.tv_sec+end.tv_nsec*1e-9) - (double)(start.tv_sec+start.tv_nsec*1e-9));	
			if(thresholdDepth>15)break;		
		}
		fprintf(stderr,"Depth: %d\n",thresholdDepth); 
	}
	else if(remainChess <= 12){

		double t = Nega_max(this->Board, &best_move, this->Red_Chess_Num, this->Black_Chess_Num, this->CoverChess, this->Color, 0, DEPTH_LIMIT+2,start);
		
	}
	else{
		if(allHashNode[hashIndex]!=NULL){
			if(allHashNode[hashIndex]->hashKey == hashKey&&compareBoard(Board,allHashNode[hashIndex]->Board)){
				if(allHashNode[hashIndex]->depth>=thresholdDepth){
					best_move = allHashNode[hashIndex]->bestMove;
					t = allHashNode[hashIndex]->bestValue;
					fprintf(stderr,"HIT: %d\n",best_move);
				}
				else{
					allHashNode[hashIndex]->init(this->Board,0,hashKey);
					allHashNode[hashIndex]->isSearch=true;
					t = Nega_Scout(allHashNode[hashIndex], &best_move, this->Red_Chess_Num, this->Black_Chess_Num, this->CoverChess, this->Color, 0, DEPTH_LIMIT,-DBL_MAX,DBL_MAX,true,0,start,isSaveEat,returnDepth);
					allHashNode[hashIndex]->isSearch=false;
				}
			}
			else{
				allHashNode[hashIndex]->init(this->Board,0,hashKey);
				allHashNode[hashIndex]->isSearch=true;
				t = Nega_Scout(allHashNode[hashIndex], &best_move, this->Red_Chess_Num, this->Black_Chess_Num, this->CoverChess, this->Color, 0, DEPTH_LIMIT,-DBL_MAX,DBL_MAX,true,0,start,isSaveEat,returnDepth);
				allHashNode[hashIndex]->isSearch=false;
			}
		}
		else{
			allHashNode[hashIndex]=new TreeNode();
			allHashNode[hashIndex]->init(this->Board,0,hashKey);
			allHashNode[hashIndex]->isSearch=true;
			t = Nega_Scout(allHashNode[hashIndex], &best_move, this->Red_Chess_Num, this->Black_Chess_Num, this->CoverChess, this->Color, 0, DEPTH_LIMIT,-DBL_MAX,DBL_MAX,true,0,start,isSaveEat,returnDepth);
			allHashNode[hashIndex]->isSearch=false;
		}
		
	}
	
	startPoint = best_move/100;
	EndPoint   = best_move%100;
	fprintf(stderr,"Make Move: %d\n",best_move);
	sprintf(move, "%c%c-%c%c",'a'+(startPoint%4),'1'+(7-startPoint/4),'a'+(EndPoint%4),'1'+(7-EndPoint/4));

	char chess_Start[4]="";
	char chess_End[4]="";
	Pirnf_Chess(Board[startPoint],chess_Start);
	Pirnf_Chess(Board[EndPoint],chess_End);
	// printf("My result: \n--------------------------\n");
	// printf("Nega max: %lf (node: %d)\n", t, this->node);
	// printf("(%d) -> (%d)\n",startPoint,EndPoint);
	// printf("<%s> -> <%s>\n",chess_Start,chess_End);
	// printf("move:%s\n",move);
	// printf("--------------------------\n");
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
unsigned long long MyAI::MakeMove(int *board, int* red_chess_num, int* black_chess_num, int* cover_chess, const int move, const int chess, unsigned long long currentKey){
	int src = move/100, dst = move%100;
	unsigned long long newKey = 0;
	if(src == dst){ 
		board[src] = chess;
		cover_chess[chess]--;
		newKey = UpdateHashForFlip(currentKey,chess,src);
		
	}else { 
		if(board[dst] != CHESS_EMPTY){
			if(board[dst] / 7 == 0){
				(*red_chess_num)--;
			}else{
				(*black_chess_num)--;
			}
			newKey = UpdateHashForEat(currentKey,board[src],src,board[dst],dst);
		}
		else{
			newKey = UpdateHashForMove(currentKey,board[src],src,dst);
		}
		board[dst] = board[src];
		board[src] = CHESS_EMPTY;
	}
	return newKey;
}

void MyAI::MakeMove(int* board, int* red_chess_num, int* black_chess_num, int* cover_chess, const char move[6])
{ 
	int src, dst;
	src = ('8'-move[1])*4+(move[0]-'a');
	if(move[2]=='('){ 
		char chessName =  GetFin(move[3]);
		printf("# call flip(): flip(%d,%d) = %d\n",src, src, chessName); 
		
		int flipChessID = ConvertChessNo(chessName);
		board[src] =flipChessID;
		cover_chess[flipChessID]--;
		this->hashKey = UpdateHashForFlip(this->hashKey,flipChessID,src);
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
			this->hashKey = UpdateHashForEat(this->hashKey,board[src],src,board[dst],dst);
		}
		else{
			this->hashKey = UpdateHashForMove(this->hashKey,board[src],src,dst);
		}
		board[dst] = board[src];
		board[src] = CHESS_EMPTY;
		
		Pirnf_Chessboard();
	}
	/* init time */
}

int MyAI::Expand(const int* board, int red_chess_num, int black_chess_num,const int* cover_chess,const int color,int *ResultEat, int* ResultMove,int* saveEat,int &saveEatCount)
{
	int ResultMoveCount = 0,ResultEatCount=0,ResultCount=0;
	int DangerousMove[256];
	int dangerousCount =0;
	int new_board[32], new_cover[14], new_move, new_red, new_black;
	saveEatCount=0;
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
					new_red = red_chess_num, new_black = black_chess_num;
					int refreeSuccess = RefereeEat(board,i,rowCount,color); 
					bool isSave = false;
					if(refreeSuccess == 1 || refreeSuccess == 2){
						memcpy(new_board, board, sizeof(int)*32);
						memcpy(new_cover, cover_chess, sizeof(int)*14);
						MakeMove(new_board, &new_red, &new_black, new_cover, i*100+rowCount, -1);
						int Move[4] = {rowCount-4,rowCount+1,rowCount+4,rowCount-1};
						isSave = true;
						for(int k=0; k<4;k++)
						{
							if(Move[k] >= 0 && Move[k] < 32)
							{

								if(RefereeEat(new_board,Move[k],rowCount,color^1)==2)
								{
									isSave = false;
								}
							}
						}
						int currentCol = rowCount%4;
						for(int r = 0;r<8;r++){
							int pos = r*4+currentCol;
							if(pos>=0&&pos<=32 &&new_board[pos]%7==1 &&new_board[pos]/7!=color){
								if(RefereeEat(new_board,pos,rowCount,color^1)==2)
								{
									isSave = false;
								}
							}

						}
						int currentRow = rowCount/4;
						for(int c = 0;c<4;c++){
							int pos = currentRow*4+c;
							if(pos>=0&&pos<=32 &&new_board[pos]%7==1 &&new_board[pos]/7!=color){
								if(RefereeEat(new_board,pos,rowCount,color^1)==2)
								{
									isSave = false;
								}
							}

						}
					}

					if(refreeSuccess==1)
					{
						if(isSave){
							ResultMove[ResultMoveCount] = i*100+rowCount;
							ResultMoveCount++;
						}
						else{
							DangerousMove[dangerousCount] = i*100+rowCount;
							dangerousCount++;
						}
					}
					else if(refreeSuccess==2)
					{
						if(isSave){
							saveEat[saveEatCount] = i*100+rowCount;
							saveEatCount++;
						}
						else{
							ResultEat[ResultEatCount] = i*100+rowCount;
							ResultEatCount++;
						}
						
					}
				}
				for(int colCount=col; colCount<32;colCount += 4)
				{
					new_red = red_chess_num, new_black = black_chess_num;
					bool isSave = false;
					int refreeSuccess =RefereeEat(board,i,colCount,color); 
					if(refreeSuccess ==1 || refreeSuccess==2){
						memcpy(new_board, board, sizeof(int)*32);
						memcpy(new_cover, cover_chess, sizeof(int)*14);
						MakeMove(new_board, &new_red, &new_black, new_cover, i*100+colCount, -1);
						isSave = true;
						int Move[4] = {colCount-4,colCount+1,colCount+4,colCount-1};
						for(int k=0; k<4;k++)
						{
							if(Move[k] >= 0 && Move[k] < 32)
							{

								if(RefereeEat(new_board,Move[k],colCount,color^1)==2)
								{
									isSave = false;
								}
							}
						}
						int currentCol = colCount%4;
						for(int r = 0;r<8;r++){
							int pos = r*4+currentCol;
							if(pos>=0&&pos<=32 &&new_board[pos]%7==1 &&new_board[pos]/7!=color){
								if(RefereeEat(new_board,pos,colCount,color^1)==2)
								{
									isSave = false;
								}
							}

						}
						int currentRow = (colCount/4);
						for(int c = 0;c<4;c++){
							int pos = currentRow*4+c;
							if(pos>=0&&pos<=32 &&new_board[pos]%7==1 &&new_board[pos]/7!=color){
								if(RefereeEat(new_board,pos,colCount,color^1)==2)
								{
									isSave = false;
								}
							}

						}						
					}

					if(refreeSuccess==1)
					{
						if(isSave){
							ResultMove[ResultMoveCount] = i*100+colCount;
							ResultMoveCount++;
						}
						else{
							DangerousMove[dangerousCount] = i*100+colCount;
							dangerousCount++;
						}
					}
					else if(refreeSuccess==2)
					{
						
						if(isSave){
							saveEat[saveEatCount] = i*100+colCount;
							saveEatCount++;
						}
						else{
							ResultEat[ResultEatCount] = i*100+colCount;
							ResultEatCount++;
						}
					}
				}
			}
			else
			{
				int Move[4] = {i-4,i+1,i+4,i-1};
				for(int k=0; k<4;k++)
				{
					if(Move[k] >= 0 && Move[k] < 32)
					{
						bool isSave = false;
						int refreeSuccess =RefereeEat(board,i,Move[k],color); 
						if(refreeSuccess == 1 || refreeSuccess == 2){
							new_red = red_chess_num, new_black = black_chess_num;
							memcpy(new_board, board, sizeof(int)*32);
							memcpy(new_cover, cover_chess, sizeof(int)*14);
							MakeMove(new_board, &new_red, &new_black, new_cover, i*100+Move[k], -1);
							isSave = true;
							int Move2[4] = {Move[k]-4,Move[k]+1,Move[k]+4,Move[k]-1};
							for(int h=0; h<4;h++)
							{
								if(Move2[h] >= 0 && Move2[h] < 32)
								{
									if(RefereeEat(new_board,Move2[h],Move[k],color^1)==2)
									{
										isSave = false;
									}
								}
							}
							int currentCol = Move[k]%4;
							for(int r = 0;r<8;r++){
								int pos = r*4+currentCol;
								if(pos>=0&&pos<=32 &&new_board[pos]%7==1 &&new_board[pos]/7!=color){
									if(RefereeEat(new_board,pos,Move[k],color^1)==2)
									{
										isSave = false;
									}
								}

							}
							int currentRow = (Move[k]/4);
							for(int c = 0;c<4;c++){
								int pos = currentRow*4+c;
								if(pos>=0&&pos<=32 &&new_board[pos]%7==1 &&new_board[pos]/7!=color){
									if(RefereeEat(new_board,pos,Move[k],color^1)==2)
									{
										isSave = false;
									}
								}

							}

						}
						

						if(refreeSuccess==1)
						{
							if(isSave){
								ResultMove[ResultMoveCount] = i*100+Move[k];
								ResultMoveCount++;
							}
							else{
								DangerousMove[dangerousCount] = i*100+Move[k];
								dangerousCount++;
							}
						}
						else if(refreeSuccess==2)
						{

							if(isSave){
								saveEat[saveEatCount] = i*100+Move[k];
								saveEatCount++;
							}
							else{
								ResultEat[ResultEatCount] = i*100+Move[k];
								ResultEatCount++;
							}
						}
					}
				}
			}
		
		}
	}
	if(ResultMoveCount == 0){
		ResultMoveCount = dangerousCount;
		memcpy(ResultMove,DangerousMove,sizeof(int)*ResultMoveCount);
	}
	ResultCount = ResultEatCount*128+ResultMoveCount;
	return ResultCount;
}
 


//TODO: Do not flip the chess that is near ourself
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


				int Move[4] = {-4,-1,1,4};
				for(int k=0; k<4;k++)
				{
					int currentMove = i+Move[k];
					if(currentMove/4!=i/4 && currentMove%4!=i%4)continue;
					if(currentMove >= 0 && currentMove < 32 && board[currentMove]==CHESS_COVER)
					{
						bool canFlip = true;
						for(int h =0;h<4;h++){
							int tmp = currentMove + Move[h];
							if(tmp/4!=currentMove/4 && currentMove%4!=tmp%4)continue;
							if(tmp >= 0 && tmp < 32 && board[tmp]!=CHESS_COVER){
								if((board[tmp]%7>2 &&board[tmp]/7 != color)||(board[tmp]/7==color && board[tmp]%7>0)){

									canFlip = false;
									break;
								}
								
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
				int middleCount = 0;
				for(int k=1; k<4;k++)
				{
					//if(k==0) continue;
					
					int currentMove = i+k;
					if(currentMove/4!=i/4)continue;
					int middleCount = 0;
					if(currentMove >= 0 && currentMove < 32){//same row
						if(middleCount==1 && board[currentMove]==CHESS_COVER){
							bool canFlip = true;
							int Move[4] = {-4,-1,1,4};
							for(int h =0;h<4;h++){
								int tmp = currentMove + Move[h];
								if(currentMove/4!=tmp/4 && currentMove%4!=tmp%4)continue;
								if(tmp >= 0 && tmp < 32 && board[tmp]!=CHESS_COVER){
									if((board[tmp]%7>1 &&board[tmp]/7 != color)||(board[tmp]/7==color && board[tmp]%7>0)){
										
										canFlip = false;
										break;
									}
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
				middleCount = 0;
				for(int k=-1; k>-4;k--)
				{
					//if(k==0) continue;
					int currentMove = i+k;
					if(currentMove/4!=i/4 && currentMove%4!=i%4)continue;
					
					if(currentMove >= 0 && currentMove < 32 && currentMove/4 == i/4){//same row
						if(middleCount==1 && board[currentMove]==CHESS_COVER){
							bool canFlip = true;
							int Move[4] = {-4,-1,1,4};
							for(int h =0;h<4;h++){
								int tmp = currentMove + Move[h];
								if(currentMove/4!=tmp/4 && currentMove%4!=tmp%4)continue;
								if(tmp >= 0 && tmp < 32 && board[tmp]!=CHESS_COVER){
									if((board[tmp]%7>1 &&board[tmp]/7 != color)||(board[tmp]/7==color && board[tmp]%7>0)){
										canFlip = false;
										break;
									}
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
				middleCount = 0;
				for(int k=4; k<32;k+=4)
				{
					//if(k==0) continue;
					int currentMove = i+k;
					if(currentMove > 32)break;
					
					if(currentMove >= 0 && currentMove < 32){
						if(middleCount==1 && board[currentMove]==CHESS_COVER){
							bool canFlip = true;
							int Move[4] = {-4,-1,1,4};
							for(int h =0;h<4;h++){
								int tmp = currentMove + Move[h];
								if(currentMove/4==tmp/4 || currentMove%4==tmp%4){
									if(tmp >= 0 && tmp < 32 && board[tmp]!=CHESS_COVER){
										if((board[tmp]%7>1 &&board[tmp]/7 != color)||(board[tmp]/7==color && board[tmp]%7>0)){
											
											canFlip = false;
											break;
										}
									}									
								}

							}
							if(canFlip){

								Result.insert(currentMove*100+currentMove);
								ResultCount++;
							}
							break;
						}
						if(board[currentMove]!=CHESS_EMPTY){
							middleCount++;

							if(middleCount > 1)break;
						}
					}
			
				}
				middleCount = 0;
				for(int k=-4; k>-32;k-=4)
				{
					//if(k==0) continue;
					int currentMove = i+k;
					if(currentMove<0)break;
					if(currentMove >= 0 && currentMove < 32){
						if(middleCount==1 && board[currentMove]==CHESS_COVER){
							bool canFlip = true;
							int Move[4] = {-4,-1,1,4};
							for(int h =0;h<4;h++){
								int tmp = currentMove + Move[h];
								if(currentMove/4==tmp/4 || currentMove%4==tmp%4){
									if(tmp >= 0 && tmp < 32 && board[tmp]!=CHESS_COVER){
										if((board[tmp]%7>1 &&board[tmp]/7 != color)||(board[tmp]/7==color && board[tmp]%7>0)){
											canFlip = false;
											break;
										}
									}									
								}

							}
							if(canFlip){
								Result.insert(currentMove*100+currentMove);
								ResultCount++;
							}
							break;
						}
						if(board[currentMove]!=CHESS_EMPTY){
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
//TODO: Merge to refree
int MyAI::RefereeEat(const int* chess, const int from_location_no, const int to_location_no, const int UserId)
{
	int MessageNo = 0;
	int IsCurrent = 2;
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
		IsCurrent = 0;
	}
	else if (from_chess_no >= 0 && from_chess_no / 7 != UserId)
	{
		MessageNo = 2;
		//strcat(Message,"**not my chess**");
		IsCurrent = 0;
	}
	else if((from_chess_no / 7 == to_chess_no / 7) && to_chess_no >= 0)
	{
		MessageNo = 3;
		//strcat(Message,"**can't eat my self**");
		IsCurrent = 0;
	}
	//check attack
	else if(to_chess_no == CHESS_EMPTY && abs(from_row-to_row) + abs(from_col-to_col)  == 1)//legal move
	{
		IsCurrent = 1;
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
				IsCurrent = 0;
			}
			else if(to_chess_no == CHESS_EMPTY)
			{
				MessageNo = 5;
				//strcat(Message,"**gun can't eat opp without between one piece**");
				IsCurrent = 0;
			}
		}
		//slide
		else
		{
			MessageNo = 6;
			//strcat(Message,"**cant slide**");
			IsCurrent = 0;
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
			IsCurrent = 0;
		}
		//judge pawn
		else if(from_chess_no % 7 == 0)
		{
			if(to_chess_no % 7 != 0 && to_chess_no % 7 != 6)
			{
				MessageNo = 8;
				//strcat(Message,"**pawn only eat pawn and king**");
				IsCurrent = 0;
			}
		}
		//judge king
		else if(from_chess_no % 7 == 6 && to_chess_no % 7 == 0)
		{
			MessageNo = 9;
			//strcat(Message,"**king can't eat pawn**");
			IsCurrent = 0;
		}
		else if(from_chess_no % 7 < to_chess_no% 7)
		{
			MessageNo = 10;
			//strcat(Message,"**cant eat**");
			IsCurrent = 0;
		}
	}
	return IsCurrent;
}
// Referee

// always use my point of view, so use this->Color
double MyAI::myEvaluate(const int* board){
	//printf("\n\n\nXXXXXXX_____MY_EVALUATE_____XXXXX\n\n\n");
	// total score
	double score = 1000; // 1*5+180*2+6*2+18*2+90*2+270*2+810*1
	// static material values
	// cover and empty are both zero
	static const double values[14] = {1,180,6,18,90,270,810,1,180,6,18,90,270,810};
	int pawnCount=0,gunCount = 0;
	int opPawnCount=0,opGunCount = 0;
	bool opHasKing = false;
	bool hasKing = false;
	int myChess[14] = {0};
	int oponentChess[14] = {0};
	//int dist[14][14] = {{0}};
	int coef[14][14] = {{0,0,-1,-1,-1,-1,1},
						   {0,0,-1,-1,-1,-1,-1},
						   {1,1,0,-1,-1,-1,-1},
						   {1,1,1,0,-1,-1,-1},
						   {1,1,1,3,0,-2,-2},
						   {1,1,1,2,3,0,-2},
						   {-1,1,1,1,2,2,0}};
	int myMax =0, opMax = 0;
	for(int i = 0; i < 32; i++){
		if(!(board[i] == CHESS_EMPTY || board[i] == CHESS_COVER)){
			if(board[i] / 7 == this->Color){
				myChess[board[i]%7]=i;
				score+=values[board[i]];
				if(board[i]%7>myMax){
					myMax = board[i]%7;
				}
			}else{
				oponentChess[board[i]%7]=i;
				score-=values[board[i]];
				if(board[i]%7>opMax){
					opMax = board[i]%7;
				}
			}
		}
	}
	double distanceCof = 0;
	if(opMax<myMax)distanceCof = 10;
	else distanceCof = 0.1;
	for(int i=0;i<14;i++){
		for(int j=0;j<14;j++){
			if(coef[i][j]>0)
				score+=distanceCof*13*(double)coef[i][j]/(double)(abs(myChess[i]/4 - oponentChess[j]/4)+abs(myChess[i]%4-oponentChess[j]%4)+1);
			else{
				score-= ((double)(coef[i][j]*(abs(myChess[i]/4 - oponentChess[j]/4)+abs(myChess[i]%4-oponentChess[j]%4))))/distanceCof;
			}
		}
	}

	return score;
}

int chessCorner[4] = {10,11,21,22};
double MyAI::Nega_Scout( TreeNode* CDCNode, int* move, const int red_chess_num, const int black_chess_num, const int* cover_chess, const int color, const int depth,  int remain_depth,double alpha, double beta,bool silence,double exchangeCount,struct timespec start,int& isSaveEat,int& returnDepth){
	int eat_count = 0;
	
	int ResultEat[256],ResultMove[256],saveEat[256];
	int saveEatCount;
	//bool silence = false;
	struct timespec end;
	clock_gettime(CLOCK_REALTIME, &end);
	isSaveEat =0;
	returnDepth = 0;
	if(remain_depth <= 0 || ((double)((end.tv_sec+end.tv_nsec*1e-9) - (double)(start.tv_sec+start.tv_nsec*1e-9)))>TIME_LIMIT){ // reach limit of depth
		returnDepth =  std::max(0,remain_depth);
		return TAEvaluate(CDCNode->Board) * (2*((depth&1)^1)-1);
	}else if(red_chess_num == 0 || black_chess_num == 0){ // terminal node (no chess type)
		this->node++;
		return TAEvaluate(CDCNode->Board) * (2*((depth&1)^1)-1);
	}
	

	//int Result[1024];
	// Moves[] = {move} U {flip}, Chess[] = {remain chess}
	int Moves[512], Chess[512];
	int flip_count = 0, remain_count = 0, remain_total = 0;
	int move_count = 0;
	int new_board[32], new_cover[14], new_move, new_red, new_black;

	// move
	if(silence){
		move_count = Expand(CDCNode->Board,red_chess_num, black_chess_num,cover_chess,color, ResultEat,ResultMove,saveEat,saveEatCount);
		if(depth ==0)
		fprintf(stderr,"Expand:%d %d %d\n",saveEatCount,move_count/128,move_count%128);
		if(saveEatCount>0){
			int maxMove = saveEat[0],maxEat = CDCNode->Board[saveEat[0]%100];
			for(int i=1;i<saveEatCount;i++){
				int eat = CDCNode->Board[saveEat[i]%100];
				if((eat>maxEat&&maxEat!=1)||eat ==1){
					maxEat = eat;
					maxMove = saveEat[i];
				}else if(eat == maxEat){
					if(CDCNode->Board[maxMove/100] > CDCNode->Board[saveEat[i]/100]){
						maxMove = saveEat[i];
					}
				}
				
			}
			*move = maxMove;
			new_red = red_chess_num, new_black = black_chess_num;
			memcpy(new_board, CDCNode->Board, sizeof(int)*32);
			memcpy(new_cover, cover_chess, sizeof(int)*14);
			MakeMove(new_board, &new_red, &new_black, new_cover,maxMove, -1); // -1: NULL

			if(depth ==0 ){
				isSaveEat=1;
				fprintf(stderr,"Save Eat: %d",*move);
			}
			return TAEvaluate(new_board)* (2*((depth&1)^1)-1);
		}
		eat_count = move_count/128;
		move_count = move_count%128;

	}
	else
	{
		//move_count = eatCount%128;
		move_count =0;
		//memcpy(Moves, ResultEat, sizeof(int)*move_count);
	}
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
		int tmpFlip = ExpandFlip(CDCNode->Board, color,tmpFlipResult);
		if(tmpFlip == 0){
			for(int i = 0; i < 32; i++){ // find cover
				if(CDCNode->Board[i] == CHESS_COVER){
					Moves[flip_count] = i*100+i;
					flip_count++;
				}
			}			
		}
		else{
			for ( auto it = tmpFlipResult.begin(); it != tmpFlipResult.end(); ++it ){
				
				Moves[flip_count] = *it;
				flip_count++;
			}
		}
		std::unordered_set<int>().swap(tmpFlipResult);
		
	}
		
	if(move_count + eat_count + flip_count == 0){ // terminal node (no move type)
		this->node++;
		return TAEvaluate(CDCNode->Board) * (2*((depth&1)^1)-1);
	}

	else if(flip_count == 32){
		int r = rand()%4;
		*move = 100*chessCorner[r]+chessCorner[r];
		return 0;

	}
	else if(move_count + eat_count + flip_count==1){
		//*move = Moves[0];
		if(move_count==1){
			*move = ResultMove[0];
		}
		else if(eat_count==1){
			*move = ResultEat[0];
		}
		else{
			*move = Moves[0];
		}
		return TAEvaluate(CDCNode->Board) * (2*((depth&1)^1)-1);
	}
	else{
		double m = -DBL_MAX;
		double n = beta;
		int src=0,dst=0,silenceExchange = 0;
		TreeNode* newNode = new TreeNode();
		// search deeper
		///MOVE
		for(int i = 0; i < move_count; i++){ // move
			new_red = red_chess_num, new_black = black_chess_num;
			memcpy(new_cover, cover_chess, sizeof(int)*14);
			memcpy(new_board, CDCNode->Board, sizeof(int)*32);

			unsigned long long newKey = MakeMove(new_board, &new_red, &new_black, new_cover, ResultMove[i], -1,CDCNode->hashKey); // -1: NULL

			double t =0;
			int hashIndex = newKey%MAX_NODE_NUM;
			if(allHashNode[hashIndex]!=NULL){
				if(!allHashNode[hashIndex]->isSearch){
					allHashNode[hashIndex]->isSearch=true;
					if(allHashNode[hashIndex]->hashKey == newKey &&compareBoard(new_board,allHashNode[hashIndex]->Board)){
						if(allHashNode[hashIndex]->depth>=remain_depth-1){
							new_move = allHashNode[hashIndex]->bestMove;
							t = allHashNode[hashIndex]->bestValue;
						}
						else{
							t = -Nega_Scout(allHashNode[hashIndex], &new_move, new_red, new_black, new_cover, color^1, depth+1, remain_depth-1,-n,-std::max(alpha,m),silence,exchangeCount+silenceExchange,start,isSaveEat,returnDepth);
						}
					}else{
						//allHashNode[hashIndex]->init(new_board,0,newKey);
						t = -Nega_Scout(allHashNode[hashIndex], &new_move, new_red, new_black, new_cover, color^1, depth+1, remain_depth-1,-n,-std::max(alpha,m),silence,exchangeCount+silenceExchange,start,isSaveEat,returnDepth);
					}	
					allHashNode[hashIndex]->isSearch=false;				
				}
				else{
					newNode->init(new_board,0,newKey);
					t = -Nega_Scout(newNode, &new_move, new_red, new_black, new_cover, color^1, depth+1, remain_depth-1,-n,-std::max(alpha,m),silence,exchangeCount+silenceExchange,start,isSaveEat,returnDepth);

				}
			}
			else{
				allHashNode[hashIndex] = new TreeNode();
				allHashNode[hashIndex]->init(new_board,0,newKey);
				allHashNode[hashIndex]->isSearch=true;
				t = -Nega_Scout(allHashNode[hashIndex], &new_move, new_red, new_black, new_cover, color^1, depth+1, remain_depth-1,-n,-std::max(alpha,m),silence,exchangeCount+silenceExchange,start,isSaveEat,returnDepth);
				allHashNode[hashIndex]->isSearch=false;
			}
			isSaveEat=0;
			if(t > m){ 
				if(n==beta || t>=beta || remain_depth<3){
					m=t;
				}
				else{
					if(allHashNode[hashIndex]->isSearch != true){
						m = -Nega_Scout(allHashNode[hashIndex], &new_move, new_red, new_black, new_cover, color^1, depth+1, remain_depth-1,-beta,-t,silence,exchangeCount+silenceExchange,start,isSaveEat,returnDepth);
						isSaveEat=0;						
					}
					else{
						m = -Nega_Scout(newNode, &new_move, new_red, new_black, new_cover, color^1, depth+1, remain_depth-1,-beta,-t,silence,exchangeCount+silenceExchange,start,isSaveEat,returnDepth);
						isSaveEat=0;
					}
				}
				// m = t;
				
				*move = ResultMove[i];
				if(depth ==0 )fprintf(stderr,"currentBest Move : %d\n",*move);
			}
			else if(t == m){
				bool r = rand()%2;
				if(r) *move = ResultMove[i];
			}
			if(m>=beta){
				CDCNode->depth = remain_depth-returnDepth;
				CDCNode->bestValue = m;
				CDCNode->bestMove = *move;
				if(depth == 0)fprintf(stderr,"return Move: %d\n",*move);
				//allHashNode[CDCNode->hashKey%MAX_NODE_NUM] = CDCNode;
				delete newNode;
				return m; 
			} 
			n=std::max(alpha,m)+1;
		}
		/// Exchange Eat
		for(int i = 0; i < eat_count; i++){ // move
			new_red = red_chess_num, new_black = black_chess_num;
			//memcpy(new_board, board, sizeof(int)*32);
			memcpy(new_cover, cover_chess, sizeof(int)*14);
			memcpy(new_board, CDCNode->Board, sizeof(int)*32);
			unsigned long long newKey = MakeMove(new_board, &new_red, &new_black, new_cover, ResultEat[i], -1,CDCNode->hashKey); // -1: NULL
			double t =0;
			int hashIndex = newKey%MAX_NODE_NUM;
			
			if(allHashNode[hashIndex]!=NULL){
				if(!allHashNode[hashIndex]->isSearch){
					allHashNode[hashIndex]->isSearch=true;
					if(allHashNode[hashIndex]->hashKey == newKey &&compareBoard(new_board,allHashNode[hashIndex]->Board)){
						if(allHashNode[hashIndex]->depth>=remain_depth-1){
							new_move = allHashNode[hashIndex]->bestMove;
							t = allHashNode[hashIndex]->bestValue;
						}
						else{
							t = -Nega_Scout(allHashNode[hashIndex], &new_move, new_red, new_black, new_cover, color^1, depth+1, remain_depth-1,-n,-std::max(alpha,m),silence,exchangeCount+silenceExchange,start,isSaveEat,returnDepth);
						}
					}else{
						//allHashNode[hashIndex]->init(new_board,0,newKey);
						t = -Nega_Scout(allHashNode[hashIndex], &new_move, new_red, new_black, new_cover, color^1, depth+1, remain_depth-1,-n,-std::max(alpha,m),silence,exchangeCount+silenceExchange,start,isSaveEat,returnDepth);
					}	
					allHashNode[hashIndex]->isSearch=false;				
				}
				else{
					newNode->init(new_board,0,newKey);
					t = -Nega_Scout(newNode, &new_move, new_red, new_black, new_cover, color^1, depth+1, remain_depth-1,-n,-std::max(alpha,m),silence,exchangeCount+silenceExchange,start,isSaveEat,returnDepth);

				}
			}
			else{
				allHashNode[hashIndex] = new TreeNode();
				allHashNode[hashIndex]->init(new_board,0,newKey);
				allHashNode[hashIndex]->isSearch=true;
				t = -Nega_Scout(allHashNode[hashIndex], &new_move, new_red, new_black, new_cover, color^1, depth+1, remain_depth-1,-n,-std::max(alpha,m),silence,exchangeCount+silenceExchange,start,isSaveEat,returnDepth);
				allHashNode[hashIndex]->isSearch=false;
			}
			isSaveEat=0;
			if(t > m){ 
				if(n==beta || t>=beta || remain_depth<3){
					m=t;
				}
				else{
					if(allHashNode[hashIndex]->isSearch != true){
						m = -Nega_Scout(allHashNode[hashIndex], &new_move, new_red, new_black, new_cover, color^1, depth+1, remain_depth-1,-beta,-t,silence,exchangeCount+silenceExchange,start,isSaveEat,returnDepth);
						isSaveEat=0;						
					}
					else{
						m = -Nega_Scout(newNode, &new_move, new_red, new_black, new_cover, color^1, depth+1, remain_depth-1,-beta,-t,silence,exchangeCount+silenceExchange,start,isSaveEat,returnDepth);
						isSaveEat=0;
					}
				}
				// m = t;
				*move = ResultEat[i];
				if(depth ==0 )fprintf(stderr,"currentBest Exchange: %d\n",*move);
			}
			else if(t == m){
				bool r = rand()%2;
				if(r) *move = ResultEat[i];
			}

			if(m>=beta){
				CDCNode->depth = remain_depth-returnDepth;
				CDCNode->bestValue = m;
				CDCNode->bestMove = *move;
				if(depth ==0 )fprintf(stderr,"returnExchange:%d\n",*move);
				//allHashNode[CDCNode->hashKey%MAX_NODE_NUM] = CDCNode;
				delete newNode;
				return m; 
			} 
			n=std::max(alpha,m)+1;
		}
		if(depth%2==0){
			for(int i = 0; i < flip_count; i++){ // flip
				double E_score =((double)(2*((depth&1)^1)-1))*Star0_F_3(CDCNode->Board, std::max(alpha,m),beta,red_chess_num, black_chess_num,  cover_chess, Chess, Moves[i],remain_count,remain_total,color,remain_depth,start,depth%2);
				if(E_score > m){ 
					m = E_score;
					*move = Moves[i];
				}
				else if(E_score == m){
					bool r = rand()%2;
					if(r) *move = Moves[i];
				}
			}

		}	
		if(depth ==0 )fprintf(stderr,"FinalreturnMove: %d\n",*move);
		delete newNode;
		return m;
	}
}
double  MyAI::F_3(const int* board, double alpha, double beta,const int red_chess_num, const int black_chess_num,const int* cover_chess, const int color,int remain_depth,struct timespec start){
	struct timespec end;
	clock_gettime(CLOCK_REALTIME, &end);
	if(remain_depth <= 0 || ((double)((end.tv_sec+end.tv_nsec*1e-9) - (double)(start.tv_sec+start.tv_nsec*1e-9)))>TIME_LIMIT){
		return TAEvaluate(board);
	}
	else if(red_chess_num == 0 || black_chess_num == 0){
		return TAEvaluate(board);
	}
	int ResultEat[256],ResultMove[256],saveEat[256];
	int saveEatCount =0;
	int Moves[512], Chess[512];
	int flip_count = 0, remain_count = 0, remain_total = 0;
	int move_count = 0;

	// move
	int eat_count = 0;
	move_count = Expand(board,red_chess_num, black_chess_num,cover_chess,color, ResultEat,ResultMove,saveEat,saveEatCount);
	int new_board[32], new_cover[14], new_move, new_red, new_black;
	if(saveEatCount>0){
		//memcpy(Moves,saveEat,sizeof(int)*saveEatCount);	
		new_red = red_chess_num, new_black = black_chess_num;
		memcpy(new_board, board, sizeof(int)*32);
		memcpy(new_cover, cover_chess, sizeof(int)*14);
		int maxMove = saveEat[0],maxEat = board[saveEat[0]%100];
		for(int i=1;i<saveEatCount;i++){
			int eat = board[saveEat[i]%100];
			if((eat>maxEat&&maxEat!=1)||eat ==1){
				maxEat = eat;
				maxMove = saveEat[i];
			}else if(eat == maxEat){
				if(board[maxMove/100] > board[saveEat[i]/100]){
					maxMove = saveEat[i];
				}
			}
			
		}
		//*move = maxMove;
		MakeMove(new_board, &new_red, &new_black, new_cover,maxMove, -1); // -1: NULL
		return 	TAEvaluate(new_board);
	}
	eat_count = move_count/128;
	move_count = move_count%128;
	// flip

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
				Moves[flip_count] = i*100+i;
				flip_count++;
			}
		}			
	}
	else{
		for ( auto it = tmpFlipResult.begin(); it != tmpFlipResult.end(); ++it ){
			Moves[flip_count] = *it;
			flip_count++;
		}
	}
		
	if(move_count + eat_count+flip_count == 0){ // terminal node (no move type)
		return TAEvaluate(board);
	}
	else{
		double m = -DBL_MAX;
		// search deeper
		for(int i = 0; i < move_count; i++){ // move
			new_red = red_chess_num, new_black = black_chess_num;
			memcpy(new_board, board, sizeof(int)*32);
			memcpy(new_cover, cover_chess, sizeof(int)*14);
			MakeMove(new_board, &new_red, &new_black, new_cover, ResultMove[i], -1); // -1: NULL
			
			double t = G_3(new_board,  std::max(alpha,m), beta,new_red, new_black, new_cover,color^1,remain_depth-1,start);
			if(t > m){ 
				m=t;
			}
			if(m>=beta) return m; 
		}
		for(int i = 0; i < eat_count; i++){ // move
			new_red = red_chess_num, new_black = black_chess_num;
			memcpy(new_board, board, sizeof(int)*32);
			memcpy(new_cover, cover_chess, sizeof(int)*14);
			MakeMove(new_board, &new_red, &new_black, new_cover, ResultEat[i], -1); // -1: NULL
			
			double t = G_3(new_board,  std::max(alpha,m), beta,new_red, new_black, new_cover,color^1,remain_depth-1,start);
			if(t > m){ 
				m=t;
			}
			if(m>=beta) return m; 
		}

		for(int i = 0; i < flip_count; i++){ // flip
			double t = Star0_F_3(board, std::max(alpha,m),beta,red_chess_num, black_chess_num,  cover_chess, Chess, Moves[i],remain_count,remain_total,color,remain_depth,start,0);
			if(t > m){ 
				m=t;
				//*move = Moves[i];
			}
			// else if(t == m){
			// 	bool r = rand()%2;
			// 	if(r) *move = Moves[i];
			// }
		}
		return m;
	}
	return 0;
}

double MyAI::G_3(const int* board, double alpha, double beta,const int red_chess_num, const int black_chess_num,const int* cover_chess, const int color,int remain_depth,struct timespec start){
	struct timespec end;
	clock_gettime(CLOCK_REALTIME, &end);
	if(remain_depth <= 0 || ((double)((end.tv_sec+end.tv_nsec*1e-9) - (double)(start.tv_sec+start.tv_nsec*1e-9)))>TIME_LIMIT){
		return TAEvaluate(board);
	}
	else if(red_chess_num == 0 || black_chess_num == 0){
		return TAEvaluate(board);
	}
	int ResultEat[256],ResultMove[256],saveEat[256];
	int saveEatCount =0;
	int Moves[512], Chess[512];
	int flip_count = 0, remain_count = 0, remain_total = 0;
	int move_count = 0;

	// move
	int eat_count = 0;
	move_count = Expand(board,red_chess_num, black_chess_num,cover_chess,color, ResultEat,ResultMove,saveEat,saveEatCount);
	int new_board[32], new_cover[14], new_move, new_red, new_black;
	if(saveEatCount>0){
		//memcpy(Moves,saveEat,sizeof(int)*saveEatCount);	
		new_red = red_chess_num, new_black = black_chess_num;
		memcpy(new_board, board, sizeof(int)*32);
		memcpy(new_cover, cover_chess, sizeof(int)*14);
		int maxMove = saveEat[0],maxEat = board[saveEat[0]%100];
		for(int i=1;i<saveEatCount;i++){
			int eat = board[saveEat[i]%100];
			if((eat>maxEat&&maxEat!=1)||eat ==1){
				maxEat = eat;
				maxMove = saveEat[i];
			}else if(eat == maxEat){
				if(board[maxMove/100] > board[saveEat[i]/100]){
					maxMove = saveEat[i];
				}
			}
			
		}
		//*move = maxMove;
		MakeMove(new_board, &new_red, &new_black, new_cover,maxMove, -1); // -1: NULL
		return 	TAEvaluate(new_board);
	}
	eat_count = move_count/128;
	move_count = move_count%128;
	// flip
		
	if(move_count == 0){ // terminal node (no move type)
		return TAEvaluate(board);
	}
	else{
		double m = DBL_MAX;
		// search deeper
		for(int i = 0; i < move_count; i++){ // move
			new_red = red_chess_num, new_black = black_chess_num;
			memcpy(new_board, board, sizeof(int)*32);
			memcpy(new_cover, cover_chess, sizeof(int)*14);
			MakeMove(new_board, &new_red, &new_black, new_cover, ResultMove[i], -1); // -1: NULL
			
			double t = F_3(new_board,  std::max(alpha,m), beta,new_red, new_black, new_cover,color^1,remain_depth-1,start);
			if(t < m){ 
				m=t;
			}
			if(m<=alpha) return m; 
		}
		for(int i = 0; i < eat_count; i++){ // move
			new_red = red_chess_num, new_black = black_chess_num;
			memcpy(new_board, board, sizeof(int)*32);
			memcpy(new_cover, cover_chess, sizeof(int)*14);
			MakeMove(new_board, &new_red, &new_black, new_cover, ResultEat[i], -1); // -1: NULL
			
			double t = F_3(new_board,  std::max(alpha,m), beta,new_red, new_black, new_cover,color^1,remain_depth-1,start);
			if(t < m){ 
				m=t;
			}
			if(m<=alpha) return m; 
		}

		return m;
	}
	return 0;
}

double  MyAI::Star0_F_3(const int* board, double alpha, double beta,const int red_chess_num, const int black_chess_num, const int* cover_chess, int* Chess, int current_move,int remain_count, int remain_total,const int color,int remain_depth,struct timespec start,int flag){
	int new_board[32], new_cover[14], new_move, new_red, new_black;

	double total = 0;
	for(int k = 0; k < remain_count; k++){
		new_red = red_chess_num, new_black = black_chess_num;
		memcpy(new_board, board, sizeof(int)*32);
		memcpy(new_cover, cover_chess, sizeof(int)*14);
		
		MakeMove(new_board, &new_red, &new_black, new_cover, current_move, Chess[k]);
		double t=0;
		if(flag==0)
			t = G_3(new_board,  alpha, beta,new_red, new_black, new_cover,color^1,remain_depth-1,start);
		else
			t = F_3(new_board,  alpha, beta,new_red, new_black, new_cover,color^1,remain_depth-1,start);
		
		total += t*Chess[k];
	}
	//if(remain_total == 0)printf("\n-------------ERROR-----------\n");
	double E_score = (total / (double)remain_total); // calculate the expect value of flip
	return E_score;
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

///////////////////////////////////////////////////////////////////////////////////

void MyAI::TAMakeMove(int* board, int* red_chess_num, int* black_chess_num, int* cover_chess, const int move, const int chess){
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


int MyAI::TAExpand(const int* board, const int color,int *Result)
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
					if(TAReferee(board,i,rowCount,color))
					{
						Result[ResultCount] = i*100+rowCount;
						ResultCount++;
					}
				}
				for(int colCount=col; colCount<32;colCount += 4)
				{
				
					if(TAReferee(board,i,colCount,color))
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
					
					if(Move[k] >= 0 && Move[k] < 32 && TAReferee(board,i,Move[k],color))
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
//TA Code

// TAReferee
bool MyAI::TAReferee(const int* chess, const int from_location_no, const int to_location_no, const int UserId)
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

double MyAI:: Nega_max(const int* board, int* move, const int red_chess_num, const int black_chess_num, const int* cover_chess, const int color, const int depth, const int remain_depth,struct timespec start){
	
	struct timespec end;
	clock_gettime(CLOCK_REALTIME, &end);
	if(remain_depth == 0 || ((double)((end.tv_sec+end.tv_nsec*1e-9) - (double)(start.tv_sec+start.tv_nsec*1e-9)))>10){ // reach limit of depth
		this->node++;
		return TAEvaluate(board) * (2*((depth&1)^1)-1); // odd: *-1, even: *1
	}else if(red_chess_num == 0 || black_chess_num == 0){ // terminal node (no chess type)
		this->node++;
		return TAEvaluate(board) * (2*((depth&1)^1)-1);
	}

	int Result[1024],ResultEat[1024],ResultMove[1024],saveEat[1024];
	int saveEatCount=0;
	// Moves[] = {move} U {flip}, Chess[] = {remain chess}
	int Moves[2048], Chess[2048];
	int flip_count = 0, remain_count = 0, remain_total = 0;
	int move_count = 0;

	// move
	if(depth==0){
		move_count = Expand(board,red_chess_num, black_chess_num,cover_chess,color, ResultEat,ResultMove,saveEat,saveEatCount);
		if(saveEatCount>0){
			int maxMove = saveEat[0],maxEat = board[saveEat[0]%100];
			for(int i=1;i<saveEatCount;i++){
				int eat = board[saveEat[i]%100];
				if((eat>maxEat&&maxEat!=1)||eat ==1){
					maxEat = eat;
					maxMove = saveEat[i];
				}else if(eat == maxEat){
					if(board[maxMove/100] > board[saveEat[i]/100]){
						maxMove = saveEat[i];
					}
				}
				
			}
			*move = maxMove;
			return TAEvaluate(board)* (2*((depth&1)^1)-1);
		}
	}

	move_count = TAExpand(board, color, Result);
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
		return TAEvaluate(board) * (2*((depth&1)^1)-1);
	}else{
		double m = -DBL_MAX;
		int new_board[32], new_cover[14], new_move, new_red, new_black;
		// search deeper
		for(int i = 0; i < move_count; i++){ // move
			new_red = red_chess_num, new_black = black_chess_num;
			memcpy(new_board, board, sizeof(int)*32);
			memcpy(new_cover, cover_chess, sizeof(int)*14);
			
			TAMakeMove(new_board, &new_red, &new_black, new_cover, Moves[i], -1); // -1: NULL
			double t = -Nega_max(new_board, &new_move, new_red, new_black, new_cover, color^1, depth+1, remain_depth-1,start);
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
				
				TAMakeMove(new_board, &new_red, &new_black, new_cover, Moves[i], Chess[k]);
				double t = -Nega_max(new_board, &new_move, new_red, new_black, new_cover, color^1, depth+1, remain_depth-1,start);
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
double MyAI::TAEvaluate(const int* board){
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




CXX = g++ -std=c++11 -O3
F_LINUX = -D LINUX
CLEAN = rm -rf

all:
	$(CXX) main.cpp MyAI.cpp TreeNode.cpp ZobristHashTable.cpp -o CDCHash
	$(CLEAN) -f *.o
myAI:        
	$(CXX) main.cpp MyAI.cpp -o myAI	
	$(CLEAN) -f *.o
	
clean:
	$(CLEAN) myAI

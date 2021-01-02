CXX = g++ -std=c++11 -Wall -O3
F_LINUX = -D LINUX
CLEAN = rm -rf

all:
	$(CXX) main.cpp MyAI.cpp -o myAI
	$(CLEAN) -f *.o

clean:
	$(CLEAN) myAI

CXXFLAGS=-O0 -g -std=c++14
INCLUDE=-Iinclude -I/usr/local/include
LIBS=-L/usr/local/lib -lglfw3 -ldl
SRC=src/*.c src/*.cpp

default:
	clang++ main.cpp $(CXXFLAGS) $(SRC) $(INCLUDE) $(LIBS) -o bin/main
	./bin/main

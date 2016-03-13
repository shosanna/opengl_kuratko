CXXFLAGS=-O0 -g -std=c++14 -Iinclude -I/usr/local/include src/glad.c src/tgaimage.cpp -L/usr/local/lib -lglfw3 -ldl

default:
	clang++ main.cpp $(CXXFLAGS) -o bin/main
	./bin/main

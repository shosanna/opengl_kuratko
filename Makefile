APPNAME   := bin/main
SOURCES		:= $(wildcard src/*.cpp src/*.c)
OBJECTS 	:= $(patsubst src%, obj%, $(patsubst %.cpp, %.o, $(patsubst %.c, %.o, $(SOURCES))))

INCLUDE   := -I./include -I/usr/local/include
LIBPATH		:= -L/usr/local/lib
LIBS			:= -lsdl2

FLAGS			:= -O0 -g -fno-strict-aliasing
CCFLAGS 	:= $(FLAGS)
CXXFLAGS  := $(FLAGS) -std=c++14

CC        := clang
CXX       := clang++

all: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(OBJECTS) -o $(APPNAME) $(LIBPATH) $(LIBS)
	./bin/main

obj/%.o: src/%.c
	$(CC) $(CCFLAGS) $(INCLUDE) -c $< -o $@

obj/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -rf obj/*
	rm -f $(APPNAME)

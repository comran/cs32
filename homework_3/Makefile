CC = /usr/bin/clang++-3.5
RM=/bin/rm
CFLAGS  = -g -Wall -std=c++11

all: tree.a #file.a linear.a maze.a

file.a: file.cpp
	$(CC) $(CFLAGS) -o file.a file.cpp

linear.a: linear.cpp
	$(CC) $(CFLAGS) -o linear.a linear.cpp

maze.a: maze.cpp
	$(CC) $(CFLAGS) -o maze.a maze.cpp

tree.a: tree.cpp
	$(CC) $(CFLAGS) -o tree.a tree.cpp

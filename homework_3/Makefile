CC = /usr/bin/clang++-3.5
RM=/bin/rm
CFLAGS  = -g -Wall -std=c++11

all: file.a linear.a maze.a #tree.a

file.a: file.cpp
	$(CC) $(CFLAGS) -o file.a file.cpp

linear.a: linear.cpp
	$(CC) $(CFLAGS) -o linear.a linear.cpp

maze.a: maze.cpp
	$(CC) $(CFLAGS) -o maze.a maze.cpp

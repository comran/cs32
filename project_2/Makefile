CC = /usr/bin/clang++-3.5
RM=/bin/rm
CFLAGS  = -g -Wall -std=c++11

all: main.a

main.a: main.o Sequence.o
	$(CC) $(CFLAGS) -o main.a main.o Sequence.o

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp -o main.o

Sequence.o: Sequence.cpp Sequence.h
	$(CC) $(CFLAGS) -c Sequence.cpp -o Sequence.o

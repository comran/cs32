CC = /usr/bin/clang++-3.5
RM=/bin/rm
CFLAGS  = -g -Wall -std=c++11

all: main.a

main.a: main.o MapLoader.o
	$(CC) $(CFLAGS) -o main main.o MapLoader.o

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

MapLoader.o: MapLoader.cpp
	$(CC) $(CFLAGS) -c MapLoader.cpp

CC = /usr/bin/clang++-3.5
RM=/bin/rm
CFLAGS  = -g -Wall -std=c++11

all: mazestack.a mazequeue.a eval.a

mazestack.a: mazestack.o
	$(CC) $(CFLAGS) -o mazestack.a mazestack.o

mazestack.o: mazestack.cpp
	$(CC) $(CFLAGS) -c mazestack.cpp -o mazestack.o

mazequeue.a: mazequeue.o
	$(CC) $(CFLAGS) -o mazequeue.a mazequeue.o

mazequeue.o: mazequeue.cpp
	$(CC) $(CFLAGS) -c mazequeue.cpp -o mazequeue.o

eval.a: eval.o
	$(CC) $(CFLAGS) -o eval.a eval.o

eval.o: eval.cpp
	$(CC) $(CFLAGS) -c eval.cpp -o eval.o

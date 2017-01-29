CC = /usr/bin/clang++-3.5
RM=/bin/rm
CFLAGS  = -g -Wall -std=c++11

all: testSequence.a testScoreList.a testnewSequence.a

testSequence.a: Sequence.o testSequence.o
	$(CC) $(CFLAGS) -o testSequence.a Sequence.o testSequence.o

testScoreList.a: testScoreList.o ScoreList.o Sequence.o
	$(CC) $(CFLAGS) -o testScoreList.a testScoreList.o ScoreList.o Sequence.o

testnewSequence.a: testnewSequence.o newSequence.o
	$(CC) $(CFLAGS) -o testnewSequence.a testnewSequence.o newSequence.o

Sequence.o: Sequence.cpp Sequence.h
	$(CC) $(CFLAGS) -c Sequence.cpp

ScoreList.o: ScoreList.cpp ScoreList.h
	$(CC) $(CFLAGS) -c ScoreList.cpp

newSequence.o: newSequence.cpp newSequence.h
	$(CC) $(CFLAGS) -c newSequence.cpp

testSequence.o: testSequence.cpp
	$(CC) $(CFLAGS) -c testSequence.cpp

testScoreList.o: testScoreList.cpp
	$(CC) $(CFLAGS) -c testScoreList.cpp

testnewSequence.o: testnewSequence.cpp
	$(CC) $(CFLAGS) -c testnewSequence.cpp

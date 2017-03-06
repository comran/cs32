CC = /usr/bin/clang++-3.5
RM=/bin/rm
CFLAGS  = -g -Wall -std=c++11

all: Sequence_test list sorts

Sequence_test: ./main.cpp
	$(CC) $(CFLAGS) -o Sequence_test ./main.cpp

Complex_test: ./main.cpp
	$(CC) $(CFLAGS) -o Complex_test ./complex.cpp

list: ./list.cpp
	$(CC) $(CFLAGS) -o list ./list.cpp

sorts: ./sorts.cpp
	$(CC) $(CFLAGS) -o sorts ./sorts.cpp

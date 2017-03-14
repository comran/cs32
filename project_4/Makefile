CC = /usr/bin/clang++-3.6
RM=/bin/rm
CFLAGS  = -g -Wall -std=c++11

all: main.a

main.a: main.o MapLoader.o AttractionMapper.o SegmentMapper.o Navigator.o
	$(CC) $(CFLAGS) -o main main.o MapLoader.o AttractionMapper.o SegmentMapper.o Navigator.o

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

MapLoader.o: MapLoader.cpp
	$(CC) $(CFLAGS) -c MapLoader.cpp

AttractionMapper.o: AttractionMapper.cpp
	$(CC) $(CFLAGS) -c AttractionMapper.cpp

SegmentMapper.o: SegmentMapper.cpp
	$(CC) $(CFLAGS) -c SegmentMapper.cpp

Navigator.o: Navigator.cpp
	$(CC) $(CFLAGS) -c Navigator.cpp

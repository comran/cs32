CC = /usr/bin/clang++-3.6

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
HEADERS = $(wildcard *.h)

.PHONY: default all clean

all: BruinNav

%.o: %.cpp $(HEADERS)
		$(CC) -c -std=c++14 -Wno-unused-parameter -Wno-unused-variable -Wno-reorder $< -o $@

BruinNav: $(OBJECTS)
		$(CC) $(OBJECTS) -o $@

clean:
		rm -f *.o
			rm -f BruinNav

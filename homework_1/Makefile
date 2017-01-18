CC = /usr/bin/clang++-3.5
RM=/bin/rm
CFLAGS  = -g -Wall -std=c++11
TARGET = rats

all: $(TARGET)

$(TARGET): ./*.cpp
	$(CC) $(CFLAGS) -o $(TARGET) ./*.cpp

clean:
	$(RM) $(TARGET)

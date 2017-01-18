CC = /usr/bin/clang++
RM=/bin/rm
CFLAGS  = -g -Wall -std=c++11
TARGET = rats

all: $(TARGET)

$(TARGET): ./*.cpp
	$(CC) $(CFLAGS) -o $(TARGET) ./*.cpp

clean:
	$(RM) $(TARGET)

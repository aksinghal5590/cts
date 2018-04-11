CC=g++
CFLAGS=-std=c++11 -O3 -Iinclude

.PHONY: all clean

all: clean cts cts_rand

cts:
	$(CC) $(CFLAGS) src/cts.cpp src/ctsMain.cpp -o bin/cts

cts_rand:
	$(CC) $(CFLAGS) src/cts.cpp src/ctsMainRandom.cpp -o bin/cts_rand

clean:
	rm -rf bin/
	mkdir -p bin


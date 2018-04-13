CC=g++
CFLAGS=-std=c++11 -O3 -Iinclude

.PHONY: all clean

all: clean cts cts_rand cts_test clear_cache

cts:
	$(CC) $(CFLAGS) src/cts.cpp src/ctsMain.cpp -o bin/cts

cts_rand:
	$(CC) $(CFLAGS) src/cts.cpp src/ctsMainRandom.cpp -o bin/cts_rand

cts_test:
	$(CC) $(CFLAGS) src/cts.cpp src/ctsMainTest.cpp -o bin/cts_test

clear_cache:
	$(CC) $(CFLAGS) src/ClearCache.cpp -o bin/clear_cache

clean:
	rm -rf bin/
	mkdir -p bin


CC=g++
CFLAGS=-std=c++11 -O3 -Iinclude

.PHONY: all clean

all: clean cts

cts:
	mkdir -p bin
	$(CC) $(CFLAGS) src/cts.cpp src/ctsMain.cpp -o bin/cts

clean:
	rm -rf bin/



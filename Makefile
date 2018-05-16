CC=g++
CFLAGS=-std=c++11 -O3 -Iinclude

.PHONY: all clean

all: clean clear_cache cts cts_recur cts_self cts_self_recur cts_test data_gen

cts:
	$(CC) $(CFLAGS) src/cts.cpp src/ctsMain.cpp -o bin/cts

cts_self:
	$(CC) $(CFLAGS) src/cts.cpp src/ctsMainSelf.cpp -o bin/cts_self

cts_recur:
	$(CC) $(CFLAGS) src/ctsRecursion.cpp src/ctsMain.cpp -o bin/cts_recur

cts_self_recur:
	$(CC) $(CFLAGS) src/ctsRecursion.cpp src/ctsMainSelf.cpp -o bin/cts_self_recur

cts_test:
	$(CC) $(CFLAGS) src/cts.cpp src/ctsMainTest.cpp -o bin/cts_test

data_gen:
	$(CC) $(CFLAGS) src/dataGenerator.cpp -o bin/data_gen

clear_cache:
	$(CC) $(CFLAGS) src/ClearCache.cpp -o bin/clear_cache

clean:
	rm -rf bin/
	mkdir -p bin


CC=g++
CFLAGS=-std=c++11 -O3 -Iinclude

.PHONY: all clean

all: clean cts_rand cts_rand_par cts_test cts_test_par  clear_cache data_gen

cts_rand:
	$(CC) $(CFLAGS) src/cts.cpp src/ctsMainRandom.cpp -o bin/cts_rand

cts_rand_par:
	$(CC) $(CFLAGS) -fcilkplus src/ctsParallel.cpp src/ctsMainRandomParallel.cpp -o bin/cts_rand_par

cts_test:
	$(CC) $(CFLAGS) src/cts.cpp src/ctsMainTest.cpp -o bin/cts_test

cts_test_par:
	$(CC) $(CFLAGS) -fcilkplus src/ctsParallel.cpp src/ctsMainTest.cpp -o bin/cts_test_par

data_gen:
	$(CC) $(CFLAGS) src/dataGenerator.cpp -o bin/data_gen

clear_cache:
	$(CC) $(CFLAGS) src/ClearCache.cpp -o bin/clear_cache

clean:
	rm -rf bin/
	mkdir -p bin


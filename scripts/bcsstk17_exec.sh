#!/bin/sh
#SBATCH -J cts_bcsstk17    	# job name
#SBATCH -o bcsstk17.out 	# output and error file name (%j expands to jobID)
#SBATCH -N 1				# total no of nodes
#SBATCH -n 1              	# total number of mpi tasks requested per node
#SBATCH -p skx-normal	    # queue (partition) -- normal, development, etc.
#SBATCH -t 02:00:00        	# run time (hh:mm:ss) -

./bin/clear_cache
./bin/cts_recur 0.0025 32 < input/bcsstk17.mtx

./bin/clear_cache
./bin/cts_self_recur 0.0025 32 < input/bcsstk17.mtx

./bin/clear_cache
./bin/cts_recur 0.0025 64 < input/bcsstk17.mtx

./bin/clear_cache
./bin/cts_self_recur 0.0025 64 < input/bcsstk17.mtx

./bin/clear_cache
./bin/cts_recur 0.0025 128 < input/bcsstk17.mtx

./bin/clear_cache
./bin/cts_self_recur 0.0025 128 < input/bcsstk17.mtx

./bin/clear_cache
./bin/cts 0.0025 32 < input/bcsstk17.mtx

./bin/clear_cache
./bin/cts_self 0.0025 32 < input/bcsstk17.mtx

./bin/clear_cache
./bin/cts 0.0025 64 < input/bcsstk17.mtx

./bin/clear_cache
./bin/cts_self 0.0025 64 < input/bcsstk17.mtx

./bin/clear_cache
./bin/cts 0.0025 128 < input/bcsstk17.mtx

./bin/clear_cache
./bin/cts_self 0.0025 128 < input/bcsstk17.mtx
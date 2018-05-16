#!/bin/sh
#SBATCH -J cts_8192        	# job name
#SBATCH -o 8192.out 		# output and error file name (%j expands to jobID)
#SBATCH -N 1				# total no of nodes
#SBATCH -n 1              	# total number of mpi tasks requested per node
#SBATCH -p skx-normal	    # queue (partition) -- normal, development, etc.
#SBATCH -t 02:00:00        	# run time (hh:mm:ss) -

./bin/clear_cache
./bin/cts_test 0.0025 16 < input/8192_4.mtx

./bin/clear_cache
./bin/cts_test 0.0025 32 < input/8192_4.mtx

./bin/clear_cache
./bin/cts_test 0.0025 128 < input/8192_4.mtx
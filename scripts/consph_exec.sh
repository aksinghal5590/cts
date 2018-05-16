#!/bin/sh
#SBATCH -J cts_consph       # job name
#SBATCH -o consph.out 		# output and error file name (%j expands to jobID)
#SBATCH -N 1				# total no of nodes
#SBATCH -n 1              	# total number of mpi tasks requested per node
#SBATCH -p skx-normal	    # queue (partition) -- normal, development, etc.
#SBATCH -t 02:00:00        	# run time (hh:mm:ss) -

./bin/clear_cache
./bin/cts_recur 0.0009 128 < input/consph.mtx

./bin/clear_cache
./bin/cts_self_recur 0.0009 128 < input/consph.mtx

./bin/clear_cache
./bin/cts_recur 0.0009 256 < input/consph.mtx

./bin/clear_cache
./bin/cts_self_recur 0.0009 256 < input/consph.mtx

./bin/clear_cache
./bin/cts 0.0009 128 < input/consph.mtx

./bin/clear_cache
./bin/cts_self 0.0009 128 < input/consph.mtx

./bin/clear_cache
./bin/cts 0.0009 256 < input/consph.mtx

./bin/clear_cache
./bin/cts_self 0.0009 256 < input/consph.mtx
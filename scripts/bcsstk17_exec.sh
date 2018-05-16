#!/bin/sh
#SBATCH -J cts           	# job name
#SBATCH -o job1024.out 		# output and error file name (%j expands to jobID)
#SBATCH -N 1			# total no of nodes
#SBATCH -n 48              	# total number of mpi tasks requested per node
#SBATCH -p skx-dev	     	# queue (partition) -- normal, development, etc.
#SBATCH -t 00:30:00        	# run time (hh:mm:ss) -

./bin/clear_cache
./bin/cts_rand 0.0025 < input/bcsstk17.mtx

./bin/clear_cache
./bin/cts_rand 0.0001 < input/bcsstk17.mtx

./bin/clear_cache
export CILK_NWORKERS=48
./bin/cts_rand_par 0.0025 48 < input/bcsstk17.mtx

./bin/clear_cache
export CILK_NWORKERS=48
./bin/cts_rand_par 0.0001 48 < input/bcsstk17.mtx

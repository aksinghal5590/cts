#!/bin/sh
#SBATCH -J cts           	# job name
#SBATCH -o job.out 		# output and error file name (%j expands to jobID)
#SBATCH -N 1			# total no of nodes
#SBATCH -n 1              	# total number of mpi tasks requested per node
#SBATCH -p skx-dev	     	# queue (partition) -- normal, development, etc.
#SBATCH -t 01:30:00        	# run time (hh:mm:ss) -

./bin/cts_rand 4096 50
./bin/cts_rand 8192 100
./bin/cts_rand 16384 200
./bin/cts_rand 32768 400
./bin/cts_rand 65536 800
./bin/cts_rand_p 4096 50
./bin/cts_rand_p 8192 100
./bin/cts_rand_p 16384 200
./bin/cts_rand_p 32768 400
./bin/cts_rand_p 65536 800

#!/bin/sh
#SBATCH -J cts_PAPI           	# job name
#SBATCH -o job.out 		# output and error file name (%j expands to jobID)
#SBATCH -N 1			# total no of nodes
#SBATCH -n 1              	# total number of mpi tasks requested per node
#SBATCH -p skx-dev	     	# queue (partition) -- normal, development, etc.
#SBATCH -t 00:30:00        	# run time (hh:mm:ss) -

#./bin/cts_rand 1024
#./bin/cts_rand 2048
#./bin/cts_rand 4096
#./bin/cts_rand_p 1024
#./bin/cts_rand_p 2048
#./bin/cts_rand_p 4096
#./bin/cts_rand 8192
#./bin/cts_rand_p 8192
./bin/cts_rand 16384
./bin/cts_rand_p 16384
./bin/cts_rand 32768
./bin/cts_rand_p 32768

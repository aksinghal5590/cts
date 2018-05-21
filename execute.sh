#!/bin/sh
#SBATCH -J cts           	# job name
#SBATCH -o job.out 			# output and error file name (%j expands to jobID)
#SBATCH -N 1				# total no of nodes
#SBATCH -n 1              	# total number of mpi tasks requested per node
#SBATCH -p skx-normal     	# queue (partition) -- normal, development, etc.
#SBATCH -t 02:00:00        	# run time (hh:mm:ss) -

./bin/clear_cache
./bin/cts 0.0001 < input/bcsstk17.mtx

./bin/clear_cache
./bin/cts_p 0.0001 < input/bcsstk17.mtx

./bin/clear_cache
./bin/cts 0.0025 < input/bcsstk17.mtx

./bin/clear_cache
./bin/cts_p 0.0025 < input/bcsstk17.mtx


./bin/clear_cache
./bin/cts 0.0001 < input/cant.mtx

./bin/clear_cache
./bin/cts_p 0.0001 < input/cant.mtx

./bin/clear_cache
./bin/cts 0.0025 < input/cant.mtx

./bin/clear_cache
./bin/cts_p 0.0025 < input/cant.mtx


./bin/clear_cache
./bin/cts 0.0001 < input/rma10.mtx

./bin/clear_cache
./bin/cts_p 0.0001 < input/rma10.mtx

./bin/clear_cache
./bin/cts 0.0025 < input/rma10.mtx

./bin/clear_cache
./bin/cts_p 0.0025 < input/rma10.mtx


./bin/clear_cache
./bin/cts 0.0001 < input/pdb1HYS.mtx

./bin/clear_cache
./bin/cts_p 0.0001 < input/pdb1HYS.mtx

./bin/clear_cache
./bin/cts 0.0025 < input/pdb1HYS.mtx

./bin/clear_cache
./bin/cts_p 0.0025 < input/pdb1HYS.mtx


./bin/clear_cache
./bin/cts 0.0001 < input/consph.mtx

./bin/clear_cache
./bin/cts_p 0.0001 < input/consph.mtx

./bin/clear_cache
./bin/cts 0.0025 < input/consph.mtx

./bin/clear_cache
./bin/cts_p 0.0025 < input/consph.mtx
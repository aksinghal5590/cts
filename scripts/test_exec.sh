#!/bin/sh
#SBATCH -J test_cts  	         	# job name
#SBATCH -o ../output/test_job16.out 	# output and error file name (%j expands to jobID)
#SBATCH -N 1				# total no of nodes
#SBATCH -n 48              		# total number of mpi tasks requested per node
#SBATCH -p skx-normal	     		# queue (partition) -- normal, development, etc.
#SBATCH -t 02:00:00        		# run time (hh:mm:ss) -

../bin/clear_cache
export CILK_NWORKERS=48
../bin/cts_test_par < ../input/2048_2.mtx

../bin/clear_cache
../bin/cts_test < ../input/2048_2.mtx

../bin/clear_cache
export CILK_NWORKERS=48
../bin/cts_test_par < ../input/4096_4.mtx

../bin/clear_cache
../bin/cts_test < ../input/4096_4.mtx

../bin/clear_cache
export CILK_NWORKERS=48
../bin/cts_test_par < ../input/8192_4.mtx

../bin/clear_cache
../bin/cts_test < ../input/8192_4.mtx


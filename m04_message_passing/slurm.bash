#!/bin/bash

#SBATCH -n 40                   # Number of tasks
#SBATCH -c 1                    # Number of cores requested
#SBATCH -N 2                    # Number of nodes requested
#SBATCH -t 240                  # Runtime in minutes
#SBATCH -o stdout               # Standard out goes to this file
#SBATCH -e stderr               # Standard err goes to this file
#SBATCH --mail-type ALL
#SBATCH --mail-user zhu2@hawaii.edu
#SBATCH --account=ics_632
#SBATCH -p ics632.q

mpirun ./main asynchronous_pipelined_bintree_bcast -c 1000000

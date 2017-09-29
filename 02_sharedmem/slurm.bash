#!/bin/bash

#SBATCH -n 1                    # Number of tasks
#SBATCH -c 18                   # Number of cores requested
#SBATCH -N 1                    # Number of nodes requested
#SBATCH -t 240                  # Runtime in minutes
#SBATCH -o stdout               # Standard out goes to this file
#SBATCH -e stderr               # Standard err goes to this file
#SBATCH --mail-type ALL
#SBATCH --mail-user zhu2@hawaii.edu
#SBATCH --account=ics_632
#SBATCH -p ics632.q

module load lang/Python/3.5.1/python
module load GCC/7.1.0-2.28
module load intel/ics
env COMPILER=icc python3 run.py

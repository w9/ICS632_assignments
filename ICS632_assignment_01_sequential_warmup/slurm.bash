#!/bin/bash

#SBATCH -n 1                    # Number of tasks
#SBATCH -c 1                    # Number of cores requested
#SBATCH -N 1                    # Number of nodes requested
#SBATCH -t 240                  # Runtime in minutes
#SBATCH -o slurm_stdout         # Standard out goes to this file
#SBATCH -e slurm_stdout         # Standard err goes to this file
#SBATCH --mail-type ALL
#SBATCH --mail-user zhu2@hawaii.edu
#SBATCH --account=ics_632
#SBATCH -p ics632.q

python

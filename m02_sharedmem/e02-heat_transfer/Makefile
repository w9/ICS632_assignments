N = 20000
NUM_THREADS = 4
DEBUG = 1

par:
	gcc -Ofast -DDEBUG=${DEBUG} -DALGO=1 -DN=${N} -DNUM_THREADS=${NUM_THREADS} -fopenmp -o main main.c && env ./main

seq:
	gcc -Ofast -DDEBUG=${DEBUG} -DALGO=0 -DN=${N} -o main main.c && ./main

# N = 2000
# NUM_THREADS = 3
# DEBUG = 1
# NUM_ITERS = 1000
# BLOCK_SIZE = 200

N = 12000
NUM_THREADS = 10
DEBUG = 1
NUM_ITERS = 20
BLOCK_SIZE = 200

b:
	gcc -Ofast -DDEBUG=${DEBUG} -DALGO=2 -DBLOCK_SIZE=${BLOCK_SIZE} -DN=${N} -DNUM_ITERS=${NUM_ITERS} -DNUM_THREADS=${NUM_THREADS} -fopenmp -o main main.c && time ./main

par:
	gcc -Ofast -DDEBUG=${DEBUG} -DALGO=1 -DBLOCK_SIZE=${BLOCK_SIZE} -DN=${N} -DNUM_ITERS=${NUM_ITERS} -DNUM_THREADS=${NUM_THREADS} -fopenmp -o main main.c && time ./main

seq:
	gcc -Ofast -DDEBUG=${DEBUG} -DALGO=0 -DBLOCK_SIZE=${BLOCK_SIZE} -DN=${N} -DNUM_ITERS=${NUM_ITERS} -o main main.c && time ./main

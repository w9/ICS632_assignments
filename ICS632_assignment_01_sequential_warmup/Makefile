N = 18000
#COMPILER = clang-3.5
COMPILER = gcc
#COMPILER = icc

ij:
	${COMPILER} ./main.c -O3 -o main -DN=$N -DALGO=0 -mcmodel=medium && perf stat ./main

ji:
	${COMPILER} ./main.c -O3 -o main -DN=$N -DALGO=1 -mcmodel=medium && perf stat ./main

tiled:
	${COMPILER} ./main.c -O3 -o main -DN=$N -DBS=16 -DALGO=2 -mcmodel=medium && perf stat ./main

pandoc:
	pandoc -i README.md -o README.pdf

N = 22000

ij:
	clang-3.5 ./main.c -O3 -o ij -DN=$N -DALGO=0 && perf stat ./main

ji:
	clang-3.5 ./main.c -O3 -o ji -DN=$N -DALGO=1 && perf stat ./main

tiled:
	clang-3.5 ./main.c -O3 -o tiled -DN=$N -DBS=16 -DALGO=2 && perf stat ./main

main:
	gcc sequential_julia.c -o sequential_julia -lm

mpi:
	mpicc sequential_julia.c -o sequential_julia -lm && mpirun -np 10 ./sequential_julia 104

main:
	gcc sequential_julia.c -o sequential_julia -lm

mpi:
	mpicc sequential_julia.c -o sequential_julia -lm && mpirun -np 10 ./sequential_julia 104

mpi-small:
	mpicc sequential_julia.c -o sequential_julia -lm && mpirun -np 10 ./sequential_julia 10

mpi-large:
	mpicc sequential_julia.c -o sequential_julia -lm && mpirun -np 20 ./sequential_julia 12345

debug:
	mpicc sequential_julia.c -o sequential_julia -lm && mpirun -np 2 ./sequential_julia 1234

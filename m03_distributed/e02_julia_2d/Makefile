main:
	gcc main.c -o main -lm

mpi:
	mpicc main.c -o main -lm && mpirun -np 16 ./main 404

mpi-small:
	mpicc main.c -o main -lm && mpirun -np 10 ./main 10

mpi-large:
	mpicc main.c -o main -lm && mpirun -np 20 ./main 12345

debug:
	mpicc main.c -DDEBUG -o main -lm && mpirun -np 13 ./main 21

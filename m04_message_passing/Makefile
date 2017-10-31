N_CORES = 5
ALGO = "naive_bcast"
CHUNK_SIZE = 1000000

main:
	mpicc -o main main.c && mpirun -np ${N_CORES} -hostfile hostfile ./main naive_bcast

sim:
	smpicc -o main main.c && smpirun --cfg=smpi/bcast:mpich -np ${N_CORES} -platform ring_${N_CORES}.xml -hostfile hostfile_${N_CORES}.txt ./main ${ALGO} -c ${CHUNK_SIZE}

bin:
	smpicc -o main main.c && smpirun --cfg=smpi/bcast:mpich -np ${N_CORES} -platform ./bintree_${N_CORES}.xml -hostfile hostfile_${N_CORES}.txt ./main ${ALGO} -c ${CHUNK_SIZE}

pandoc:
	pandoc -o README.pdf README.md

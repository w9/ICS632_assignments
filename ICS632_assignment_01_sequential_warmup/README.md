%Assignment 1: Sequential warmup
%Xun Zhu

Question 1
==========

The source code is included in the folder under the name `main.c.` Parameters
are defined as C-preprocessor variables and algorithm (i-j, j-i, or tiled) and
block size are chosen at the compile time.

Note that in the tiled version, I used four separate for-loops for the edge
cases when N is not divisible by the block size, as opposed to having one
for-loop and branching using if-statements, which would be slower.

With $N=18,000$ and using the Intel Compiler (`icc`), the running time for i-j was
about 2 seconds. However, with the array being of type double (8 bytes = 64
bits), this means the the memory allocated for either arrays would be
$$64 \times 18,000^2 \approx 4.83 \times 2^{32} \text{bits},$$
which necessitates the `-mcmodel=medium` parameter when compiled.

An interesting observation is that `icc` (14.0.2.144 Build 20140120) did
not generate faster code than `gcc` (7.1.0). The binary created by the latter
finished in 1.99 seconds.

I wrote a Python script for invoking the compiler with various parameter
combinations, collecting the results, and aggregate the results into a CSV file.

Question 2
==========

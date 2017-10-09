%ICS632 Assignment #2 (Shared Memory)
%Xun Zhu

Exercise 1
==========

Exercise 2
==========

**Explain why using a simple parallel for pragma to parallelize the `i` or the `j`
loop will not lead to correct executions.**

Because the loop iterations have data dependency. Specifically, at any
iteration, the calculation of `A[i][j]` depends on `A[i-1][j]` and `A[i][j-1]`
(which have been updated in the same iteration,) as well as `A[i+1][j]` and
`A[i][j+1]` (which have *not* been updated in the same iteration.) If either `i`
or `j` is naively parellelized, the updating state for any of the four
surrounding cells at the time of calculating `A[i][j]` is undefined. This will
almost certainly lead to incorrect results.


**Re-organize the computation (not the data) so that you can have a
parallelizable loop. This can be done by changing the order in which elements of
the matrix are evaluated (by adding an extra loop). Implement this change in a
source file named exercise2.c and briefly explain the general idea.**

The algorithm is implemented in `./e02-heat_transfer/main.c`. The important observation
is that if we slice the matrix into strips of diagnals (from bottom-left to top-right,)
and rank them from 1 to $2N-1$ (from the top-left corner to bottom-right corner,)
at each iteration, any cell `A[i][j]` only depends on data on its previous diagnal.
This means that we could have the outer loop iterate through diagnals, and the inner
loop iterate within a diagnal. This frees the inner loop from data depenency issues
and allows for its parallelization.


**Determine and use a number of iterations that makes the original program run
in about 10 seconds. Plot the performance of your OpenMP program vs. the number
of threads (from 1 to 20 threads). On that same plot also plot the performance
of the original sequential program (as a straight line). What do you observe?
Any explanation? Should we be happy with this performance/efficiency?**


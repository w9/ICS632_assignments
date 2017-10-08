from subprocess import run, PIPE
from os import environ
from datetime import datetime
# import re
import csv
# from colors import color
from timeit import default_timer as timer

N = 1600
# N = 10
COMPILER = environ.get('COMPILER', 'gcc')
OPENMP_OPT = '-openmp' if COMPILER == 'icc' else '-fopenmp'


def shell(cmd, time=False, debug=False, stdout=True, stderr=True):
    # print(color(cmd, fg='blue'))

    if time:
        tic = timer()

    cmdp = run(cmd.split(), stdout=PIPE, stderr=PIPE)

    if time:
        toc = timer()
        print('Seconds elapsed: {}'.format(toc - tic))

    if stdout:
        print('Stdout: {}'.format(cmdp.stdout.decode('utf-8')))

    if stderr:
        print('Stderr: {}'.format(cmdp.stderr.decode('utf-8')))

    print('')

    if time:
        return (cmdp.stdout.decode('utf-8'), toc - tic)


def perf(openmp, n_threads_arr=[1], par_loop=0, log_file=None):
    if openmp:
        title = 'OpenMP - loop {} parallelized'.format(par_loop)
    else:
        title = 'Serial'

    # print(color('----------- {} -----------'.format(title), fg='yellow'))

    shell('{COMPILER} -o exercise1 -Ofast {OPENMP} -DN={N} -DPAR_LOOP={PAR_LOOP} ./exercise1.c'
          .format(COMPILER=COMPILER, OPENMP=OPENMP_OPT if openmp else '', N=N, PAR_LOOP=par_loop))

    for n_threads in n_threads_arr:
        for rep in range(10):
            print('rep: {}'.format(rep))
            result, elapsed_time = shell(
                'env OMP_NUM_THREADS={} ./exercise1'.format(n_threads), time=True)

            record = {
                'openmp': openmp,
                'n_threads': n_threads,
                'par_loop': par_loop,
                'rep': rep + 1,
                'elapsed_time': elapsed_time,
                'result': result,
            }

            wr.writerow(record)


with open('results_{}_{}.csv'.format(environ.get('HOSTNAME', 'host'),
                                     datetime.now().strftime('%y%m%d_%H%M%S')),
          'w') as f:

    wr = csv.DictWriter(f, ['openmp', 'n_threads', 'par_loop', 'rep', 'elapsed_time', 'result'])
    f.write('# HOSTNAME = {}\n'.format(environ.get('HOSTNAME', 'host')))
    wr.writeheader()

    perf(False)

    for par_loop in [0, 1, 2]:
        # for n_threads in [x + 1 for x in range(20)]:
        perf(True, [x + 1 for x in range(20)], par_loop)

"""
This script compiles different versions of the source code, runs them with `perf stat`, and gathers the
stats in a text file.
"""

from subprocess import run, PIPE
from os import system, popen, environ
import re
import csv

RE_WALL_TIME = re.compile(r'([0-9.]+) seconds time elapsed')
RE_L1_LOAD_MISSES = re.compile(r'([0-9,]+) +L1-dcache-load-misses')
RE_LLC_LOAD_MISSES = re.compile(r'([0-9,]+) +LLC-load-misses')
N = 18000
N_REP = 10
#N_REP = 1
N_SEQUENTIAL = 300
#N_SEQUENTIAL = 0
COMPILER = 'gcc'

ALGO_CODES = {
    'ij': 0,
    'ji': 1,
    'tiled': 2,
}


def compile_run_and_perf(rep, algo='tiled', bs=1):
    cmd = '{COMPILER} ./main.c -O3 -o main -DN={N} -DBS={BS} -DALGO={ALGO} -mcmodel=medium'.format(
        COMPILER=COMPILER, N=N, BS=bs, ALGO=ALGO_CODES[algo])
    print(cmd)
    cmdp = run(cmd.split(' '), stdout=PIPE, stderr=PIPE)
    print('Stdout: {}'.format(cmdp.stdout))
    print('Stderr: {}'.format(cmdp.stderr))

    cmd = 'perf stat -e L1-dcache-load-misses -e LLC-load-misses ./main'
    print(cmd)
    cmdp = run(cmd.split(' '), stdout=PIPE, stderr=PIPE)
    # print('Stdout: {}'.format(cmdp.stdout))
    # print('Stderr: {}'.format(cmdp.stderr))

    return {
        'algo':
        algo,
        'rep':
        rep,
        'bs':
        bs,
        'wall_time':
        float(RE_WALL_TIME.findall(cmdp.stderr.decode('utf-8'))[0]),
        'l1_load_misses':
        int(
            RE_L1_LOAD_MISSES.findall(cmdp.stderr.decode('utf-8'))[0].replace(
                ',', '')),
        'llc_load_misses':
        int(
            RE_LLC_LOAD_MISSES.findall(cmdp.stderr.decode('utf-8'))[0].replace(
                ',', '')),
    }


def main():
    wall_times = []

    # Warm-up runs
    for rep in range(3):
        wall_times.append(compile_run_and_perf(0, 'ij'))
        wall_times.append(compile_run_and_perf(0, 'ji'))
        wall_times.append(compile_run_and_perf(0, 'tiled', 4))

    for rep in [x + 1 for x in range(N_REP)]:
        print('============== rep = {} =============='.format(rep))

        print('---------- ij ----------')
        wall_times.append(compile_run_and_perf(rep, 'ij'))

        print('---------- ji ----------')
        wall_times.append(compile_run_and_perf(rep, 'ji'))

        bs_candidates = sorted(
            list(
                set([x + 1 for x in range(N_SEQUENTIAL)] +
                    [round(2**(x / 2)) for x in range(8, 40)])))
        for bs in bs_candidates:
            print('---------- tiled (bs = {}) ----------'.format(bs))
            wall_times.append(compile_run_and_perf(rep, 'tiled', bs))

    with open('results_{}_{}.csv'.format(environ['HOSTNAME'], COMPILER), 'w') as f:
        f.write('# HOSTNAME = {}\n'.format(environ['HOSTNAME']))
        wr = csv.DictWriter(f, wall_times[0].keys())
        wr.writeheader()
        wr.writerows(wall_times)


if __name__ == '__main__':
    main()

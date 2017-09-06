"""
This script compiles different versions of the source code, runs them with `perf stat`, and gathers the
stats in a text file.
"""

from subprocess import run, PIPE
from os import system, popen
import re
import csv

RE_WALL_TIME = re.compile(r'([0-9.]+) seconds time elapsed')
RE_L1_LOAD_MISSES = re.compile(r'([0-9,]+) +L1-dcache-load-misses')
RE_LLC_LOAD_MISSES = re.compile(r'([0-9,]+) +LLC-load-misses')

ALGO_CODES = {
    'ij': 0,
    'ji': 1,
    'tiled': 2,
}


def compile_run_and_perf(rep, algo='tiled', bs=1):
    cmd = 'clang-3.5 ./main.c -O3 -o main -DN=22000 -DBS={BS} -DALGO={ALGO} -mcmodel=medium'.format(BS=bs, ALGO=ALGO_CODES[algo])
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
        'algo': algo,
        'rep': rep,
        'bs': bs,
        'wall_time': float(RE_WALL_TIME.findall(cmdp.stderr.decode('utf-8'))[0]),
        'l1_load_misses': int(RE_L1_LOAD_MISSES.findall(cmdp.stderr.decode('utf-8'))[0].replace(',', '')),
        'llc_load_misses': int(RE_LLC_LOAD_MISSES.findall(cmdp.stderr.decode('utf-8'))[0].replace(',', '')),
    }


def main():
    wall_times = []
    for rep in [x+1 for x in range(10)]:
        print('============== rep = {} =============='.format(rep))

        print('---------- ij ----------')
        wall_times.append(compile_run_and_perf(rep, 'ij'))

        print('---------- ji ----------')
        wall_times.append(compile_run_and_perf(rep, 'ji'))

        bs_candidates = set([x+1 for x in range(300)] + [round(2**(x/2)) for x in range(8,40)])
        for bs in bs_candidates:
            print('---------- tiled (bs = {}) ----------'.format(bs))
            wall_times.append(compile_run_and_perf(rep, 'tiled', bs))

    with open('results.csv', 'w') as f:
        wr = csv.DictWriter(f, wall_times[0].keys())
        wr.writeheader()
        wr.writerows(wall_times)


if __name__ == '__main__':
    main()

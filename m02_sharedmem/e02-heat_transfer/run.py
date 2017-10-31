from subprocess import run, PIPE
from os import environ
from datetime import datetime
# import re
import csv
from colors import color
from timeit import default_timer as timer

default_params = {
    'N': 12000,
    'DEBUG': 0,
    'NUM_ITERS': 30,
    'BLOCK_SIZE': 200,
}


def build_gcc_cmd(p):
    params = default_params.copy()
    params.update(p)

    defines = ' '.join(['-D{}={}'.format(k, v) for k, v in params.items()])

    cmd = 'gcc -Ofast -fopenmp {} -o main main.c'.format(defines)

    return cmd


def gen_params():
    a = [{'ALGO': 0}]

    b = [{
        'ALGO': 1,
        'NUM_THREADS': x + 1,
    } for x in range(20)]

    c = [{
        'ALGO': 2,
        'NUM_THREADS': x + 1,
    } for x in range(20)]

    return a + b + c


def shell(cmd, stdout=True, stderr=True):
    print(color(cmd, fg='blue'))

    tic = timer()
    cmdp = run(cmd.split(), stdout=PIPE, stderr=PIPE)
    toc = timer()

    print('Seconds elapsed: {}'.format(toc - tic))

    if stdout:
        print('Stdout: {}'.format(cmdp.stdout.decode('utf-8')))
    if stderr:
        print('Stderr: {}'.format(cmdp.stderr.decode('utf-8')))

    print('')

    return {
        'OUTPUT': cmdp.stdout.decode('utf-8'),
        'RUNNING_TIME': toc - tic,
    }


with open('results_{}_{}.csv'.format(
        environ.get('HOSTNAME', 'host'),
        datetime.now().strftime('%y%m%d_%H%M%S')), 'w') as f:

    wr = csv.DictWriter(f, ['ALGO', 'NUM_THREADS', 'REP', 'RUNNING_TIME', 'OUTPUT'])

    f.write('# HOSTNAME={}\n'.format(environ.get('HOSTNAME', 'host')))

    for k, v in default_params.items():
        f.write('# {}={}\n'.format(k, v))

    wr.writeheader()

    for params in gen_params():
        for rep in range(10):
            shell(build_gcc_cmd(params))
            params.update({'REP': rep + 1})
            params.update(shell('./main'))
            wr.writerow(params)

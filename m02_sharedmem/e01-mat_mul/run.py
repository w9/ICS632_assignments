from subprocess import run, PIPE
from os import environ
from datetime import datetime
# import re
import csv
from colors import color
from timeit import default_timer as timer

default_params = {
    'N': 2400,
}


def shell(cmd):
    print(color(cmd, fg='blue'))
    tic = timer()
    cmdp = run(cmd.split(), stdout=PIPE, stderr=PIPE)
    toc = timer()
    print('Seconds elapsed: {}'.format(toc - tic))

    stdout = format(cmdp.stdout.decode('utf-8'))
    stderr = cmdp.stderr.decode('utf-8')

    print('Stdout: {}'.format(stdout))
    print('Stderr: {}'.format(stderr))
    print('')

    return {
        'RUNNING_TIME': toc - tic,
        'STDOUT': stdout,
    }


def perf(params):
    dargs = ' '.join(['-D{}={}'.format(k, v) for k, v in params.items()])
    shell('gcc -o main -Ofast -fopenmp {} ./main.c'.format(dargs))
    return shell('env ./main')


with open('results_{}_{}.csv'.format(
        environ.get('HOSTNAME', 'host'),
        datetime.now().strftime('%y%m%d_%H%M%S')), 'w') as f:

    params_list = [{
        'REP': x,
        'N_THREADS': y + 1,
        'PAR_LOOP': z,
    # } for x in range(10) for y in range(20)]
    } for x in range(10) for y in range(20) for z in range(3)]

    params = params_list[0]
    row = default_params.copy()
    row.update(params)
    row.update(perf(row))

    wr = csv.DictWriter(f, row.keys())
    wr.writeheader()
    wr.writerow(row)

    for params in params_list[1:]:
        row = default_params.copy()
        row.update(params)
        row.update(perf(row))
        wr.writerow(row)
        f.flush()

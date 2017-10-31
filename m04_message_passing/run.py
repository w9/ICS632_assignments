from subprocess import run, PIPE
from os import environ
from datetime import datetime
# import re
import csv
from colors import color
from timeit import default_timer as timer

default_params = {
    'N_CORES': 5,
    'ALGO': 'asynchronous_pipelined_bintree_bcast',
    'CHUNK_SIZE': 1000000,
    'NETWORK_STRUCTURE': 'ring',
    'OUTPUT_TIME': 1,
}


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


def extend(d, e):
    t = d.copy()
    t.update(e)
    return t


def gen_params():
    # xs = [x * 100000 for x in [1, 5, 10, 50, 100, 500, 1000]]
    xs = [1]
    ys = [50]
    # ys = [20, 35, 50]

    return [{'CHUNK_SIZE': x, 'N_CORES': y} for y in ys for x in xs]


def do(params=default_params):
    if params['NETWORK_STRUCTURE'] == 'ring':
        script = 'generate_xml_ring_and_hostfile.py'
    else:
        script = 'generate_xml_bintree_and_hostfile.py'

    cmd = 'python {} {}'.format(script, params['N_CORES'])

    shell(cmd)

    dparams = ' '.join(['-D{}={}'.format(k, v) for k, v in params.items()])

    cmd = 'smpicc -o main {} main.c'.format(dparams)

    shell(cmd)

    cmd = 'smpirun --cfg=smpi/bcast:mpich -np {0} -platform ring_{0}.xml -hostfile hostfile_{0}.txt ./main {1} -c {2}'.format(
        params['N_CORES'], params['ALGO'], params['CHUNK_SIZE'])

    return extend(params, shell(cmd))


def main():
    results = [do(x) for x in ([extend(default_params, x) for x in gen_params()])]

    with open('results.csv', 'w') as f:
        wr = csv.DictWriter(f, results[0].keys())

        f.write('# HOSTNAME={}\n'.format(environ.get('HOSTNAME', 'unknown')))

        for k, v in default_params.items():
            f.write('# {}={}\n'.format(k, v))

        wr.writeheader()

        wr.writerows(results)

# generate the xml files
# compile main.c
# run main
# run plot.R


def build_gcc_cmd(p):
    params = default_params.copy()
    params.update(p)

    defines = ' '.join(['-D{}={}'.format(k, v) for k, v in params.items()])

    cmd = 'gcc -Ofast -fopenmp {} -o main main.c'.format(defines)
    shell(cmd)

    return cmd


# with open('results_{}_{}.csv'.format(
#         environ.get('HOSTNAME', 'host'),
#         datetime.now().strftime('%y%m%d_%H%M%S')), 'w') as f:

#     wr = csv.DictWriter(f, ['ALGO', 'NUM_THREADS', 'REP', 'RUNNING_TIME', 'OUTPUT'])

#     f.write('# HOSTNAME={}\n'.format(environ.get('HOSTNAME', 'host')))

#     for k, v in default_params.items():
#         f.write('# {}={}\n'.format(k, v))

#     wr.writeheader()

#     for params in gen_params():
#         for rep in range(10):
#             shell(build_gcc_cmd(params))
#             params.update({'REP': rep + 1})
#             params.update(shell('./main'))
#             wr.writerow(params)

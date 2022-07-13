import csv
import random
import numpy as np

import config

data_sets = ['tpcds', 'swim', 'tpch']
node_num = max([n for n, k in config.nks])
node_num = 16

floor = 1
start = 5
step = 1
end = 11
ceil = 100
base = 10
each_n = 20
seps = [floor] + list(range(start, end, step)) + [ceil]

bws = []
for data_set in data_sets:
    with open(f'scripts/bw_data/{data_set}.csv', 'r') as f:
        new_bw = np.array(list(csv.reader(f)), dtype=float)
        cvs = [(i, bw.std() / bw.mean()) for i, bw in enumerate(new_bw)]
        for i in range(len(seps) - 1):
            cur_cvs = [x for x in cvs if \
                    (x[1] * base > seps[i] and x[1] * base <= seps[i+1])]
            if len(cur_cvs) > 0:
                pool = []
                while len(pool) < each_n:
                    pool.extend([new_bw[x[0]] for x in cur_cvs])
                random.shuffle(pool)
                bws.extend(pool[:each_n])
            else:
                print(f'no cv between {seps[i]/base} and {seps[i+1]/base}',
                      f'in dataset {data_set}')

with open(config.config_dir+config.bandwidth_file, 'w') as f:
    f.write(f'{len(bws)}\n{node_num}')
    for bw in bws:
        u = ' '.join([f'{int(x):>4}' for x in bw[:len(bw)//2]])
        d = ' '.join([f'{int(x):>4}' for x in bw[len(bw)//2:]])
        f.write(f'\n\n{u}\n{d}')

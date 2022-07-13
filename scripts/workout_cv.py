import numpy as np

import config

with open(config.config_dir + config.bandwidth_file, 'r') as f:
    data = [line[:-1].split(' ') for line in f if line != '\n']
bws = [[bw for bw in line if bw != ''] for line in data[2:]]
for line in bws:
    line.pop(config.rid - 1)

for n, k in config.nks:
    cur_bws = np.array([line[:n-1] for line in bws], dtype=int)
    cur_bws = cur_bws.reshape(-1, 2 * n - 2)
    with open(f'analyze/data/cvs_{n:02}.csv', 'w') as f:
        for line in cur_bws:
            f.write(f'{line.std() / line.mean()}\n')

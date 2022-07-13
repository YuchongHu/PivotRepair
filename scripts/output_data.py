import numpy as np

import config

data_dir = 'analyze/data/'

with open(config.data_file_dir + config.result_file) as f:
    data = np.array([line[:-1].split(' ') for line in f], dtype=float)
    data = np.array(data, dtype=int)

data = data.reshape(config.times, len(config.nks), len(config.algs), -1, 4)
result = np.array(data.mean(axis=0), dtype=int)

for i, nk in enumerate(config.nks):
    n, k = nk
    for j, alg in enumerate(config.algs):
        with open(f'{data_dir}{config.alg_dict[alg]}_{n:02}.csv', 'w') as f:
            for r in result[i, j, :]:
                f.write(f'{r[0]},{r[1]},{r[2]},{r[3]}\n')

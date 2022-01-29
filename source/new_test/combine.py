import csv
import numpy as np

datasets = ['tpcds', 'swim', 'tpch']
ns = [12]
algs = ['f', 'r', 'p']


repeat = 5


result_file_format = 'results{}_{}.csv'
source_file_format = '../trace_{}_{}.csv'
output_file_format = 'res/results_{}_{}.csv'

data_set_title = ['Cv']
title_formats = ['global_min_{}', 'compute_time_{}', 'repair_time_{}']

for n in ns:
    data = [[data_set_title]]
    nums = [len(data)]
    for dataset in datasets:
        with open(source_file_format.format(dataset, n), 'r') as sf:
            data[0].extend([[line[-2]] for line in csv.reader(sf) if len(line) > 1])
            nums.append(len(data[0]))

    for alg in algs:
        data.append([[title_format.format(alg) for title_format in title_formats]])
        with open(result_file_format.format(n, alg), 'r') as rf:
            result = np.array(list(csv.reader(rf)), dtype=float)
            data[-1].extend(result.reshape((-1, repeat, len(title_formats))).mean(axis=1).tolist())

    results = [[x for item in items for x in item] for items in zip(*data)]
    for i, dataset in enumerate(datasets):
        with open(output_file_format.format(dataset, n), 'w', newline='') as wf:
            writer = csv.writer(wf)
            writer.writerow(results[0])
            writer.writerows(results[nums[i] : nums[i+1]])

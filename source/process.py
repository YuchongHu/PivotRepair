import csv
import random

lfile_name = '/Users/pitwinner/Desktop/ftp_repair/source/3traces_{}_{}.csv'
sfile_name = '/Users/pitwinner/Desktop/ftp_repair/source/trace_{}_{}.csv'
ufn = 'upload'
dfn = 'download'
datasets = ['tpcds', 'swim', 'tpch']
nks = [(12, 8)]

low_limit = 1

floor = 0
start = 0.5
step = 0.1
stop = 0.99
ceil = 10
each_n = 20

def get_big_enough_datas(n, k):
	with open(lfile_name.format(ufn, n), 'r') as uf:
		udatas = [[float(x) for x in line] for line in csv.reader(uf)]
	with open(lfile_name.format(dfn, n), 'r') as df:
		ddatas = [[float(x) for x in line] for line in csv.reader(df)]
	
	results = [[] for _ in range(len(datasets))]
	per_size = len(udatas) // len(datasets)
	for i, udata in enumerate(udatas):
		if len([x for x in udata if x > low_limit]) < k:
			continue
		results[i // per_size].append(udata[:-1])
		results[i // per_size][-1].extend(ddatas[i])
		results[i // per_size][-1].append(random.randint(1, n))
	return results

def get_datas_of_cv(datas, low, ceil):
	return [data for data in datas\
			if (data[-2] >= low and data[-2] < ceil)]

def random_select(datas, n):
	if len(datas) == 0:
		return []
	elif len(datas) >= n:
		result = datas
	else:
		result = []
		for _ in range(n // len(datas)):
			result.extend(datas)
		random.shuffle(datas)
		result.extend(datas[:n - len(result)])
	random.shuffle(result)
	return result[:n]

def store_data(datas, n, k, i):
	with open(sfile_name.format(datasets[i], n), 'w', newline='') as f:
		writer = csv.writer(f)
		for data in datas:
			writer.writerow((len(data),))
			writer.writerows(data)

def load_data(dataset, n):
	with open(sfile_name.format(dataset, n), 'r') as f:
		lines = list(csv.reader(f))
		results = []

		i = 0
		while i < len(lines):
			rnum = int(lines[i][0])
			results.append([[(int(line[i]) if i == len(line)-1 else float(line[i])) for i in range(len(line))] for line in lines[i+1 : i+1+rnum]])
			i += rnum + 1

		return results


if __name__ == '__main__':
	for n, k in nks:
		for i, data in enumerate(get_big_enough_datas(n, k)):
			results = []
			s = start
			results.append(random_select(get_datas_of_cv(data, floor, s), each_n))
			while s < stop:
				results.append(random_select(get_datas_of_cv(data, s, s+step), each_n))
				s += step
			results.append(random_select(get_datas_of_cv(data, s, ceil), each_n))

			store_data(results, n, k, i)

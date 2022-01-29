import csv
import numpy as np

udf = '3traces_upload_%d.csv'
ddf = '3traces_download_%d.csv'

idn = 14
odn = 12

iud = list(csv.reader(open(udf % idn, 'r')))
idd = list(csv.reader(open(ddf % idn, 'r')))

oudf = open(udf % odn, 'w', newline='')
oddf = open(ddf % odn, 'w', newline='')
uwriter = csv.writer(oudf)
dwriter = csv.writer(oddf)

for i in range(len(iud)):
	print(iud[i][:odn], idd[i][:odn])
	oud = np.array(iud[i][:odn], dtype=float)
	odd = np.array(idd[i][:odn], dtype=float)

	minod = np.minimum(oud, odd)
	cv = minod.std() / minod.mean()

	uwriter.writerow(iud[i][:odn] + [cv])
	dwriter.writerow(idd[i][:odn] + [cv])

oudf.close()
oddf.close()

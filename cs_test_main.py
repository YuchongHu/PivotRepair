import subprocess

import source.process as sp
import tester

nodes_num = 6
each_times = 5

algs = ['r', 'f', 'p']
n = 6
k = 4
datafile = '/root/ftp_repair/source/chunk_slice.csv'

raw_size = 26
raw_psize = 15

sizes = range(23, 28)
psizes = range(11, 19)

t = tester.Tester(nodes_num)
print('making executable cpp files...')
t.make_cpp_and_send()
print('sending config files to the nodes...')
t.hand_out_config()

with open(datafile, 'r') as f:
    line = list(csv.reader(f))[0]
    bw = [int(line[i]) if i == len(line)-1 else float(line[i]) for i in range(len(line))]

    for alg in algs:
        print('\nstart testing alg: {}'.format(alg))
        for psize in psizes:
            print('\rtesting psize: {}'.format(1 << psize), end='')
            t.fail_node_unlimited_start(\
                bw[:n], bw[n:-2],\
                (\
                    (each_times, 1, (bw[-1],), (1 << raw_size, 1 << psize), alg),\
                )\
            )

        for size in sizes:
            print('\rtesting size: {}'.format(1 << size), end='')
            t.fail_node_unlimited_start(\
                bw[:n], bw[n:-2],\
                (\
                    (each_times, 1, (bw[-1],), (1 << size, 1 << raw_psize), alg),\
                )\
            )
        print('\rtesting of alg-{} finished'.format(alg))
print('closing nodes...')
t.close()
t.wait_nodes_finish()
cmd = 'mv /root/ftp_repair/files/results.txt /root/ftp_repair/source/new_test/results_cs.csv'
subprocess.call(cmd, shell=True)

# List of (n, k) that is needed to be tested
nks = [(6, 4), (8, 6), (12, 9)]

# The size of a block which is needed to be transmitted
size = 1 << 26
# The size of the slices
psize = 1 << 15

# The number of threads that are responsible for
#    receiving, computing and transmitting, respectively, for each node
recv_thr_num = 20
comp_thr_num = 10
proc_thr_num = 40

# The number of memory blocks that need to be allocated
#    (bigger than three times of the task number)
mem_num = 10
# The size of each memory (bigger than the block size)
mem_size = 1 << 26

# The path of config files
config_dir = 'config/'
address_file = 'addresses.txt'
algorithm_file = 'algorithms.txt'
bandwidth_file = 'bandwidths.txt'
config_file = 'config.txt'
config_format_file = 'config_format.txt'
task_file = 'tasks.txt'

# The path of data files
data_file_dir = 'files/'
result_file = 'results.txt'
rw_file_len = 2
rw_file_fill = 0
rfile_end = '-rfile.txt'
wfile_end = '-wfile.txt'

# True for print the constrain but not actually constrain the network
# Flase for not print anything and constrain the network speed
only_print_net_constrain = True
# The name of the net card (Check this by 'ifconfig')
eth_name = 'eth0'

# The ip addresses of each nodes (The first one is the master node)
ips = [('127.0.0.1', 10083),
       ('127.0.0.1', 10084),
       ('127.0.0.1', 10085),
       ('127.0.0.1', 10086),
       ('127.0.0.1', 10087),
       ('127.0.0.1', 10088),
       ('127.0.0.1', 10089),
       ('127.0.0.1', 10090),
       ('127.0.0.1', 10091),
       ('127.0.0.1', 10092),
       ('127.0.0.1', 10093),
       ('127.0.0.1', 10094),
       ('127.0.0.1', 10095),
       ('127.0.0.1', 10096),
       ('127.0.0.1', 10097),
       ('127.0.0.1', 10098),
       ('127.0.0.1', 10099)]

# The id of the requstor (0 is the master which will never be the requestor)
rid = 1
# The number of times that is needed to be tested
#    under each bandwidth and algorithm
times = 1

alg_dict = {'f': 'PivotRepair', 'p': 'PPT', 'r': 'RP', 'j': 'PPR'}
# The algorithms that is needed to be tested, selected from 'alg_dict'
algs = ['f', 'p']
# When the overall bandwith is less than 'min_bw', the test would be ignored
min_bw = 50

config_format = '''\
{size} {psize}

{config_dir + address_file}
{config_dir + bandwidth_file}

{recv_thr_num} {comp_thr_num} {proc_thr_num}
{mem_num} {mem_size}

{config_dir + algorithm_file}
{config_dir + task_file}
{data_file_dir + result_file}

{data_file_dir}
{rw_file_len} {rw_file_fill}
{rfile_end}
{wfile_end}

{if_only_print_net_constrain}
{eth_name}
'''

def write_address_file():
    with open(config_dir + address_file, 'w') as f:
        f.write(f'{len(ips)}\n')
        f.writelines([f'{ip} {port}\n' for ip, port in ips])

def get_alg_string(alg, n, k):
    if alg == 'j':
        return f'{alg} 4 {k} {n} {rid} {min_bw}\n'
    else:
        return f'{alg} 4 {k} {n} {rid} {min_bw}\n'

def write_algorithm_file():
    with open(config_dir + algorithm_file, 'w') as f:
        f.write(f'{len(nks) * len(algs) * times}\n')
        for _ in range(times):
            for n, k in nks:
                    for alg in algs:
                        f.write(get_alg_string(alg, n, k))

def write_config_file():
    with open(config_dir + config_file, 'w') as f:
        if_only_print_net_constrain = 1 if only_print_net_constrain else 0
        f.write(eval(f"f'''{config_format}'''"))
    with open(config_dir + config_format_file, 'w') as f:
        f.write(config_format)


if __name__ == '__main__':
    write_address_file()
    write_algorithm_file()
    write_config_file()

class ConfigCreator:
    def __init__(self):
        self.BW_CEILING = 1000
        self.node_num_fill = 0
        self.node_num_len = 2

        self.min_global_min = 10

        self.user_name = 'root'
        self.pdir = '/{}/ftp_repair/'.format(self.user_name)
        self.suffix = '.txt'

        #config files
        self.ip_file_name = 'ip'
        self.nodes_port = 12580

        self.conf_dir = 'config/'
        self.conf_file_name = 'config'
        self.addr_file_name = 'addresses'
        self.bw_file_name = 'bandwidth'
        self.task_file_name = 'test_task'
        self.wd_file_name = 'results'

        #tester
        self.tester_host = 'localhost'
        self.tester_port = 10086

        #helpers
        self.recv_num = 5
        self.cal_num = 3
        self.out_num = 3
        self.mem_num = 10
        self.mem_size = 1 << 26

        #data files
        self.rw_dir = 'files/'
        self.rf_name = 'read_file'
        self.wf_name = 'write_file'
        self.rw_num_len = 2
        self.rw_fill = 0

        self.max_read_size = 67108864

        #bin files
        self.master_bin = 'build/bin/'
        self.master_main = 'master_main'
        self.node_bin = 'bin/'
        self.node_main = 'node_main'

    def write_config_file(self):
        conf_file_format = self.pdir + self.conf_dir + '{}' + self.suffix
        f = open(conf_file_format.format(self.conf_file_name), 'w')

        conf_file_format += '\n'
        f.write(conf_file_format.format(self.addr_file_name))
        f.write(conf_file_format.format(self.bw_file_name))
        f.write(conf_file_format.format(self.task_file_name))
        f.write(self.pdir+self.rw_dir+self.wd_file_name+self.suffix+'\n')

        f.write('{} {}\n'.format(self.tester_host, self.tester_port))

        f.write('{} {} {}\n'.format(self.recv_num, self.cal_num, self.out_num))
        f.write('{} {}\n'.format(self.mem_num, self.mem_size))

        rw_file_format = self.pdir + self.rw_dir + '{}' + '\n'
        f.write(rw_file_format.format(self.rf_name))
        f.write(rw_file_format.format(self.wf_name))

        f.write(self.suffix + '\n')
        f.write('{} {}\n'.format(self.rw_num_len, self.rw_fill))

        f.write(str(self.max_read_size))

        f.write(str(self.min_global_min))

        f.close()

    def write_address_file(self, k, m):
        rf = open(self.pdir + self.ip_file_name + self.suffix, 'r')
        wf = open(self.pdir + self.conf_dir + self.addr_file_name + self.suffix, 'w')

        wf.write('{} {}\n'.format(k, m))
        for i in range(k + m + 1):
            line = rf.readline().strip()
            wf.write('{} {}\n'.format(line, self.nodes_port))

        rf.close()
        wf.close()


if __name__ == '__main__':
    cc = ConfigCreator()
    cc.write_config_file()

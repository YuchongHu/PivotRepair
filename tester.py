import subprocess
import socket
import threading

from config_creator import ConfigCreator

class Tester:
    def __init__(self, total_n):
        self.cc = ConfigCreator()
        self.sock = None
        self.conn = None
        self.total_n = total_n
        self.has_change_bw = False
        self.test_n = 0
        self.show_cmd = False

        self.master_thread = None
        self.nodes_thread = []

        self.hello_message = b'y'

    #multithread working
    def do_muti_cmd(self, n, cmd_format):
        thread_list = []
        for i in range(1, n + 1):
            cmd = cmd_format.format(i)
            thread_list.append(threading.Thread(target=self.do_cmd, args=(cmd, self.show_cmd)))
            thread_list[-1].start()
        for t in thread_list:
            t.join()

    def do_cmd(self, cmd, if_output):
        if if_output:
            subprocess.call(cmd, shell=True)
        else:
            subprocess.call(cmd, shell=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

    def get_fnum(self, fill, len):
        return '0:{}>{}d'.format(fill, len).join(('{', '}'))

    #the deployment of all the nodes
    def make_cpp_and_send(self):
        #run makefile
        self.do_cmd('cd {}; make clean; make'.format(self.cc.pdir), self.show_cmd)
        #send the executable file to all the nodes
        fcmd = 'ssh {0}@node{1} "rm -rf {2}*; mkdir {2}{3}"; scp {2}{4} {0}@node{1}:{2}{3}'
        cmd = fcmd.format(self.cc.user_name,
                          self.get_fnum(self.cc.node_num_fill, self.cc.node_num_len),
                          self.cc.pdir,
                          self.cc.node_bin,
                          self.cc.master_bin + self.cc.node_main)
        self.do_muti_cmd(self.total_n, cmd)

    def hand_out_config(self):
        self.do_cmd('rm {} -f'.format(self.cc.pdir + self.cc.conf_dir + self.cc.task_file_name + self.cc.suffix), self.show_cmd)
        self.do_cmd('rm {} -f'.format(self.cc.pdir + self.cc.conf_dir + self.cc.bw_file_name + self.cc.suffix), self.show_cmd)
        self.cc.write_config_file()
        cmd = 'ssh {0}@node{1} "rm -rf {2}; mkdir {2}"; scp {2}* {0}@node{1}:{2}'.format(self.cc.user_name,
                                                self.get_fnum(self.cc.node_num_fill, self.cc.node_num_len),
                                                self.cc.pdir + self.cc.conf_dir)
        self.do_muti_cmd(self.total_n, cmd)

    #during single test
    def start_nodes(self, k, m):
        self.test_n = k + m
        #create and send address file
        self.cc.write_address_file(k, m)
        cmd = 'scp {2} {0}@node{1}:{2}'.format(self.cc.user_name,
                                              self.get_fnum(self.cc.node_num_fill, self.cc.node_num_len),
                                              self.cc.pdir + self.cc.conf_dir + self.cc.addr_file_name + self.cc.suffix)
        self.do_muti_cmd(self.test_n, cmd)

        #start master
        cmd = self.cc.pdir + self.cc.master_bin + self.cc.master_main
        self.master_thread = threading.Thread(target=self.do_cmd, args=(cmd, self.show_cmd))
        self.master_thread.start()
        #start nodes
        cmd = 'ssh {0}@node{1} "{2} {3}"'.format(self.cc.user_name,
                                             self.get_fnum(self.cc.node_num_fill, self.cc.node_num_len),
                                             self.cc.pdir + self.cc.node_bin + self.cc.node_main, '{0}')
        for i in range(1, self.test_n + 1):
            self.nodes_thread.append(threading.Thread(target=self.do_cmd, args=(cmd.format(i), self.show_cmd)))
            self.nodes_thread[-1].start()

        #connect to the cpp master node thread
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.bind(('', self.cc.tester_port))
        self.sock.listen(1)
        self.conn, addr = self.sock.accept()
        c = self.conn.recv(1)                                                   #waiting master to compelete
        return addr

    def close(self):
        self.conn.send(b'n')

    def wait_nodes_finish(self):
        self.master_thread.join()
        self.master_thread = None
        for t in self.nodes_thread:
            t.join()
        self.nodes_thread = []
        cmd = 'ssh {0}@node{1} "wondershaper -c -a eth0"'.format(self.cc.user_name, self.get_fnum(self.cc.node_num_fill, self.cc.node_num_len))
        self.do_muti_cmd(self.total_n, cmd)
        self.conn.close()
        self.sock.close()
        self.conn = None
        self.sock = None

    def deploy_bw(self, upload, download):
        self.has_change_bw = True
        #write bw file
        bwf = open(self.cc.pdir+self.cc.conf_dir+self.cc.bw_file_name+self.cc.suffix, 'w')
        bwf.write(' '.join([str(bw) for bw in upload[:self.test_n]]) + '\n')
        bwf.write(' '.join([str(bw) for bw in download[:self.test_n]]) + '\n')
        bwf.close()
        #using wondershaper to constrain the bws
        for i in range(self.test_n):
            ul = int(float(upload[i]) * 1000)
            dl = int(float(download[i]) * 1000)
            cmd = 'ssh {0}@node{1} "wondershaper -c -a eth0; wondershaper -a eth0 -u {2} -d {3}"'.format(
                    self.cc.user_name,
                    self.get_fnum(self.cc.node_num_fill, self.cc.node_num_len).format(i + 1),
                    ul if ul > 10000 else 1000000, dl if dl > 1000 else 1000000)
            self.do_cmd(cmd, self.show_cmd)

    def start_one_test(self, tasks):
        max_size = self.save_task_file(tasks)
        cmd = 'ssh {0}@node{1} "rm -rf {2}; mkdir {2}; dd if=/dev/urandom of={2}{3} bs=1M count={4}"'.format(
                self.cc.user_name,
                self.get_fnum(self.cc.node_num_fill, self.cc.node_num_len),
                self.cc.pdir + self.cc.rw_dir,
                self.cc.rf_name + self.get_fnum(self.cc.rw_fill, self.cc.rw_num_len) + self.cc.suffix,
                max_size // 1024 // 1024 + 1)
        self.do_muti_cmd(self.test_n, cmd) 
        self.conn.send(b'y')                                                      #notify the master
        c = self.conn.recv(1)

    def save_task_file(self, tasks):
        tf = open(self.cc.pdir+self.cc.conf_dir+self.cc.task_file_name+self.cc.suffix, 'w')
        tf.write('{}\n'.format(len(tasks)))

        size = 0
        for task in tasks:
            tf.write('\n{}\n'.format(task[0]))                                    #the repeat time
            tf.write('{}\n'.format(task[1]))                                      #the number of err nodes
            tf.write(' '.join([str(node_id) for node_id in task[2]]) + '\n')      #the id of err nodes
            size = max(size, task[3][0])
            tf.write('{} {}\n'.format(task[3][0], task[3][1]))                    #the whole size of task and size per piece
            tf.write('{}\n'.format(task[4]))                                      #the algorithm
            tf.write('{}\n'.format(1 if self.has_change_bw else 0))               #if need to reload the bandwith
            self.has_change_bw = False

        tf.close()
        return size

    def fail_node_unlimited_start(self, upload, download, tasks):
        for task in tasks:
            fail_node = task[2][0]
            raw_down_load = download[fail_node - 1]
            download[fail_node - 1] = self.cc.BW_CEILING

            self.deploy_bw(upload, download)
            self.start_one_test((task,))

            download[fail_node - 1] = raw_down_load

if __name__ == '__main__':
    t = Tester(14)
    #t.make_cpp_and_send()
    #t.hand_out_config()
    for i in range(1):
        t.start_nodes(2, 2)
        for j in range(1):
            t.fail_node_unlimited_start(\
                    [1, 750, 500, 150],\
                    [2.5, 100, 130, 1000],\
                    (\
                        (1, 1, (4,), (134217728, 32768), 'f'),\
                    )\
            )
        t.close()
        t.wait_nodes_finish()

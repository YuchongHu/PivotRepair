import subprocess

import config

proj_path = '~/.exr/'
config_file = config.config_dir + config.config_file
log_file = proj_path + '/output.log'

should_send = True

def do_multi_cmd(cmd_format, ip_list, if_id):
    if if_id:
        print(cmd_format.format(f'[{len(ip_list)} IP(s)]', 0))
    else:
        print(cmd_format.format(f'[{len(ip_list)} IP(s)]'))

    for i, ip in enumerate(ip_list):
        if if_id:
            cmd = cmd_format.format(ip[0], i + 1)
        else:
            cmd = cmd_format.format(ip[0])
        subprocess.call(cmd, shell=True, stdout=subprocess.DEVNULL,
                                         stderr=subprocess.DEVNULL)
        #print(cmd)

ips = config.ips[1:]

def stop():
    #Stop threads and clear
    do_multi_cmd('ssh {} "pkill node_main"', ips, False)
    do_multi_cmd(('ssh {} ' f'"wondershaper -c -a {config.eth_name}"'), ips, False)

if __name__ == '__main__':
    #Send files
    stop()
    if should_send:
        do_multi_cmd(('ssh {} ' f'"rm -rf {proj_path}; mkdir -p {proj_path}"'),
                     ips, False)
        do_multi_cmd(('scp -r config {}:' f'{proj_path}'), ips, False)
        do_multi_cmd(('scp bin/node_main {}:' f'{proj_path}'), ips, False)
        do_multi_cmd(('ssh {} ' f'"cd {proj_path}; '
                                  'mkdir files; cd files; '
                                  'dd if=/dev/urandom of={:02}-rfile.txt '
                                  'bs=1M count=64"'), ips, True)

    #Start threads
    do_multi_cmd(('ssh {} ' f'"cd {proj_path}; '
                              'nohup ./node_main {} '
                             f'> {log_file} 2>&1 &"'), ips, True)
    subprocess.call('./bin/master_main')
    #print('./bin/master_main')
    stop()

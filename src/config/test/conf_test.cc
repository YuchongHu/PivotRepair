#include <iostream>

#include "config/config_reader.hh"

int main()
{
  exr::ConfigReader cr;
  cr.Load("src/config/test/config.txt", 3);

  std::cout << "task_size: " << cr.get_size() << std::endl
            << "piece_size: " << cr.get_psize() << std::endl
            << "address file: " << cr.get_addr_conf_path() << std::endl
            << "bandwidth file: " << cr.get_bw_conf_path() << std::endl
            << "receive thread number: " << cr.get_recv_thr_num()
                                         << std::endl
            << "compute thread number: " << cr.get_comp_thr_num()
                                         << std::endl
            << "proceed thread number: " << cr.get_proc_thr_num()
                                         << std::endl
            << "memory number: " << cr.get_mem_num() << std::endl
            << "memory size: " << cr.get_mem_size() << std::endl
            << "alg file: " << cr.get_algorithm_file() << std::endl
            << "task file: " << cr.get_task_file() << std::endl
            << "result file: " << cr.get_result_file() << std::endl
            << "data read file: " << cr.get_read_file() << std::endl
            << "data write file: " << cr.get_write_file() << std::endl
            << "if print constrain: " << cr.get_if_print() << std::endl
            << "eth name: " << cr.get_eth_name() << std::endl;
  return 0;
}

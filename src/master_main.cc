#include <iostream>
#include <memory>
#include <sys/time.h>
#include <thread>

#include "master_node.hh"
#include "config_reader.hh"
#include "address_handler.hh"

#include "task_getter.hh"

int main()
{
  //initialization
  std::cout << "This is the master node\n"
            << "loading the config file..." << std::endl;
  ConfigReader *pcr = new ConfigReader("/root/ftp_repair/config/config.txt");

  std::cout << "Getting address infomation..." << std::endl
            << "connecting to all the nodes..." << std::endl
            << "and waiting nodes to be ready" << std::endl;
  MasterNode *pmn = new MasterNode(pcr->get_aconf_addr());
  std::cout << "All nodes get ready" << std::endl;

  std::cout << "Connecting to the test controller..." << std::endl;
  TaskGetter *ptg = new TaskGetter(pcr->get_tc_addr(), pcr->get_tc_port(), pcr->get_tfile_addr(), pcr->get_wdfile_addr());
  std::cout << "Connected! Start getting test tasks!\n"
            << std::endl;

  //doing tasks
  double global_min, compute_time, repair_time;
  int i = 0;
  while (ptg->loaded_new())
  {
    //get a task
    int err_cnt = ptg->get_err_cnt();
    int *frag_err_list = ptg->get_frag_err_list();
    ssize_t size = ptg->get_request_size();
    ssize_t psize = ptg->get_piece_size();
    char build_alg = ptg->get_build_alg();
    if (ptg->need_load_bw()) {
      pmn->load_bandwidth(pcr->get_bw_addr());
    }

    //start task
    ++i;
    std::cout << "The Task " << i << " begins" << std::endl;
    struct timeval start_time, end_time;

    //build path
    std::cout << "building repair path..." << std::endl;
    gettimeofday(&start_time, nullptr);
    global_min = pmn->build_repair_path(frag_err_list[0], build_alg);
    gettimeofday(&end_time, nullptr);
    compute_time = (end_time.tv_sec - start_time.tv_sec) * 1e6 +
                   (end_time.tv_usec - start_time.tv_usec);
    std::cout << "repair path built" << std::endl;

    //repair
    if (global_min > pcr->get_min_global_min()) {
      std::cout << "start repairing" << std::endl;
      gettimeofday(&start_time, nullptr);
      pmn->start_repairing(i, frag_err_list, err_cnt, size, psize);
      gettimeofday(&end_time, nullptr);
      repair_time = (end_time.tv_sec - start_time.tv_sec) * 1e6 +
                    (end_time.tv_usec - start_time.tv_usec);
    } else {
      repair_time = 0;
    }

    ptg->store_result(global_min, compute_time, repair_time);
    //std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    std::cout << global_min << " " << compute_time << " " << repair_time << std::endl;
    std::cout << "The Task " << i << " finished\n"
              << std::endl;
  }

  //closing
  std::cout << "closing..." << std::endl;
  pmn->close();
  std::cout << "all nodes closed" << std::endl;

  //clearing
  std::cout << "shutting down..." << std::endl;
  delete pcr;
  delete pmn;
  delete ptg;
}

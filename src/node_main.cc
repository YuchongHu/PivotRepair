#include <iostream>

#include "config/address_reader.hh"
#include "config/bandwidth_solver.hh"
#include "config/config_reader.hh"
#include "repair/repairer.hh"
#include "util/typedef.hh"

using exr::Count;

using exr::ConfigReader;
using exr::AddressReader;
using exr::BandwidthSolver;
using exr::Repairer;

/* The main function of each nodes */
int main(int argc, char *argv[])
{
  //Get the node id from input
  if (argc != 2 || atoi(argv[1]) <= 0) {
    std::cerr << "bad args" << std::endl;
    exit(-1);
  }
  Count id = atoi(argv[1]);

  //Load configurations
  std::cout << "This is node " << id << std::endl
            << "Loading config files..." << std::endl;
  ConfigReader cr;
  cr.Load("config/config.txt", id);

  //Get ip addresses and ports
  AddressReader ar;
  ar.Load(cr.get_addr_conf_path());

  //Create the repairer
  std::cout << "Creating and initializing the repairer..." << std::endl;
  Repairer nr(id, ar.get_total(),
              cr.get_read_file(), cr.get_write_file(),
              cr.get_mem_num(), cr.get_mem_size(),
              cr.get_bw_conf_path(), cr.get_eth_name(),
              cr.get_if_print(), cr.get_recv_thr_num(),
              cr.get_comp_thr_num(), cr.get_proc_thr_num());

  //Connect to other nodes
  std::cout << "Connecting to the other nodes and starting to repair"
            << std::endl;
  nr.Prepare(ar.GetAddresses());


  //Wait for the tasks to be finished
  nr.WaitForFinish();
  std::cout << std::endl
            << "Received the closing signal, all tasks compeleted"
            << std::endl;
  return 0;
}

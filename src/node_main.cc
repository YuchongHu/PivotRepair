#include <iostream>
#include <thread>
#include <memory>

#include "config_reader.hh"
#include "address_handler.hh"
#include "node_repairer.hh"

int main(int argc, char *argv[])
{
  if (argc != 2 || atoi(argv[1]) <= 0) {
    std::cout << "bad args" << std::endl;
    exit(-1);
  }
  uint16_t id = atoi(argv[1]);

  std::cout << "loading the config file..." << std::endl;
  ConfigReader *cr = new ConfigReader("/root/ftp_repair/config/config.txt");

  std::cout << "getting address infomation..." << std::endl;
  AddressHandler *ah = new AddressHandler(cr->get_aconf_addr());

  std::cout << "starting the node repairer..." << std::endl;
  NodeRepairer *np = new NodeRepairer(cr->get_din_num(), cr->get_prc_num(), cr->get_out_num(),
                                      id,
                                      ah->get_k(), ah->get_m(), ah->get_node_addresses(),
                                      cr->get_mem_num(), cr->get_mem_size(),
                                      cr->get_read_addr(id), cr->get_write_addr(id),
                                      cr->get_max_load_size());

  std::cout << "connecting to other nodes..." << std::endl;
  np->start();

  std::cout << "the node " << id << " is started" << std::endl;

  //waiting the node to receive a shut down message and shut down
  while (!np->is_shut_down())
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
  std::cout << "received a shut down message\n\t--the node has already shut down"
            << std::endl;

  //clearing...
  delete cr;
  delete ah;
  delete np;

  return 0;
}

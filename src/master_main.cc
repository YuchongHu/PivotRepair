#include <fstream>
#include <iostream>
#include <sys/time.h>
#include <thread>

#include "config/address_reader.hh"
#include "config/alg_loader.hh"
#include "config/config_reader.hh"
#include "task/controller.hh"

using exr::Count;
using exr::Time;

using exr::ConfigReader;
using exr::AddressReader;
using exr::Controller;
using exr::AlgLoader;

/* The main function of the master node */
int main(int argc, char *argv[])
{
  //Load configurations
  std::cout << "This is the master node" << std::endl
            << "Loading config files..." << std::endl;
  ConfigReader cr;
  cr.Load("config/config.txt", 0);

  //Get ip addresses and ports
  AddressReader ar;
  ar.Load(cr.get_addr_conf_path());

  //Get ready for algorithms
  AlgLoader al(cr.get_task_file(), cr.get_bw_conf_path());
  al.Open(cr.get_algorithm_file());

  //Open the result file
  std::ofstream result_file(cr.get_result_file());
  if (!result_file.is_open()) {
    std::cerr << "Open result file error: " << cr.get_result_file()
              << std::endl;
    exit(-1);
  }

  //Create the controller and connect to other nodes
  std::cout << "Creating and initializing the controller..." << std::endl;
  Controller con(ar.get_total(), cr.get_size(), cr.get_psize());
  con.Connect(ar.GetAddresses());
  std::cout << "Connected" << std::endl << std::endl;

  //Run tasks
  struct timeval time_a, time_b, time_c, time_d;
  exr::BwType capacity;
  while (al.LoadNext()) {
    //Load and start a new algorithm's tasks
    con.ChangeAlg(al.GetAlg(), al.GetArgs(), al.GetPath());
    con.ReloadNodeBandwidth(ar.get_total());
    std::cout << "start testing alg " << al.GetAlg() << std::endl;
    while (true) {
      //Calculate task route
      gettimeofday(&time_a, nullptr);
      if (!con.GetTasks()) break;
      gettimeofday(&time_b, nullptr);

      //Change bandwidth
      if (al.GetAlg() != 't') con.SetNewNodeBandwidth(ar.get_total());
      capacity = con.GetCapacity();

      //Repair
      gettimeofday(&time_c, nullptr);
      auto max_task_num = con.DoTaskGroups(ar.get_total());
      gettimeofday(&time_d, nullptr);

      //Calculate times write to the result
      Time compute_time = (time_b.tv_sec - time_a.tv_sec) * 1e6 +
                          (time_b.tv_usec - time_a.tv_usec),
           repair_time = (time_d.tv_sec - time_c.tv_sec) * 1e6 +
                         (time_d.tv_usec - time_c.tv_usec);
      result_file << capacity << " "
                  << compute_time << " "
                  << repair_time << " "
                  << max_task_num << std::endl;
    }
    std::cout << "\tfinished alg " << al.GetAlg() << std::endl << std::endl;
  }

  //Finished and closing
  std::cout << "All the tasks finished, sending closing signal to the nodes"
            << std::endl;
  result_file.close();
  con.Close(ar.get_total());
  std::cout << "Closed." << std::endl;
  return 0;
}

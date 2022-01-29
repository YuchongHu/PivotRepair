#ifndef FTPR_NODE_REPAIRER_HH
#define FTPR_NODE_REPAIRER_HH

#include <thread>
#include <memory>
#include <string>
#include <unordered_map>

#include "waiting_queue.hh"
#include "tasks.hh"

#include "node_connector.hh"
#include "memory_pool.hh"
#include "calculator.hh"

class NodeRepairer
{
private:
  //threads that getting the incoming data
  uint16_t din_num;
  std::unique_ptr<std::thread[]> din_thrs;
  //threads that processing the data
  uint16_t prc_num;
  std::unique_ptr<std::thread[]> prc_thrs;
  //threads that sending data out
  uint16_t out_num;
  std::unique_ptr<std::thread[]> out_thrs;
  //the master thread
  std::thread mthr;

  //other info
  ssize_t max_load_size;
  uint16_t id;
	uint16_t n;
  bool shut_down_flag;

  //for thread control
  WaitingQueue<itask> itask_q;
  std::unique_ptr<ssize_t[]> input_remains;
  std::mutex im_mtx;

  WaitingQueue<data_piece> ptask_q;

  std::unordered_map<uint16_t, uint16_t> task_tid_map;
  std::unordered_map<uint16_t, ssize_t> task_remains;
  std::mutex tm_mtx;

  WaitingQueue<otask> otask_q;
  std::unique_ptr<ssize_t[]> output_remains;
  std::mutex om_mtx;

  std::unordered_map<uint16_t, WaitingQueue<data_piece>*> output_wq_map;
  std::mutex omap_mtx;

  //inter-node connection
  NodeConnector *pnc;
  std::mutex master_send_mtx;

  //caculator
  Calculator *pc;

  //the functions of the threads
  void get_data();
  void process_data();
  void send_data();
  void get_tasks();
  void shut_down();

public:
  NodeRepairer(uint16_t _din_num, uint16_t _prc_num, uint16_t _out_num,
               uint16_t id,
               uint16_t k, uint16_t m, std::unique_ptr<node_address[]> node_addresses,
               uint16_t block_num, ssize_t block_size,
               const std::string &load_ad, const std::string &store_ad,
               ssize_t _max_load_size);
  ~NodeRepairer();

  bool is_shut_down();
  void start();
};

#endif

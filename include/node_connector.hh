#ifndef FTPR_NODE_CONNECTOR_HH
#define FTPR_NODE_CONNECTOR_HH

#include "sockpp/tcp_acceptor.h"

#include <memory>
#include <string>
#include <queue>
#include <unordered_map>
#include <mutex>

#include "tasks.hh"
#include "address_handler.hh"
#include "data_solver_interface.hh"
#include "memory_pool.hh"

class NodeConnector
{
private:
  sockpp::tcp_acceptor acc;
  uint16_t id;
  uint16_t node_num;
  std::unique_ptr<DataSolverInterface *[]> nodes;

  std::queue<load_task> ltasks;
  std::mutex lmtx;

  std::unordered_map<uint16_t, store_task> stasks;
  std::mutex smtx;

	MemoryPool* pmp;

	std::unordered_map<uint16_t, std::unique_ptr<char*[]>> mem_map;
	std::unordered_map<uint16_t, uint16_t> mem_num_map;
	std::unordered_map<uint16_t, ssize_t> ps_map;
	std::mutex mem_mtx;

public:
  NodeConnector(uint16_t _id,
                uint16_t _node_num,
                std::unique_ptr<node_address[]> node_addresses,
                const std::string &load_ad,
                const std::string &store_ad,
								uint16_t block_num, ssize_t block_size);
  ~NodeConnector();

	void prepare(uint16_t task_id, uint16_t sid_num, bool is_fail_node, ssize_t size, ssize_t psize);
	void release_task(uint16_t task_id);

  void read(uint16_t sid, void* buf, ssize_t size);
  data_piece read(uint16_t sid, uint16_t index);
  void write(uint16_t tid, void* buf, ssize_t size);
  void write(uint16_t tid, data_piece dp);
};

#endif

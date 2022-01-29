#include "node_connector.hh"

#include <iostream>
#include <thread>
#include <memory>

#include "connection_solver.hh"
#include "socket_solver.hh"
#include "local_file_solver.hh"

//initialization: connect to all ports
NodeConnector::NodeConnector(uint16_t _id,
                             uint16_t _node_num,
                             std::unique_ptr<node_address[]> node_addresses,
                             const std::string &load_ad,
                             const std::string &store_ad,
														 uint16_t block_num, ssize_t block_size)
    : id{_id}, node_num{_node_num}
{
  //start listening
  acc = sockpp::tcp_acceptor(node_addresses[id].port);
  if (!acc) {
    std::cerr << acc.last_error_str()
              << std::endl;
    exit(-1);
  }

  //initialize the solver list
  nodes = std::unique_ptr<DataSolverInterface *[]>(new DataSolverInterface *[node_num]);

  //receive connection from nodes whose id is smaller
  std::thread receive_thread = std::thread([&] {
    uint16_t client_id;
    for (uint16_t i = 0; i < id; ++i)
    {
      sockpp::tcp_socket sock = acc.accept();
      sock.read_n(&client_id, sizeof(client_id));
      nodes[client_id] = new SocketSolver(std::move(sock));
    }
  });

  //connect to the nodes whose id is bigger
  for (uint16_t i = id + 1; i < node_num; ++i)
  {
    nodes[i] = new ConnectionSolver(node_addresses[i].host, node_addresses[i].port);
    nodes[i]->write(&id, sizeof(id));
  }
  //wait for receiving
  receive_thread.join();

  //local file solver
  nodes[id] = new LocalFileSolver(load_ad, store_ad);

	//memory
	pmp = new MemoryPool(block_num, block_size);
}

NodeConnector::~NodeConnector()
{
  for (uint16_t i = 0; i < node_num; ++i)
  {
    delete[] nodes[i];
  }
  acc.close();
	delete pmp;
}

void NodeConnector::prepare(uint16_t task_id, uint16_t sid_num, bool is_fail_node,
                            ssize_t size, ssize_t psize)
{
	std::unique_lock<std::mutex> mem_lck(mem_mtx);
	mem_num_map[task_id] = is_fail_node ? sid_num : sid_num+1;
	mem_map[task_id] = std::unique_ptr<char*[]>(new char*[mem_num_map.at(task_id)]());
	for (uint16_t i = 0; i < mem_num_map.at(task_id); ++i) {
		mem_map.at(task_id)[i] = pmp->get();
	}
	ps_map[task_id] = psize;
	mem_lck.unlock();

	if (is_fail_node) {
		store_task task(size, psize);
		task.buf = pmp->get();
		std::unique_lock<std::mutex> lck(smtx);
		stasks[task_id] = task;
		lck.unlock();
	} else {
		load_task task(task_id, size, psize);
		task.buf = nullptr;
		std::unique_lock<std::mutex> lck(lmtx);
		ltasks.push(task);
		lck.unlock();
	}
}

void NodeConnector::release_task(uint16_t task_id)
{
	std::unique_lock<std::mutex> mem_lck(mem_mtx);
	for (uint16_t i = 0; i < mem_num_map.at(task_id); ++i) {
		pmp->release(mem_map.at(task_id)[i]);
	}
	mem_map.erase(task_id);
	mem_num_map.erase(task_id);
	ps_map.erase(task_id);
	mem_lck.unlock();
}

//read and write
void NodeConnector::read(uint16_t sid, void* buf, ssize_t size)
{
  nodes[sid]->read(buf, size);
}

data_piece NodeConnector::read(uint16_t sid, uint16_t index)
{
  data_piece dp;
  if (sid != id) {
    nodes[sid]->read(&(dp.task_id), sizeof(dp.task_id));
    nodes[sid]->read(&(dp.pid), sizeof(dp.pid));
    nodes[sid]->read(&(dp.size), sizeof(dp.size));

		std::unique_lock<std::mutex> mem_lck(mem_mtx);
		dp.buf = (mem_map.at(dp.task_id))[index] + dp.pid * ps_map.at(dp.task_id);
    nodes[sid]->read(dp.buf, dp.size);
  } else {
    std::unique_lock<std::mutex> lck(lmtx);
    load_task cur_task = ltasks.front();
    lck.unlock();
    dp.task_id = cur_task.task_id;
    dp.pid = cur_task.cur_pid;
    dp.size = cur_task.remain < cur_task.psize ? cur_task.remain : cur_task.psize;
		if (cur_task.buf == nullptr) {
			std::unique_lock<std::mutex> mem_lck(mem_mtx);
			dp.buf = (mem_map.at(dp.task_id))[index];
			nodes[id]->read(dp.buf, cur_task.remain);
			ltasks.front().buf = dp.buf;
		}
    dp.buf = ltasks.front().buf + cur_task.cur_pid * cur_task.psize;

    lck.lock();
    if (cur_task.remain == dp.size) {
      ltasks.pop();
    } else {
      ltasks.front().remain -= dp.size;
      ltasks.front().cur_pid++;
    }
    lck.unlock();
  }
  return dp;
}

void NodeConnector::write(uint16_t tid, void* buf, ssize_t size) {
  nodes[tid]->write(buf, size);
}

void NodeConnector::write(uint16_t tid, data_piece dp)
{
  if (tid != id) {
    nodes[tid]->write(&(dp.task_id), sizeof(dp.task_id));
    nodes[tid]->write(&(dp.pid), sizeof(dp.pid));
    nodes[tid]->write(&(dp.size), sizeof(dp.size));
    nodes[tid]->write(dp.buf, dp.size);
  } else {
    std::unique_lock<std::mutex> lck(smtx);
    store_task task = stasks.at(dp.task_id);
    lck.unlock();

    std::memcpy(task.buf + dp.pid * task.psize, dp.buf, dp.size);

    lck.lock();
    if (task.remain == dp.size) {
      store_task task = stasks.at(dp.task_id);
      nodes[id]->write(task.buf, task.size);
      stasks.erase(dp.task_id);
      pmp->release(task.buf);
    } else {
      stasks.at(dp.task_id).remain -= dp.size;
    }
    lck.unlock();
  }
  return;
}

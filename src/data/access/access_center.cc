#include "data/access/access_center.hh"

#include <iostream>
#include <thread>

#include "data/access/connection_solver.hh"
#include "data/access/socket_solver.hh"

namespace exr {

//Constructor and destructor
AccessCenter::AccessCenter(const Count &id, const Count &total)
    : id_(id), total_(total), tis(new pTI[total]) {}

AccessCenter::~AccessCenter() { if (acc_) acc_.close(); }

//Connect to others
void AccessCenter::Connect(const IPAddressList &ip_addresses) {
  //Start listening and receive connection from those whose ids are bigger
  std::thread receive_thread;
  if (id_ != total_ - 1) {
    acc_ = sockpp::tcp_acceptor(ip_addresses[id_].port);
    if (!acc_) {
      std::cerr << acc_.last_error_str() << std::endl;
      exit(-1);
    }
    receive_thread = std::thread([&] {
      Count client_id;
      for (Count i = id_ + 1; i < total_; ++i) {
        pTI ds(new SocketSolver(acc_.accept()));
        ds->Receive(sizeof(client_id), &client_id);
        tis[client_id] = std::move(ds);
      }
    });
  }

  //Connect to those whose ids are smaller
  for (Count i = 0; i < id_; ++i) {
    tis[i] = pTI(new ConnectionSolver(ip_addresses[i]));
    tis[i]->Send(sizeof(id_), &id_);
  }

  //Wait for receiving
  if (id_ != total_ - 1) receive_thread.join();
}

//Send and Receive
void AccessCenter::Send(const Count &tar_id,
                        const DataSize &size, void *buf) {
  if (tar_id == id_) {
    std::cerr << "Cannot send data to local!!!" << std::endl;
    exit(-1);
  }
  tis[tar_id]->Send(size, buf);
}

void AccessCenter::Receive(const Count &src_id,
                           const DataSize &size, void *buf) {
  if (src_id == id_) {
    std::cerr << "Cannot receive from local!!!" << std::endl;
    exit(-1);
  }
  tis[src_id]->Receive(size, buf);
}

} // namespace exr

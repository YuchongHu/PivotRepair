#include "data/access/connection_solver.hh"

#include <thread>

namespace exr {

ConnectionSolver::ConnectionSolver(const IPAddress &ip_ad) {
  //Connect to the server
  while (!conn_.connect(sockpp::inet_address(ip_ad.host, ip_ad.port)))
    std::this_thread::yield();
}

ConnectionSolver::~ConnectionSolver() { conn_.close(); }

//Send messages to another host
void ConnectionSolver::Send(const DataSize &size, void *buf) {
  conn_.write_n(buf, size);
}

//Receive messages from another host
void ConnectionSolver::Receive(const DataSize &size, void *buf) {
  conn_.read_n(buf, size);
}

} // namespace exr

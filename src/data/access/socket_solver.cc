#include "data/access/socket_solver.hh"

#include <utility>

namespace exr {

SocketSolver::SocketSolver(sockpp::tcp_socket sock)
    : sock_(std::move(sock)) {}

SocketSolver::~SocketSolver() = default;

//Send messages to another host
void SocketSolver::Send(const DataSize &size, void *buf) {
  sock_.write_n(buf, size);
}

//Receive messages from another host
void SocketSolver::Receive(const DataSize &size, void *buf) {
  sock_.read_n(buf, size);
}

} // namespace exr

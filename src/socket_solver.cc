#include "socket_solver.hh"

#include <utility>

SocketSolver::SocketSolver(sockpp::tcp_socket _sock) : sock{std::move(_sock)} {}

SocketSolver::~SocketSolver() = default;

//DataSolverInterface
void SocketSolver::read(void *buf, ssize_t size)
{
  sock.read_n(buf, size);
}

void SocketSolver::write(void *buf, ssize_t size)
{
  sock.write_n(buf, size);
}

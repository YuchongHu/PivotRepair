#include "connection_solver.hh"

#include <thread>

ConnectionSolver::ConnectionSolver(const std::string &host, int16_t port)
{
  //connect to the server
  while (!conn.connect(sockpp::inet_address(host, port)))
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

ConnectionSolver::~ConnectionSolver()
{
  conn.close();
}

//DataSolverInterface
void ConnectionSolver::read(void *buf, ssize_t size)
{
  conn.read_n(buf, size);
}

void ConnectionSolver::write(void *buf, ssize_t size)
{
  conn.write_n(buf, size);
}
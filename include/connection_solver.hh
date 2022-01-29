#ifndef FTRP_CONNECTION_SOLVER_HH
#define FTRP_CONNECTION_SOLVER_HH

#include <string>

#include "sockpp/tcp_connector.h"

#include "data_solver_interface.hh"

class ConnectionSolver : public DataSolverInterface
{
private:
  sockpp::tcp_connector conn;

public:
  ConnectionSolver(const std::string &host, int16_t port);
  ~ConnectionSolver();

  //implement the DataSolverInterface, to receive/send message to another host
  void read(void *buf, ssize_t size);
  void write(void *buf, ssize_t size);
};

#endif
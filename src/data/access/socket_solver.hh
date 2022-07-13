#ifndef EXR_DATA_ACCESS_SOCKETSOLVER_HH_
#define EXR_DATA_ACCESS_SOCKETSOLVER_HH_

#include "sockpp/tcp_acceptor.h"

#include "data/access/transmit_interface.hh"
#include "util/typedef.hh"

namespace exr {

/* Solve with a incoming connection and send/recieve data */
class SocketSolver : public TransmitInterface
{
 public:
  SocketSolver(sockpp::tcp_socket sock);
  ~SocketSolver();

  //Implement TransmitInterface: to receive/send messages
  void Send(const DataSize &size, void *buf) override;
  void Receive(const DataSize &size, void *buf) override;

  //SocketSolver is neither copyable nor movable
  SocketSolver(const SocketSolver&) = delete;
  SocketSolver& operator=(const SocketSolver&) = delete;

 private:
  //The saved socket connection
  sockpp::tcp_socket sock_;
};

} // namespace exr

#endif // EXR_DATA_ACCESS_SOCKETSOLVER_HH_

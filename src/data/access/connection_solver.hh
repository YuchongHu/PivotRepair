#ifndef EXR_DATA_ACCESS_CONNECTIONSOLVER_HH_
#define EXR_DATA_ACCESS_CONNECTIONSOLVER_HH_

#include "sockpp/tcp_connector.h"

#include "data/access/transmit_interface.hh"
#include "util/typedef.hh"

namespace exr {

/* Connected to a listening port and send/receive data */
class ConnectionSolver : public TransmitInterface
{
 public:
  ConnectionSolver(const exr::IPAddress &ip_ad);
  ~ConnectionSolver();

  //Implement TransmitInterface: to receive/send messages
  void Send(const exr::DataSize &size, void *buf) override;
  void Receive(const exr::DataSize &size, void *buf) override;

  //ConnectionSolver is neither copyable nor movable
  ConnectionSolver(const ConnectionSolver&) = delete;
  ConnectionSolver& operator=(const ConnectionSolver&) = delete;

 private:
  //The saved connection
  sockpp::tcp_connector conn_;
};

} // namespace exr

#endif // EXR_DATA_ACCESS_CONNECTIONSOLVER_HH_

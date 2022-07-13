#ifndef EXR_DATA_ACCESS_ACCESSCENTER_HH_
#define EXR_DATA_ACCESS_ACCESSCENTER_HH_

#include <memory>

#include "sockpp/tcp_acceptor.h"

#include "data/access/transmit_interface.hh"
#include "util/typedef.hh"

namespace exr {

/* A controller that can send and receive data with other controllers */
class AccessCenter
{
 public:
  AccessCenter(const Count &id, const Count &total);
  ~AccessCenter();

  //Connect to others
  void Connect(const IPAddressList &ip_addresses);

  //Send and Receive
  void Send(const Count &tar_id, const DataSize &size, void *buf);
  void Receive(const Count &src_id, const DataSize &size, void *buf);

  //AccessCenter is neither copyable nor movable
  AccessCenter(const AccessCenter&) = delete;
  AccessCenter& operator=(const AccessCenter&) = delete;

 private:
  Count id_; //this ConnectionCenter's id
  Count total_; //total number of candidates
  sockpp::tcp_acceptor acc_; //socket acceptor

  //Sockets and Connections
  using pTI = std::unique_ptr<TransmitInterface>;
  using TIList = std::unique_ptr<pTI[]>;
  TIList tis;
};

} // namespace exr

#endif // EXR_DATA_ACCESS_ACCESSCENTER_HH_

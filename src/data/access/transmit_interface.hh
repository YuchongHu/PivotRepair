#ifndef EXR_DATA_ACCESS_TRANSMITINTERFACE_HH_
#define EXR_DATA_ACCESS_TRANSMITINTERFACE_HH_

#include "util/typedef.hh"

namespace exr {

/* A interface which allows read and write data with specific size */
class TransmitInterface
{
 public:
  //Interfaces
  //Send data to another solver
  virtual void Send(const DataSize &size, void *buf) = 0;

  //To receive data from others
  virtual void Receive(const DataSize &size, void *buf) = 0;

  //Virtual Destructor
  virtual ~TransmitInterface() {}
};

} // namespace exr

#endif // EXR_DATA_ACCESS_TRANSMITINTERFACE_HH_

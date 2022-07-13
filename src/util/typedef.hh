#ifndef EXR_UTIL_TYPEDEF_HH_
#define EXR_UTIL_TYPEDEF_HH_

#include <memory>
#include <string>

namespace exr {

/* This file defines several types used by project EXR */

//General
using Count = uint16_t;
using Path = std::string;
using Name = std::string;
using Time = double;
using Alg = char;

//Memory
using DataSize = ssize_t;
using BufUnit = char;
using RSUnit = unsigned char;

//Socket
using IP = std::string;
using Port = uint16_t;
struct IPAddress {
  IP host;
  Port port;
};
using IPAddressList = std::unique_ptr<IPAddress[]>;

//Bandwidth
using BwType = uint32_t;
struct Bandwidth {
  BwType upload;
  BwType download;
};
using TTime = ssize_t;

} // namespace exr

#endif // EXR_UTIL_TYPEDEF_HH_

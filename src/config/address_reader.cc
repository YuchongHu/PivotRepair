#include "config/address_reader.hh"

#include <fstream>
#include <iostream>
#include <memory>

namespace exr {

AddressReader::AddressReader() = default;
AddressReader::~AddressReader() = default;

void AddressReader::Load(const Path &path) {
  path_ = path;
  Load();
}

IPAddressList AddressReader::GetAddresses() {
  if (!ip_addresses_)
    Load();
  return std::move(ip_addresses_);
}

Count AddressReader::get_total() { return total_; }

void AddressReader::Load() {
  std::ifstream addr_file(path_);
  if (!addr_file.is_open()) {
    std::cerr << "Read address file error: " << path_ << std::endl;
    exit(-1);
  }

  addr_file >> total_;
  ip_addresses_ = std::make_unique<IPAddress[]>(total_);
  for (Count i = 0; i < total_; ++i)
    addr_file >> ip_addresses_[i].host >> ip_addresses_[i].port;

  addr_file.close();
}

} // namespace exr

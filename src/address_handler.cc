#include "address_handler.hh"

#include <iostream>
#include <fstream>

AddressHandler::AddressHandler(const std::string &_aconf_addr)
    : aconf_addr{_aconf_addr}
{
  load_aconf();
}

AddressHandler::~AddressHandler() = default;

void AddressHandler::load_aconf()
{
  std::ifstream cf(aconf_addr);
  if (!cf.is_open()) {
    std::cerr << "no address file" << std::endl;
    exit(-1);
  }

  cf >> k >> m;
  node_addresses = std::unique_ptr<node_address[]>(new node_address[k + m + 1]);

  for (uint16_t i = 0; i < k + m + 1; ++i)
  {
    cf >> node_addresses[i].host >> node_addresses[i].port;
  }

  cf.close();
}

std::unique_ptr<node_address[]> AddressHandler::get_node_addresses()
{
  if (node_addresses == nullptr) {
    load_aconf();
  }
  std::unique_ptr<node_address[]> naddrs = std::move(node_addresses);
  node_addresses = nullptr;
  return naddrs;
}

uint16_t AddressHandler::get_k()
{
  return k;
}

uint16_t AddressHandler::get_m()
{
  return m;
}
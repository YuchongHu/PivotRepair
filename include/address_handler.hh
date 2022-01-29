#ifndef FTPR_ADDRESS_HANDLER_HH
#define FTPR_ADDRESS_HANDLER_HH

#include <string>
#include <memory>

struct node_address
{
  std::string host;
  int16_t port;

  node_address(const std::string &_host, uint16_t _port)
      : host(_host), port(_port){};
  node_address() = default;
};

class AddressHandler
{
private:
  uint16_t k, m;
  std::unique_ptr<node_address[]> node_addresses;

  std::string aconf_addr;
  void load_aconf();

public:
  AddressHandler(const std::string &_aconf_addr);
  ~AddressHandler();

  std::unique_ptr<node_address[]> get_node_addresses();
  uint16_t get_k();
  uint16_t get_m();
};

#endif
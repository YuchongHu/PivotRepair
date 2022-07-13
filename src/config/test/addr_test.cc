#include <iostream>

#include "config/address_reader.hh"

int main()
{
  exr::AddressReader ar;
  ar.Load("src/config/test/addresses.txt");

  auto ip_addresses = ar.GetAddresses();
  for (int i = 0; i < ar.get_total(); ++i)
    std::cout << ip_addresses[i].host << " " << ip_addresses[i].port
              << std::endl;
  return 0;
}

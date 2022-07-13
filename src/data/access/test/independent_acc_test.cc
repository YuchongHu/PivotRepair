#include <iostream>

#include "data/access/access_center.hh"
#include "util/typedef.hh"

int main(int argc, char *argv[])
{
  exr::Count total = 7;

  //Get id (0 to total-1)
  if (argc != 2 || atoi(argv[1]) < 0 || atoi(argv[1]) >= total) {
    std::cout << "bad args" << std::endl;
    return 0;
  }
  exr::Count id = atoi(argv[1]);

  //Args
  auto ip_ads = exr::IPAddressList(new exr::IPAddress[total]{
      {"localhost", 10083},
      {"localhost", 10084},
      {"localhost", 10085},
      {"localhost", 10086},
      {"localhost", 10087},
      {"localhost", 10088},
      {"localhost", 10089},
  });

  //Connecting...
  exr::AccessCenter ac(id, total);
  std::cout << "Connecting..." << std::endl;
  ac.Connect(ip_ads);
  std::cout << "Connected!" << std::endl;

  return 0;
}

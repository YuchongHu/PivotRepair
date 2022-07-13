#include <array>
#include <iostream>
#include <mutex>
#include <thread>

#include "data/access/access_center.hh"
#include "util/typedef.hh"

int main()
{
  //Args
  const exr::Count total = 7;
  auto ip_ads = exr::IPAddressList(new exr::IPAddress[total]{
      {"localhost", 10083},
      {"localhost", 10084},
      {"localhost", 10085},
      {"localhost", 10086},
      {"localhost", 10087},
      {"localhost", 10088},
      {"localhost", 10089}
  });

  //Connecting...
  std::thread t[total];
  std::array<exr::AccessCenter, total> ac = { {{0, total}, {1, total},
                                               {2, total}, {3, total},
                                               {4, total}, {5, total},
                                               {6, total}} };
  std::mutex mtx;
  for (int i = 0; i < total; ++i) {
    t[i] = std::thread([&, i] {
      ac[i].Connect(ip_ads);
      std::unique_lock<std::mutex> lck(mtx);
      std::cout << "node " << i << " connected" << std::endl;
      lck.unlock();
    });
  }
  for (int i = 0; i < total; ++i)
    t[i].join();
  std::cout << "Connected!" << std::endl;

  //Test
  int m = 6;
  ac[0].Send(2, sizeof(m), &m);
  std::cout << "AC0 has sent " << m << " to AC2" << std::endl;
  ac[2].Receive(0, sizeof(m), &m);
  std::cout << "AC2 has received " << m << " from AC0" << std::endl;

  char s = 'B';
  ac[1].Send(0, sizeof(s), &s);
  std::cout << "AC1 has sent '" << s << "' to AC0" << std::endl;
  int k = 9;
  ac[2].Send(1, sizeof(k), &k);
  std::cout << "AC2 has sent " << k << " to AC1" << std::endl;

  ac[1].Receive(2, sizeof(k), &k);
  std::cout << "AC1 has received " << k << " from AC2" << std::endl;
  ac[0].Receive(1, sizeof(s), &s);
  std::cout << "AC0 has received '" << s << "' from AC1" << std::endl;

  return 0;
}

#include <iostream>
#include <memory>
#include <thread>

#include "sockpp/tcp_acceptor.h"

#include "data/access/connection_solver.hh"
#include "data/access/socket_solver.hh"
#include "data/access/transmit_interface.hh"
#include "util/typedef.hh"

int main()
{
  exr::IPAddress ip_ad{"localhost", 10086};

  //Connect
  sockpp::tcp_acceptor acc = sockpp::tcp_acceptor(ip_ad.port);
  if (!acc) {
    std::cerr << acc.last_error_str()
              << std::endl;
    exit(-1);
  }

  using pTI = std::unique_ptr<exr::TransmitInterface>;
  pTI n1;
  std::thread acc_thread = std::thread([&] {
    sockpp::tcp_socket sock = acc.accept();
    n1 = pTI(new exr::SocketSolver(std::move(sock)));
  });

  pTI n2(new exr::ConnectionSolver(ip_ad));

  acc_thread.join();

  //Send & receive test
  int a = 123, b;
  n1->Send(sizeof(a), &a);
  n2->Receive(sizeof(b), &b);
  std::cout << "n1 sent: " << a
            << " and n2 read: " << b
            << std::endl;

  ++b;
  n2->Send(sizeof(b), &b);
  n1->Receive(sizeof(a), &a);
  std::cout << "n2 sent: " << b
            << " and n1 read: " << a
            << std::endl;

  char buf1[100] = "helloadfasdf";
  char buf2[100] = "";
  exr::DataSize size = 20;
  n1->Send(size, buf1);
  n2->Receive(size, buf2);
  std::cout << "n1 sent: " << buf1
            << " and n2 read: " << buf2
            << std::endl;

  acc.close();
  return 0;
}

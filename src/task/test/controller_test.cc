#include <array>
#include <iostream>
#include <mutex>
#include <thread>

#include "data/access/access_center.hh"
#include "task/controller.hh"
#include "util/typedef.hh"
#include "util/types.hh"

int main()
{
  const exr::Count total = 4;
  const exr::DataSize size = 67108864, psize = 32768;
  auto ip_ads = exr::IPAddressList(new exr::IPAddress[total]{
      {"localhost", 10086},
      {"localhost", 10087},
      {"localhost", 10088},
      {"localhost", 10089}
  });
  exr::Alg t_alg = 't', e_alg = 'e';
  exr::Path t_path = "src/task/test/tasks.txt",
            b_path = "src/task/test/bandwidths.txt";
  std::thread t[total - 1];

  //Connect
  exr::Controller con(total, size, psize);
  std::array<exr::AccessCenter, total - 1> ac = {
      {{1, total}, {2, total}, {3, total}}
  };
  for (int i = 0; i < total - 1; ++i)
    t[i] = std::thread([&, i] { ac[i].Connect(ip_ads); });
  con.Connect(ip_ads);
  for (int i = 0; i < total - 1; ++i) t[i].join();
  std::cout << "Connected!" << std::endl;

  //Show tasks
  std::mutex mtx;
  for (int i = 0; i < total - 1; ++i) {
    t[i] = std::thread([&, i] {
      exr::RepairTask rt;
      exr::Count c;
      while (true) {
        ac[i].Receive(0, sizeof(rt), &rt);
        if (rt.size == 0) break;
        std::unique_lock<std::mutex> lck(mtx);
        std::cout << std::endl
                  << "node " << i + 1 << " receives: " << std::endl
                  << "  task_id:   " << rt.task_id << std::endl
                  << "  tar_id:    " << rt.tar_id << std::endl
                  << "  offset:    " << rt.offset << std::endl
                  << "  size:      " << rt.size << std::endl
                  << "  psize:     " << rt.piece_size << std::endl
                  << "  bandwidth: " << rt.bandwidth << std::endl
                  << "  coef:      " << static_cast<int>(rt.coef)
                  << std::endl << "  src_ids:   ";
        for (exr::Count j = 0; j < rt.src_num; ++j) {
          ac[i].Receive(0, sizeof(c), &c);
          std::cout << c << " ";
        }
        std::cout << std::endl;
        lck.unlock();
        if (rt.tar_id == i + 1) {
          std::this_thread::sleep_for(std::chrono::milliseconds(1000));
          ac[i].Send(0, sizeof(c), &c);
        }
      }
    });
  }

  //Run task reader
  std::cout << std::endl
            << "------------ START TASK READER TEST ------------"
            << std::endl;
  con.ChangeAlg(t_alg, nullptr, t_path);
  while (con.GetTasks()) {
    auto mtn = con.DoTaskGroups(total);
    std::cout << std::endl
              << "******ONE TASK GROUP FINISHED******"
              << "(" << mtn << ")"
              << std::endl;
  }

  //Run algorithm
  std::cout << std::endl
            << "------------ START EXR ALG TEST ------------"
            << std::endl;
  exr::Count args[] = {2, 3, 3, 1};
  con.ChangeAlg(e_alg, args, b_path);
  while (con.GetTasks()) {
    auto mtn = con.DoTaskGroups(total);
    std::cout << std::endl
              << "******ONE TASK GROUP FINISHED******"
              << "(" << mtn << ")"
              << std::endl;
  }

  //Close
  con.Close(total);
  for (int i = 0; i < total - 1; ++i) t[i].join();
  std::cout << std::endl << "All threads closed, test ended." << std::endl;
  return 0;
}

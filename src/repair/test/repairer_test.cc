#include <iostream>
#include <memory>
#include <mutex>

#include "data/access/access_center.hh"
#include "repair/repairer.hh"
#include "util/typedef.hh"
#include "util/types.hh"

int main()
{
  //Parameters
  exr::Path dpath = "src/repair/test/";
  exr::Path pathr = "rfile.txt";
  exr::Path pathw = "-wfile.txt";
  auto ip_addresses = exr::IPAddressList(new exr::IPAddress[7]{
    {"localhost", 10083},
    {"localhost", 10084},
    {"localhost", 10085},
    {"localhost", 10086},
    {"localhost", 10087},
    {"localhost", 10088},
    {"localhost", 10089} });
  exr::Path bw_path = "";
  exr::Name eth_name = "";

  //Initialize
  auto _ = system(("dd if=/dev/urandom of=" + dpath + pathr +
                   " bs=2097152 count=64").c_str());
  const exr::Count total = 7;
  const exr::DataSize bsize = 67108864;
  exr::Repairer nr[total - 1] = {
    {1, total, dpath + pathr, dpath + "1" + pathw, total, bsize,
     bw_path, eth_name, true, 6, 3, 10},
    {2, total, dpath + pathr, dpath + "2" + pathw, total, bsize,
     bw_path, eth_name, true, 6, 3, 10},
    {3, total, dpath + pathr, dpath + "3" + pathw, total, bsize,
     bw_path, eth_name, true, 6, 3, 10},
    {4, total, dpath + pathr, dpath + "4" + pathw, total, bsize,
     bw_path, eth_name, true, 6, 3, 10},
    {5, total, dpath + pathr, dpath + "5" + pathw, total, bsize,
     bw_path, eth_name, true, 6, 3, 10},
    {6, total, dpath + pathr, dpath + "6" + pathw, total, bsize,
     bw_path, eth_name, true, 6, 3, 10}};
  exr::AccessCenter ac(0, total);

  //Connect
  std::thread t[total - 1];
  for (int i = 0; i < total - 1; ++i)
    t[i] = std::thread([&, i] { nr[i].Prepare(ip_addresses); });
  ac.Connect(ip_addresses);
  for (int i = 0; i < total - 1; ++i)
    t[i].join();
  std::cout << "Connected and has prepared for repairing..." << std::endl;

  exr::Count c1 = 1, c2 = 2, c3 = 3, c4 = 4, c5 = 5;
  exr::Count r = 0;
  exr::BwType bandwidth = 1000000;
  //Test #1 piece 100
  exr::RepairTask task{1, 0, 2, 0, 1024, 1024, 1, bandwidth};
  ac.Send(1, sizeof(task), &task);

  task.src_num = 1;
  task.tar_id = 3;
  ac.Send(2, sizeof(task), &task);
  ac.Send(2, sizeof(c1), &(c1));

  ac.Send(3, sizeof(task), &task);
  ac.Send(3, sizeof(c2), &(c2));

  ac.Receive(3, sizeof(r), &r);
  std::cout << "node 1, 2, 3 compeleted task " << r << std::endl;

  //Test #2 & #3 piece 10
  std::cout << "start task2 and task3 simultaneously" << std::endl;
  exr::RepairTask task2{2, 0, 2, 0, 1024, 1024, 1, bandwidth};
  ac.Send(1, sizeof(task2), &task2);
  ac.Send(3, sizeof(task2), &task2);

  exr::RepairTask task3{3, 1, 1, 0, 1024, 1024, 1, bandwidth};
  ac.Send(2, sizeof(task3), &task3);
  ac.Send(2, sizeof(c3), &(c3));

  task3.src_num = 0;
  task3.tar_id = 2;
  ac.Send(3, sizeof(task3), &task3);

  task2.src_num = 2;
  ac.Send(2, sizeof(task2), &task2);
  ac.Send(2, sizeof(c3), &(c3));
  ac.Send(2, sizeof(c1), &(c1));

  task3.src_num = 1;
  task3.tar_id = 1;
  ac.Send(1, sizeof(task3), &task3);
  ac.Send(1, sizeof(c2), &(c2));

  std::mutex mtx;
  t[0] = std::thread([&] {
    exr::Count k;
    ac.Receive(1, sizeof(k), &k);
    std::unique_lock<std::mutex> lck(mtx);
    std::cout << "node 1, 2, 3 compeleted task " << k << std::endl;
    lck.unlock();
  });
  t[1] = std::thread([&] {
    exr::Count k;
    ac.Receive(2, sizeof(k), &k);
    std::unique_lock<std::mutex> lck(mtx);
    std::cout << "node 1, 2, 3 compeleted task " << k << std::endl;
    lck.unlock();
  });
  t[0].join();
  t[1].join();

  //Test #4 psize 32768
  std::cout << "start task4" << std::endl;
  exr::DataSize size = 67108864, psize = 67108864 / 4;
  auto task4 = exr::RepairTask{4, 0, 3, 0, size, psize, 1, bandwidth};
  ac.Send(2, sizeof(task4), &task4);

  task4.tar_id = 4;
  task4.src_num = 1;
  ac.Send(3, sizeof(task4), &task4);
  ac.Send(3, sizeof(c2), &(c2));

  task4.tar_id = 5;
  ac.Send(4, sizeof(task4), &task4);
  ac.Send(4, sizeof(c3), &(c3));

  task4.tar_id = 1;
  ac.Send(5, sizeof(task4), &task4);
  ac.Send(5, sizeof(c4), &(c4));

  ac.Send(1, sizeof(task4), &task4);
  ac.Send(1, sizeof(c5), &(c5));

  ac.Receive(1, sizeof(r), &r);
  std::cout << "node 2, 3, 4, 5, 1 compeleted task " << r << std::endl;

  //Close
  _ = system(("rm " + dpath + "*.txt").c_str());
  exr::RepairTask end_task{0, 0, 0, 0, 0, 0, 0};
  for (int i = 1; i < total; ++i) {
    ac.Send(i, sizeof(end_task), &end_task);
    nr[i - 1].WaitForFinish();
  }
  std::cout << "Closed, test ended" << std::endl;

  ++_;
  return 0;
}

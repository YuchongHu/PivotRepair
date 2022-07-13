#include <array>
#include <iostream>
#include <sys/time.h>
#include <thread>

#include "data/access/access_center.hh"
#include "repair/procs/proceed_processor.hh"
#include "util/memory_pool.hh"
#include "util/typedef.hh"

int main()
{
  const exr::Count total = 4, id = 1, thr_n = 20;
  const exr::DataSize buf_size = 1 << 16;
  exr::Path path = "src/repair/procs/test/test.txt";
  auto ip_ads = exr::IPAddressList(new exr::IPAddress[total]{
      {"localhost", 10086},
      {"localhost", 10087},
      {"localhost", 10088}
  });

  //Network connection
  std::thread t[total];
  std::array<exr::AccessCenter, total> ac = { {{0, total}, {1, total},
                                               {2, total}, {3, total}} };
  for (int i = 0; i < total; ++i) {
    t[i] = std::thread([&, i] {
      ac[i].Connect(ip_ads);
    });
  }
  for (int i = 0; i < total; ++i)
    t[i].join();
  std::cout << "Connected" << std::endl;

  //Initialization
  exr::ProceedProcessor pp(id, total, thr_n, path, ac[id]);
  struct timeval start_time, end_time;
  exr::BufUnit buf[buf_size] = "abcdefghijklmnopgrstuvwxyz";
  pp.Run();
  exr::DataSize size, psize, remain;
  exr::BwType bandwidth;

  //Single send task test
  size = 67108864;
  psize = 65536;
  bandwidth = 250000;
  std::cout << std::endl << "Single send task test started" << std::endl;
  t[0] = std::thread([&] {
    exr::Count task_id;
    ac[0].Receive(2, sizeof(task_id), &task_id);
    gettimeofday(&end_time, nullptr);
    double duration = (end_time.tv_sec - start_time.tv_sec) * 1e6 +
                      (end_time.tv_usec - start_time.tv_usec);
    std::cout << "task " << task_id << " finished" << std::endl
              << "  sending data " << size << " by psize " << psize
              << ", using time: " << duration
              << ", by bandwidth: " << bandwidth
              << std::endl;
  });
  t[1] = std::thread([&] {
    exr::Count tt;
    exr::DataSize nn = 0, oo, ss;
    exr::BufUnit bb[buf_size];
    while (nn < size) {
      ac[2].Receive(id, sizeof(tt), &tt);
      ac[2].Receive(id, sizeof(oo), &oo);
      ac[2].Receive(id, sizeof(ss), &ss);
      ac[2].Receive(id, ss, bb);
      nn += ss;
    }
    ac[2].Send(0, sizeof(tt), &tt);
  });
  pp.PushData({5, 0, size, nullptr, 0, 0, 0});
  gettimeofday(&start_time, nullptr);
  remain = size;
  while (remain > 0) {
    auto s = remain > psize ? psize : remain;
    pp.PushData({5, size - remain, s, buf, 2, 1, bandwidth});
    remain -= s;
  }
  t[0].join();
  t[1].join();

  //Single store task test
  size = 67108864;
  psize = 65536;
  bandwidth = 250000;
  std::cout << std::endl << "Single store task test started" << std::endl;
  t[0] = std::thread([&] {
    exr::Count task_id;
    ac[0].Receive(id, sizeof(task_id), &task_id);
    gettimeofday(&end_time, nullptr);
    double duration = (end_time.tv_sec - start_time.tv_sec) * 1e6 +
                      (end_time.tv_usec - start_time.tv_usec);
    std::cout << "task " << task_id << " finished" << std::endl
              << "  storing data " << size << " by psize " << psize
              << ", using time: " << duration
              << ", by bandwidth: " << bandwidth
              << std::endl;
  });
  pp.PushData({9, 0, size, nullptr, 0, 0, 0});
  gettimeofday(&start_time, nullptr);
  remain = size;
  while (remain > 0) {
    auto s = remain > psize ? psize : remain;
    pp.PushData({9, size - remain, s, buf, id, 0, bandwidth});
    remain -= s;
  }
  t[0].join();

  //Double send task test
  size = 67108864;
  psize = 65536;
  bandwidth = 250000;
  std::cout << std::endl << "Double send task test started" << std::endl;
  std::thread tint[2], trec[2], t_t[2];
  std::mutex mtx;
  for (exr::Count i = 0; i < 2; ++i) {
    tint[i] = std::thread([&, i] {
      exr::Count ftid, eid = i + 2;
      ac[0].Receive(eid, sizeof(ftid), &ftid);
      struct timeval etime;
      gettimeofday(&etime, nullptr);
      double duration = (etime.tv_sec - start_time.tv_sec) * 1e6 +
                        (etime.tv_usec - start_time.tv_usec);
      std::unique_lock<std::mutex> lck(mtx);
      std::cout << "task " << ftid << " finished" << std::endl
                << "  sending data " << size << " by psize " << psize
                << ", using time: " << duration
                << ", by bandwidth: " << bandwidth
                << std::endl;
    });
  }
  for (int i = 0; i < 2; ++i) {
    trec[i] = std::thread([&, i] {
      exr::Count tt;
      exr::DataSize oo, ss, nn = 0;
      exr::BufUnit bb[buf_size];
      while (nn < size) {
        ac[i + 2].Receive(id, sizeof(tt), &tt);
        ac[i + 2].Receive(id, sizeof(oo), &oo);
        ac[i + 2].Receive(id, sizeof(ss), &ss);
        ac[i + 2].Receive(id, ss, bb);
        nn += ss;
      }
      ac[i + 2].Send(0, sizeof(tt), &tt);
    });
  }
  gettimeofday(&start_time, nullptr);
  for (exr::Count i = 0; i < 2; ++i) {
    t_t[i] = std::thread([&, i] {
      exr::Count ttiidd = i + 2;
      auto rr = size;
      while (rr > 0) {
        auto s = rr > psize ? psize : rr;
        pp.PushData({i, size - rr, s, buf, ttiidd, 0, bandwidth});
        rr -= s;
      }
    });
  }
  pp.PushData({0, 0, size, nullptr, 0, 0, 0});
  pp.PushData({1, 0, size, nullptr, 0, 0, 0});
  for (int i = 0; i < 2; ++i) {
    tint[i].join();
    trec[i].join();
    t_t[i].join();
  }

  //Cleaning
  auto _ = system(("rm " + path).c_str());
  ++_;
  return 0;
}

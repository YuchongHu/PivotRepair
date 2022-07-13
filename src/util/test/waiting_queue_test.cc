#include <iostream>
#include <thread>

#include "util/waiting_queue.hh"

int main()
{
  const int thr_num = 10, times = 1000000;

  //Create
  exr::WaitingQueue<int> wqi;
  std::cout << "WaitingQueue created" << std::endl;

  //Test
  std::thread tpush[thr_num], tpop[thr_num];
  for (int i = 0; i < thr_num; ++i) {
    tpush[i] = std::thread([&] {
      for (int j = 0; j < times; ++j)
        wqi.Push(j);
    });
    tpop[i] = std::thread([&] {
      for (int j = 0; j < times; ++j)
        wqi.Pop();
    });
  }
  std::cout << thr_num
            << " pairs of threads start pushing & poping for each "
            << times
            << " times..."
            << std::endl;

  //Wait for finish
  for (int i = 0; i < thr_num; ++i) {
    tpush[i].join();
    tpop[i].join();
  }
  std::cout << "Test ended" << std::endl;

  //Close
  wqi.Close();
  return 0;
}

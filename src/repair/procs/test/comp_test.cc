#include <iostream>
#include <thread>

#include "repair/procs/compute_processor.hh"
#include "util/memory_pool.hh"
#include "util/typedef.hh"
#include "util/types.hh"

//To show what the output is
class DataShower : public exr::DataProcessor<exr::DataPiece> {
 public:
  DataShower() : exr::DataProcessor<exr::DataPiece>(1, 1) {}
  ~DataShower() = default;

 protected:
  exr::Count Distribute(const exr::DataPiece &data) override { return 0; }
  void Process(exr::DataPiece data, exr::Count pid) override {
    std::cout << "Detected a new DataPiece:" << std::endl
              << "\ttask_id:   " << data.task_id << std::endl
              << "\toffset:    " << data.offset << std::endl
              << "\tsize:      " << data.size << std::endl
              << "\ttar_id:    " << data.tar_id << std::endl
              << "\tsrc_num:   " << data.src_num << std::endl
              << "\tdelaytime: " << data.delay_time << std::endl;
    if (data.buf) {
      std::cout << "\tcontent:   ";
      std::cout.write(data.buf, data.size);
    } else {
      std::cout << "\twith no content.";
    }
    std::cout << std::endl << std::endl;
  }
};

//Main
int main()
{
  //Parameters
  const exr::Count thr_n = 3, buf_n = 100;
  exr::DataSize buf_size = 1 << 10;

  const exr::Count src_num = 3, psize = 7, pn = 3, times = 5;
  exr::BufUnit buf[30] = "abcdefghijklmnopqrstuvwxyz";

  //Initialization
  exr::MemoryPool mp(buf_n, buf_size);
  DataShower ds;
  exr::ComputeProcessor cp(thr_n, mp, ds);
  ds.Run();
  cp.Run();

  //Test
  std::thread ts[src_num * pn * times];
  for (exr::Count i = 0; i < src_num * pn * times; ++i) {
    ts[i] = std::thread([&, i] {
      exr::Count tid = i / (src_num * pn), pid = i % pn;
      cp.PushData({tid, tid * pn * psize + pid * psize, psize,
                   buf + pid * psize, 0, 0, 0});
    });
  }
  std::thread lt[pn * times];
  for (exr::Count i = 0; i < pn * times; ++i) {
    lt[i] = std::thread([&, i] {
      exr::Count tid = i / pn, pid = i % pn,
                 tar = i / pn + 10, sn = src_num + 1;
      cp.PushData({tid, tid * pn * psize + pid * psize, 0,
                   nullptr, tar, sn, 500});
    });
  }
  std::thread it[times];
  for (exr::Count i = 0; i < times; ++i) {
    it[i] = std::thread([&, i] {
      cp.PushData({i, 0, psize * pn, nullptr, 0, 0, 0});
    });
  }

  for (exr::Count i = 0; i < src_num * pn * times; ++i) ts[i].join();
  for (exr::Count i = 0; i < pn * times; ++i) lt[i].join();
  for (exr::Count i = 0; i < times; ++i) it[i].join();

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  return 0;
}

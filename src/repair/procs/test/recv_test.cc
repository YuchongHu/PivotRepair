#include <array>
#include <iostream>
#include <thread>

#include "data/access/access_center.hh"
#include "repair/procs/data_processor.hh"
#include "repair/procs/receive_processor.hh"
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
  const exr::Count total = 3, id = 1, thr_n = 3, buf_n = 100;
  exr::Path path = "src/repair/procs/test/test.txt";
  if (system(("echo hello,world! >> " + path).c_str()) == -1) {
    std::cerr << "Create file error" << std::endl;
    exit(-1);
  }
  auto ip_ads = exr::IPAddressList(new exr::IPAddress[total]{
      {"localhost", 10086},
      {"localhost", 10087},
      {"localhost", 10088}
  });
  exr::DataSize buf_size = 1 << 10;

  //Network connection
  std::thread t[total];
  std::array<exr::AccessCenter, total> ac = { {{0, total}, {1, total},
                                               {2, total}} };
  for (int i = 0; i < total; ++i) {
    t[i] = std::thread([&, i] {
      ac[i].Connect(ip_ads);
    });
  }
  for (int i = 0; i < total; ++i)
    t[i].join();
  std::cout << "Connected" << std::endl;

  //Initialization
  exr::MemoryPool mp(buf_n, buf_size);
  DataShower ds;
  exr::ReceiveProcessor rp(total, id, path, thr_n, ac[id], mp, ds);
  ds.Run();
  rp.Run();

  //Local load test
  std::cout << "Start local file loading test..." << std::endl << std::endl;
  rp.PushData({{1, 3, 2, 2, 9, 2, 1, 200}, 1});
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  rp.PushData({{9, 1, 2, 0, 8, 4, 40, 500}, 1});
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  //Network receive test
  std::cout << "Start network receiving test..." << std::endl << std::endl;

  std::cout << "Sending a piece" << std::endl;
  exr::Count task_id = 2;
  exr::DataSize offset = 80, size = 5;
  exr::BufUnit temp_buf[20] = "abcdefghijk";
  ac[2].Send(id, sizeof(task_id), &task_id);
  ac[2].Send(id, sizeof(offset), &offset);
  ac[2].Send(id, sizeof(size), &size);
  ac[2].Send(id, size, temp_buf);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  std::cout << "Pushing a task" << std::endl;
  rp.PushData({{2, 2, 1, 80, 10, 5, 1, 300}, 2});
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  std::cout << "Sending a piece" << std::endl;
  exr::Count task_id2 = 3;
  exr::DataSize offset2 = 256, size2 = 10;
  exr::BufUnit temp_buf2[20] = "ABCDEFGHIJK";
  ac[2].Send(id, sizeof(task_id2), &task_id2);
  ac[2].Send(id, sizeof(offset2), &offset2);
  ac[2].Send(id, sizeof(size2), &size2);
  ac[2].Send(id, size2, temp_buf2);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  std::cout << "Sending a piece" << std::endl;
  offset += size;
  ac[2].Send(id, sizeof(task_id), &task_id);
  ac[2].Send(id, sizeof(offset), &offset);
  ac[2].Send(id, sizeof(size), &size);
  ac[2].Send(id, size, temp_buf + size);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  std::cout << "Pushing a task" << std::endl;
  rp.PushData({{3, 4, 3, 256, 10, 10, 3, 600}, 2});
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  return 0;
}

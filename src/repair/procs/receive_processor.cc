#include "repair/procs/receive_processor.hh"

#include <sys/time.h>

#include "data/file/file_reader.hh"
#include "util/rs_computer.hh"

namespace exr {

//Constructor and destructor
ReceiveProcessor::ReceiveProcessor(const Count &total, const Count &id,
                                   const Path &path, const Count &thr_n,
                                   AccessCenter &ac, MemoryPool &mp,
                                   DataProcessor<DataPiece> &next_prc)
    : DataProcessor<ReceiveTask>(1, thr_n),
      id_(id), path_(path), ac_(ac), mp_(mp), next_prc_(next_prc),
      remains_(std::make_unique<DataSize[]>(total - 1)) {
  for (Count i = 0; i < total - 1; ++i)
    remains_[i] = 0;
}

ReceiveProcessor::~ReceiveProcessor() { Close(); }

//Distribute
Count ReceiveProcessor::Distribute(const ReceiveTask &data) { return 0; }

//Distinguish between a local task and a remote task
void ReceiveProcessor::Process(ReceiveTask data, Count qid) {
  //Send the infomation of the task to the next processor
  if (data.src_id == id_)
    LoadData_(std::move(data));
  else
    ReceiveData_(std::move(data));
}

//Load data from local
void ReceiveProcessor::LoadData_(ReceiveTask data) {
  //Send task's size to the next processor
  auto t = std::chrono::system_clock::now();
  next_prc_.PushData({data.rt.task_id, 0, data.rt.size, nullptr, 0, 0, 0});

  //Initialization
  RSComputer rc(1, 1);
  exr::FileReader reader;
  BufUnit *buf = nullptr, *temp_buf = nullptr;
  DataSize remain = data.rt.size, offset = data.rt.offset, size = 0;

  //Check if need to load data
  if (data.rt.tar_id != id_) {
    rc.InitForEncode(&(data.rt.coef));
    reader.Open(path_);
    reader.SetOffset(offset);
    buf = mp_.Get(id_, offset);
    temp_buf = mp_.Get(data.rt.tar_id, offset);
  }

  TTime dt = 0;
  size = data.rt.piece_size;
  if (data.rt.bandwidth > 0)
    dt = static_cast<TTime>((size * 8000.0) / data.rt.bandwidth);
  //Load pieces
  while (remain > 0) {
    DataPiece dp{data.rt.task_id, offset, 0, buf, data.rt.tar_id,
                 data.rt.src_num, dt};
    if (remain < size) {
      size = remain;
      if (data.rt.bandwidth > 0)
        dt = static_cast<TTime>((size * 8000.0) / data.rt.bandwidth);
    }

    if (buf) {
      dp.size = size;
      //Load data
      auto s = reader.Read(size, temp_buf);
      if (s != size) {
        std::cerr << "File is not big enough for reading..." << std::endl;
        exit(-1);
      }
      //Multiply
      BufUnit *srcs[1] = {temp_buf}, *tars[1] = {dp.buf};
      rc.Encode(size, srcs, tars);
      buf += size;
      temp_buf += size;
      //Wait
      t += std::chrono::microseconds(dt);
      std::this_thread::sleep_until(t);
    }

    next_prc_.PushData(std::move(dp));
    remain -= size;
    offset += size;
  }
}

//Get pieces from other nodes
void ReceiveProcessor::ReceiveData_(ReceiveTask data) {
  std::unique_lock<std::mutex> lck(mtx_);
  remains_[data.src_id - 1] += data.rt.size;
  //If a task of the same source is running, this thread needn't do anything
  if (remains_[data.src_id - 1] > data.rt.size)
    return;

  while (remains_[data.src_id - 1] > 0) {
    lck.unlock();

    DataPiece dp{0, 0, 0, nullptr, 0, 0, 0};
    ac_.Receive(data.src_id, sizeof(dp.task_id), &(dp.task_id));
    ac_.Receive(data.src_id, sizeof(dp.offset), &(dp.offset));
    ac_.Receive(data.src_id, sizeof(dp.size), &(dp.size));
    dp.buf = mp_.Get(data.src_id, dp.offset);
    ac_.Receive(data.src_id, dp.size, dp.buf);

    auto size = dp.size;
    next_prc_.PushData(std::move(dp));

    lck.lock();
    remains_[data.src_id - 1] -= size;
  }
}

} // namespace exr

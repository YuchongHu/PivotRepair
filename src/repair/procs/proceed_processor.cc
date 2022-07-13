#include "repair/procs/proceed_processor.hh"

#include <sys/time.h>
#include <thread>

#include "data/file/file_writer.hh"

namespace exr {

//Constructor and destructor
ProceedProcessor::ProceedProcessor(const Count &id, const Count &total,
                                   const Count &thr_n, const Path &path,
                                   AccessCenter &ac)
    : DataProcessor<DataPiece>(thr_n, 1), id_(id), ac_(ac), path_(path),
      mtxs_(std::make_unique<std::mutex[]>(total)),
      sizes_(std::make_unique<DataSize[]>(thr_n)) {
  for (Count i = 0; i < thr_n; ++i) {
    sizes_[i] = 0;
    free_threads_.push(i);
  }
}

ProceedProcessor::~ProceedProcessor() {
  writer_.Close();
  Close();
}

//Distribute
Count ProceedProcessor::Distribute(const DataPiece &data) {
  std::unique_lock<std::mutex> lck(mtxs_[0]);
  auto it = task_threads_.find(data.task_id);
  if (it != task_threads_.end()) {
    return it->second;
  } else if (free_threads_.empty()) {
    return queue_n_;
  } else {
    auto qid = free_threads_.front();
    free_threads_.pop();
    task_threads_[data.task_id] = qid;
    return qid;
  }
}

//Send the data out
void ProceedProcessor::Process(DataPiece data, Count qid) {
  //Store or send data
  if (data.buf) {
    if (data.tar_id == id_)
      Store_(data);
    else
      Send_(data);
    sizes_[qid] -= data.size;
  } else {
    sizes_[qid] += data.size;
  }

  //Check if finished
  if (sizes_[qid] == 0) {
    std::unique_lock<std::mutex> lck(mtxs_[0]);
    task_threads_.erase(data.task_id);
    if (data.tar_id == id_) {
      ac_.Send(0, sizeof(data.task_id), &(data.task_id));
    }
    free_threads_.push(qid);
  }
}

void ProceedProcessor::Store_(DataPiece &data) {
  std::unique_lock<std::mutex> lck(mtxs_[id_]);
  if (!writer_.is_open()) writer_.Open(path_);
  writer_.Write(data.offset, data.size, data.buf);
}

void ProceedProcessor::Send_(DataPiece &data) {
  auto ts = std::chrono::system_clock::now();

  //Send data
  std::unique_lock<std::mutex> lck(mtxs_[data.tar_id]);
  ac_.Send(data.tar_id, sizeof(data.task_id), &(data.task_id));
  ac_.Send(data.tar_id, sizeof(data.offset), &(data.offset));
  ac_.Send(data.tar_id, sizeof(data.size), &(data.size));
  ac_.Send(data.tar_id, data.size, data.buf);
  lck.unlock();

  if (data.delay_time > 0) {
    auto tp = ts + std::chrono::microseconds(data.delay_time);
    std::this_thread::sleep_until(tp);
  }
}

} // namespace exr

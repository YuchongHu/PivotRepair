/* Class WaitingQueue -- from "util/waiting_queue.hh" */

namespace exr {

//Constructor and destructor
template <typename Data> WaitingQueue<Data>::WaitingQueue()
    : close_flag_(false) {}

template <typename Data> WaitingQueue<Data>::~WaitingQueue() = default;

//Insert data
template <typename Data> void WaitingQueue<Data>::Push(Data data) {
  std::unique_lock<std::mutex> lck(mtx_);
  data_queue_.push(std::move(data));
  lck.unlock();
  cv_.notify_one();
}

//Get earliest data
template <typename Data> Data WaitingQueue<Data>::Pop() {
  std::unique_lock<std::mutex> lck(mtx_);
  while (!close_flag_ && data_queue_.empty())
    cv_.wait(lck, [&] { return close_flag_ || !data_queue_.empty(); });

  if (close_flag_) {
    return Data();
  } else {
    Data data = std::move(data_queue_.front());
    data_queue_.pop();
    return data;
  }
}

//Close and wake up waiting threads
template <typename Data> void WaitingQueue<Data>::Close() {
  std::unique_lock<std::mutex> lck(mtx_);
  close_flag_ = true;
  lck.unlock();
  cv_.notify_all();
}

} // namespace exr

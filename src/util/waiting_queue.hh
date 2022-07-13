#ifndef EXR_UTIL_WAITINGQUEUE_HH_
#define EXR_UTIL_WAITINGQUEUE_HH_

#include <condition_variable>
#include <mutex>
#include <queue>

namespace exr {

/* A queue that getters will wait for data to be filled */
template <typename Data>
class WaitingQueue
{
 public:
  WaitingQueue();
  ~WaitingQueue();

  //Store and get data
  void Push(Data data);
  Data Pop();

  //Wake up all the waiting threads
  void Close();

  //WaitingQueue is neither copyable nor movable
  WaitingQueue(const WaitingQueue&) = delete;
  WaitingQueue& operator=(const WaitingQueue&) = delete;

 private:
  std::queue<Data> data_queue_;
  std::mutex mtx_;
  std::condition_variable cv_;
  bool close_flag_;
};

} // namespace exr

#include "util/waiting_queue-inl.hh"

#endif // EXR_UTIL_WAITINGQUEUE_HH_

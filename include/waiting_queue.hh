#ifndef FTPR_WAITING_QUEUE_HH
#define FTPR_WAITING_QUEUE_HH

#include <queue>
#include <mutex>
#include <condition_variable>

template <typename Data>
class WaitingQueue
{
private:
  std::queue<Data> data_queue;
  std::mutex mtx;
  std::condition_variable cv;
  bool close_flag;

public:
  WaitingQueue()
      : close_flag{false} {}
  ~WaitingQueue() = default;

  void push(Data data) {
    std::unique_lock<std::mutex> lck(mtx);
    data_queue.push(std::move(data));
    lck.unlock();
    cv.notify_one();
  }

  Data pop() {
    std::unique_lock<std::mutex> lck(mtx);
    while (!close_flag && data_queue.empty())
    {
      cv.wait(lck, [&] { return close_flag || !data_queue.empty(); });
    }
    if (close_flag)
    {
      return Data();
    }
    else
    {
      Data data = data_queue.front();
      data_queue.pop();
      return data;
    }
  }

  void close() {
    std::unique_lock<std::mutex> lck(mtx);
    close_flag = true;
    lck.unlock();
    cv.notify_all();
  }
};

#endif
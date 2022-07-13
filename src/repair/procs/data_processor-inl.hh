/* Class DataProcessor -- from "repair/procs/data_processor.hh" */

namespace exr {

//Constructor
template <typename Data>
DataProcessor<Data>::DataProcessor(const Count &queue_n, const Count &thr_n)
    : queue_n_(queue_n), on_run_(false),
      data_queues_(new WaitingQueue<Data>[queue_n]),
      thr_n_(thr_n), threads_(new std::thread[queue_n * thr_n]) {}

//Destructor
template <typename Data>
DataProcessor<Data>::~DataProcessor() {
  Close();
}

//Run the processor
template <typename Data>
void DataProcessor<Data>::Run() {
  on_run_ = true;
  for (Count i = 0; i < queue_n_; ++i) {
    for (Count j = 0; j < thr_n_; ++j) {
      threads_[i * thr_n_ + j] = std::thread([&, i] {
        while (on_run_) {
          auto data = std::move(data_queues_[i].Pop());
          if (!on_run_) break;
          Process(std::move(data), i);
        }
      });
    }
  }
}

//Close the processor
template <typename Data>
void DataProcessor<Data>::Close() {
  if (on_run_) {
    on_run_ = false;
    for (Count i = 0; i < queue_n_; ++i)
      data_queues_[i].Close();
    for (Count i = 0; i < queue_n_ * thr_n_; ++i)
      threads_[i].join();
  }
}

//Distribute the incoming data to a specific processing queue
template <typename Data>
void DataProcessor<Data>::PushData(Data data) {
  auto id = Distribute(data);
  if (on_run_) {
    if (id < queue_n_)
      data_queues_[id].Push(std::move(data));
    else
      PushData(std::move(data));
  }
}

} // namespace exr

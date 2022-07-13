#ifndef EXR_REPAIR_PROCS_DATAPROCESSOR_HH_
#define EXR_REPAIR_PROCS_DATAPROCESSOR_HH_

#include <memory>
#include <thread>

#include "util/typedef.hh"
#include "util/waiting_queue.hh"

namespace exr {

/* A processor which can use mutithreads to deal with input data */
template <typename Data>
class DataProcessor
{
 public:
  DataProcessor(const Count &queue_n, const Count &thr_n);
  ~DataProcessor();

  //Run the processor
  void Run();
  //Close the processor
  void Close();
  //Add a data into the processor
  void PushData(Data data);

  //DataProcessor is neither copyable nor movable
  DataProcessor(const DataProcessor&) = delete;
  DataProcessor& operator=(const DataProcessor&) = delete;

 protected:
  //Number of queues
  Count queue_n_;
  //Determine which queue to deal with the data
  virtual Count Distribute(const Data &data) = 0;
  //Process the data
  virtual void Process(Data data, Count qid) = 0;

 private:
  bool on_run_; //Whether the processor is still running
  std::unique_ptr<WaitingQueue<Data>[]> data_queues_;
  Count thr_n_; //Number of threads
  std::unique_ptr<std::thread[]> threads_;
};

} // namespace exr

#include "repair/procs/data_processor-inl.hh"

#endif // EXR_REPAIR_PROCS_DATAPROCESSOR_HH_

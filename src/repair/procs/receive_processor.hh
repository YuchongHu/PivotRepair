#ifndef EXR_REPAIR_PROCS_RECEIVEPROCESSOR_HH_
#define EXR_REPAIR_PROCS_RECEIVEPROCESSOR_HH_

#include <memory>
#include <mutex>

#include "data/access/access_center.hh"
#include "repair/procs/data_processor.hh"
#include "util/memory_pool.hh"
#include "util/typedef.hh"
#include "util/types.hh"

namespace exr {

/* A Processor that can load local data and receive data from other nodes */
class ReceiveProcessor : public DataProcessor<ReceiveTask>
{
 public:
  ReceiveProcessor(const Count &total, const Count &id,
                   const Path &path, const Count &thr_n,
                   AccessCenter &ac, MemoryPool &mp,
                   DataProcessor<DataPiece> &next_prc);
  ~ReceiveProcessor();

  //ReceiveProcessor is neither copyable nor movable
  ReceiveProcessor(const ReceiveProcessor&) = delete;
  ReceiveProcessor& operator=(const ReceiveProcessor&) = delete;

 protected:
  Count Distribute(const ReceiveTask &data) override;
  void Process(ReceiveTask data, Count qid) override;

 private:
  Count id_;
  Path path_;
  AccessCenter &ac_;
  MemoryPool &mp_;
  DataProcessor<DataPiece> &next_prc_;

  //The remain size to receive of each node
  std::unique_ptr<DataSize[]> remains_;
  std::mutex mtx_;

  void LoadData_(ReceiveTask data);
  void ReceiveData_(ReceiveTask data);
};

} // namespace exr

#endif // EXR_REPAIR_PROCS_RECEIVEPROCESSOR_HH_

#ifndef EXR_REPAIR_PROCS_PROCEEDPROCESSOR_HH_
#define EXR_REPAIR_PROCS_PROCEEDPROCESSOR_HH_

#include <memory>
#include <mutex>
#include <unordered_map>
#include <queue>

#include "data/access/access_center.hh"
#include "data/file/file_writer.hh"
#include "repair/procs/data_processor.hh"
#include "util/typedef.hh"
#include "util/types.hh"

namespace exr {

/* A Processor that receive DataPieces and send them out */
class ProceedProcessor : public DataProcessor<DataPiece>
{
 public:
  ProceedProcessor(const Count &id, const Count &total, const Count &thr_n,
                   const Path &path, AccessCenter &ac);
  ~ProceedProcessor();

  //ProceedProcessor is neither copyable nor movable
  ProceedProcessor(const ProceedProcessor&) = delete;
  ProceedProcessor& operator=(const ProceedProcessor&) = delete;

 protected:
  Count Distribute(const DataPiece &data) override;
  void Process(DataPiece data, Count id) override;

 private:
  Count id_;
  AccessCenter &ac_;
  Path path_;
  FileWriter writer_;

  std::unordered_map<Count, Count> task_threads_;
  std::queue<Count> free_threads_;
  std::unique_ptr<std::mutex[]> mtxs_;

  std::unique_ptr<DataSize[]> sizes_;

  void Store_(DataPiece &data);
  void Send_(DataPiece &data);
};

} // namespace exr

#endif // EXR_REPAIR_PROCS_PROCEEDPROCESSOR_HH_

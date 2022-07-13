#ifndef EXR_REPAIR_PROCS_COMPUTEPROCESSOR_HH_
#define EXR_REPAIR_PROCS_COMPUTEPROCESSOR_HH_

#include <memory>
#include <mutex>
#include <unordered_map>

#include "repair/procs/data_processor.hh"
#include "util/memory_pool.hh"
#include "util/rs_computer.hh"
#include "util/typedef.hh"
#include "util/types.hh"

namespace exr {

struct TempPiece {
  DataPiece dp;
  Count num;
  Count src_num;
  BufUnit *temp_buf;
  std::mutex mtx;
  TempPiece(DataPiece _dp, const Count &_num)
    : dp(std::move(_dp)), num(_num), src_num(0), temp_buf(nullptr) {}
};

struct PieceGroup {
  std::unordered_map<DataSize, std::unique_ptr<TempPiece>> pieces;
  std::mutex map_mtx;
  DataSize sum;
  DataSize total;
  std::mutex remain_mtx;
  PieceGroup() : sum(0), total(0) {}
};

/* A Processor that can collect data pieces and encode */
class ComputeProcessor : public DataProcessor<DataPiece>
{
 public:
  ComputeProcessor(const Count &thr_n, MemoryPool &mp,
                   DataProcessor<DataPiece> &next_prc);
  ~ComputeProcessor();

  //ComputeProcessor is neither copyable nor movable
  ComputeProcessor(const ComputeProcessor&) = delete;
  ComputeProcessor& operator=(const ComputeProcessor&) = delete;

 protected:
  Count Distribute(const DataPiece &data) override;
  void Process(DataPiece data, Count qid) override;

 private:
  MemoryPool &mp_;
  DataProcessor<DataPiece> &next_prc_;

  RSComputer rc_;
  std::unordered_map<Count, PieceGroup> task_pieces_;
  std::mutex mtx_;

  bool AddPiece_(PieceGroup &pg, DataPiece data);
};

} // namespace exr

#endif // EXR_REPAIR_PROCS_COMPUTEPROCESSOR_HH_

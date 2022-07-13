#ifndef EXR_TASK_ALGORITHM_FTPREPAIR_HH_
#define EXR_TASK_ALGORITHM_FTPREPAIR_HH_

#include <memory>

#include "task/algorithm/route_calculator.hh"
#include "task/algorithm/old_alg/tree_builder.hh"
#include "util/typedef.hh"
#include "util/types.hh"

namespace exr {

class FTPRepair : public RouteCalculator
{
 public:
  FTPRepair(const Count &k, const Count &num, const Count &rid,
            const Alg &alg, const BwType &min_bw, const Path &bw_path);
  ~FTPRepair();

  Count GetTaskNumber(const Count &gid) override;
  void FillTask(const Count &gid, const Count &tid, const Count &node_id,
                RepairTask &rt, Count *src_ids) override;
  BwType get_capacity() override;

  //FTPRepair is neither copyable nor movable
  FTPRepair(const FTPRepair&) = delete;
  FTPRepair& operator=(const FTPRepair&) = delete;

 protected:
  Count CalculateRoute(const Bandwidth *bws, const Count &rid) override;

 private:
  Alg alg_;
  Count num_;
  BwType min_bw_;
  Count rid_;
  std::unique_ptr<TreeBuilder> ptb_;
  BwType capacity_;
};

} // namespace exr

#endif // EXR_TASK_ALGORITHM_FTPREPAIR_HH_

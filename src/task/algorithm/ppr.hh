#ifndef EXR_TASK_ALGORITHM_PPR_HH_
#define EXR_TASK_ALGORITHM_PPR_HH_

#include <memory>
#include <vector>

#include "task/algorithm/route_calculator.hh"
#include "util/typedef.hh"
#include "util/types.hh"

namespace exr {

class PPR : public RouteCalculator
{
 public:
  PPR(const Count &k, const Count &n, const Count &rid,
      const BwType &min_bw, const Path &path);
  ~PPR();

  Count GetTaskNumber(const Count &gid) override;
  void FillTask(const Count &gid, const Count &tid, const Count &node_id,
                RepairTask &rt, Count *src_ids) override;
  BwType get_capacity() override;

  //PPR is neither copyable nor movable
  PPR(const PPR&) = delete;
  PPR& operator=(const PPR&) = delete;

 protected:
  Count CalculateRoute(const Bandwidth *bws, const Count &rid) override;

 private:
  Count n_;
  Count k_;
  BwType min_bw_;
  BwType capacity_;

  std::vector<std::unique_ptr<Count[]>> task_groups_;
};

} // namespace exr

#endif // EXR_TASK_ALGORITHM_PPR_HH_

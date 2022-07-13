#ifndef EXR_TASK_ALGORITHM_ROUTECALCULATOR_HH_
#define EXR_TASK_ALGORITHM_ROUTECALCULATOR_HH_

#include "config/bandwidth_solver.hh"
#include "task/task_getter_interface.hh"
#include "util/typedef.hh"
#include "util/types.hh"

namespace exr {

/* Define how algorithms load data, calculate, and output the result */
class RouteCalculator : public TaskGetterInterface
{
 public:
  RouteCalculator(const Count &rid, const Path &path);
  ~RouteCalculator();

  Count GetNextGroupNumber() override;
  Count GetRid() override;

  //RouteCalculator is neither copyable nor movable
  RouteCalculator(const RouteCalculator&) = delete;
  RouteCalculator& operator=(const RouteCalculator&) = delete;

 protected:
  virtual Count CalculateRoute(const Bandwidth *bws, const Count &rid) = 0;

 private:
  Count rid_;
  BandwidthSolver bs_;
};

} // namespace exr

#endif // EXR_TASK_ALGORITHM_ROUTECALCULATOR_HH_

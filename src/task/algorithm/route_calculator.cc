#include "task/algorithm/route_calculator.hh"

namespace exr {

RouteCalculator::RouteCalculator(const Count &rid, const Path &path)
    : rid_(rid), bs_("", true) {
  bs_.Open(path);
}

RouteCalculator::~RouteCalculator() = default;

Count RouteCalculator::GetNextGroupNumber() {
  if (bs_.LoadNext()) {
    bs_.SetFull(rid_);
    return CalculateRoute(bs_.GetBandwidths(), rid_);
  } else {
    return kMaxGroupNum;
  }
}

Count RouteCalculator::GetRid() {
  return rid_;
}

} // namespace exr

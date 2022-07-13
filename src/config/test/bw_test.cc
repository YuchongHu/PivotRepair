#include "config/bandwidth_solver.hh"

#include "util/typedef.hh"

int main()
{
  std::string name = "ens33";
  exr::Path path = "src/config/test/bandwidths.txt";
  exr::BandwidthSolver bs(name, true);
  return 0;
}

#include <iostream>
#include <memory>

#include "task/task_getter_interface.hh"
#include "task/algorithm/ppr.hh"
#include "util/typedef.hh"
#include "util/types.hh"

int main()
{
  exr::Count k = 7, n = 11;
  exr::BwType min_bw = 50000;
  exr::Path path = "src/task/algorithm/test/bandwidths.txt";

  std::unique_ptr<exr::TaskGetterInterface> ptg(new exr::PPR(k, n, 1,
                                                             min_bw, path));
  auto srcs = std::make_unique<exr::Count[]>(n);

  while (true) {
    //Calculate
    auto gnum = ptg->GetNextGroupNumber();
    if (gnum == exr::kMaxGroupNum) break;

    //Get results
    for (exr::Count gid = 0; gid < gnum; ++gid) {
      auto act_num = ptg->GetTaskNumber(0);
      std::cout << "Task Group " << gid << ": " << std::endl;
      for (exr::Count i = 1; i <= n; ++i) {
        for (exr::Count j = 0; j < act_num; ++j) {
          exr::RepairTask rt{j, 0, 0, 1024, 1024, 20, 1, 0};
          ptg->FillTask(gid, j, i, rt, srcs.get());

          //Output
          if (rt.size > 0) {
            std::cout << "Node " << i << ", Task " << j << ":" << std::endl
                      << "\toffset: " << rt.offset << std::endl
                      << "\tsize: " << rt.size << std::endl
                      << "\tpiece: " << rt.piece_size << std::endl
                      << "\tcoef: " <<static_cast<int>(rt.coef) << std::endl
                      << "\tbandwidth: " << rt.bandwidth << std::endl
                      << "\ttarget: " << rt.tar_id << std::endl
                      << "\tsources:";
            for (int k = 0; k < rt.src_num; ++k)
              std::cout << " " << srcs[k];
            std::cout << std::endl << std::endl;
          }
        }
      }
    }
    break;
  }
  return 0;
}

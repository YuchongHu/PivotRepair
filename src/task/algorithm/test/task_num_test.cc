#include <fstream>
#include <iostream>
#include <memory>

#include "task/task_getter_interface.hh"
#include "util/typedef.hh"
#include "util/types.hh"

int main()
{
  exr::Count nk_num = 3, rid = 1;
  exr::Count ns[] = {6, 12, 16};
  exr::Count ks[] = {4, 8, 12};
  exr::BwType min_bw = 50000;
  exr::Path path = "src/task/algorithm/test/bandwidths.txt",
            result_file = "src/task/algorithm/test/task_nums.txt";

  std::ofstream ofs(result_file);

  for (int i = 0; i < nk_num; ++i) {
    auto srcs = std::make_unique<exr::Count[]>(ns[i]);

    std::unique_ptr<exr::TaskGetterInterface> ptg1(
      new exr::BestFlow(ks[i], ns[i], rid, false, min_bw, path));
    while (true) {
      //Calculate
      auto gnum = ptg1->GetNextGroupNumber();
      if (gnum == exr::kMaxGroupNum) break;
      auto act_num = ptg1->GetTaskNumber(0);
      //Write result
      ofs << act_num << std::endl;
    }

    std::unique_ptr<exr::TaskGetterInterface> ptg2(
      new exr::BestFlow(ks[i], ns[i], rid, true, min_bw, path));
    while (true) {
      //Calculate
      auto gnum = ptg2->GetNextGroupNumber();
      if (gnum == exr::kMaxGroupNum) break;
      auto act_num = ptg2->GetTaskNumber(0);
      //Write result
      ofs << act_num << std::endl;
    }
  }
  ofs.close();
  return 0;
}

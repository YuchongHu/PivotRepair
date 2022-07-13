#include <iostream>
#include <memory>

#include "task/task_getter_interface.hh"
#include "task/algorithm/eva_pipe.hh"
#include "util/typedef.hh"
#include "util/types.hh"

int main()
{
  exr::Count k = 6, num = 9, task_num = 3;
  exr::Path path = "src/task/algorithm/test/tt.txt";

  std::unique_ptr<exr::TaskGetterInterface> ptg(
    new exr::EvaPipe(k, num, 1, task_num, path));
  auto srcs = std::make_unique<exr::Count[]>(num);

  int gid = 0;
  exr::Count tid = 0;
  while (true) {
    //Calculate
    auto gnum = ptg->GetNextGroupNumber();
    if (gnum == exr::kMaxGroupNum) break;
    auto act_num = ptg->GetTaskNumber(0);
    if (act_num != task_num) {
      std::cout << "kkk" << std::endl;
      return 0;
    }
    std::cout << ++gid << std::endl;

    //Get results
    for (exr::Count i = 1; i <= num; ++i) {
      for (exr::Count j = 0; j < act_num; ++j) {
        exr::Count ctid = tid + j;
        exr::RepairTask rt{ctid, 0, 0, 0, 67108864, 32768, 1, 0};
        ptg->FillTask(0, j, i, rt, srcs.get());

        //Output
        if (rt.size > 0) {
          std::cout << "Node " << i << ", Task " << j
                    << "(" << rt.task_id << ")" << ":" << std::endl
                    << "\toffset: " << rt.offset << std::endl
                    << "\tsize: " << rt.size << std::endl
                    << "\tpiece: " << rt.piece_size << std::endl
                    << "\tcoef: " << static_cast<int>(rt.coef) << std::endl
                    << "\tbandwidth: " << rt.bandwidth << std::endl
                    << "\ttarget: " << rt.tar_id << std::endl
                    << "\tsources:";
          for (int k = 0; k < rt.src_num; ++k)
            std::cout << " " << srcs[k];
          std::cout << std::endl << std::endl;
        }
      }
    }
    tid += act_num;
  }
  return 0;
}

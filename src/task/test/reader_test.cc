#include <iostream>
#include <memory>

#include "task/task_getter_interface.hh"
#include "task/task_reader.hh"
#include "util/typedef.hh"
#include "util/types.hh"

int main()
{
  exr::Count max_node_num = 6;
  exr::Path path = "src/task/test/tasks.txt";
  std::unique_ptr<exr::TaskGetterInterface> ptg(new exr::TaskReader(path));

  int num = 0;
  auto srcs = std::make_unique<exr::Count[]>(max_node_num);

  while (true) {
    //Load
    auto group_num = ptg->GetNextGroupNumber();
    if (group_num == exr::kMaxGroupNum) break;
    auto task_num = ptg->GetTaskNumber(0);

    //Get results
    std::cout << "Group " << num++ << std::endl;
    for (exr::Count i = 0; i < max_node_num; ++i) {
      for (exr::Count j = 0; j < task_num; ++j) {
        exr::RepairTask rt{0, 0, 0, 0, 0, 0, 1, 0};
        ptg->FillTask(0, j, i, rt, srcs.get());

        //Output
        if (rt.size > 0) {
          std::cout << "Node " << i << ", Task " << j << ":" << std::endl
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
  }
  return 0;
}

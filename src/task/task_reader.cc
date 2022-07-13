#include "task/task_reader.hh"

#include <iostream>

namespace exr {

TaskReader::TaskReader(const Path &path) {
  task_file_ = std::ifstream(path);
  if (!task_file_.is_open()) {
    std::cerr << "Cannot open task file: " << path << std::endl;
    exit(-1);
  }
  task_file_ >> group_num_;
  cur_num_ = 0;
}

TaskReader::~TaskReader() {
  if (task_file_.is_open())
    task_file_.close();
}

Count TaskReader::GetNextGroupNumber() {
  if (++cur_num_ > group_num_) return kMaxGroupNum;

  capacity_ = 0;
  task_file_ >> task_num_;
  task_infos_ = std::make_unique<TaskInfo[]>(task_num_);
  for (Count i = 0; i < task_num_; ++i) {
    //Load task info
    task_file_ >> task_infos_[i].task_id >> task_infos_[i].offset
               >> task_infos_[i].size >> task_infos_[i].piece_size
               >> task_infos_[i].bandwidth >> task_infos_[i].node_num;
    capacity_ += task_infos_[i].bandwidth;
    //Get nodes' target
    auto node_tasks = std::make_unique<NodeTask[]>(task_infos_[i].node_num);
    for (Count j = 0; j < task_infos_[i].node_num; ++j)
      task_file_ >> node_tasks[j].node_id >> node_tasks[j].tar_id;
    task_infos_[i].node_tasks = std::move(node_tasks);
  }
  return 1;
}

Count TaskReader::GetTaskNumber(const Count &gid) {
  return gid == 0 ? task_num_ : 0;
}

void TaskReader::FillTask(const Count &gid, const Count &tid,
                          const Count &node_id,
                          RepairTask &rt, Count *src_ids) {
  auto &task = task_infos_[tid];
  rt.tar_id = 0;
  rt.src_num = 0;
  for (Count i = 0; i < task.node_num; ++i) {
    auto &ntask = task.node_tasks[i];
    if (ntask.node_id == node_id)
      rt.tar_id = ntask.tar_id;
    else if (ntask.tar_id == node_id)
      src_ids[(rt.src_num)++] = ntask.node_id;
  }
  if (rt.tar_id == 0) {
    rt.size = 0;
    return;
  }

  rt.offset = task.offset;
  rt.size = task.size;
  rt.piece_size = task.piece_size;
  rt.bandwidth = task.bandwidth;
}

BwType TaskReader::get_capacity() { return capacity_; }

Count TaskReader::GetRid() { return 0; }

} // namespace exr

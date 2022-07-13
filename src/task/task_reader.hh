#ifndef EXR_TASK_TASKREADER_HH_
#define EXR_TASK_TASKREADER_HH_

#include <fstream>
#include <memory>

#include "task/task_getter_interface.hh"
#include "util/typedef.hh"
#include "util/types.hh"

namespace exr {

struct NodeTask {
  Count node_id;
  Count tar_id;
};

struct TaskInfo {
  Count task_id;
  DataSize offset;
  DataSize size;
  DataSize piece_size;
  BwType bandwidth;
  Count node_num;
  std::unique_ptr<NodeTask[]> node_tasks;
};

/* Load tasks which do not need further scheduling */
class TaskReader : public TaskGetterInterface
{
 public:
  TaskReader(const Path &path);
  ~TaskReader();

  Count GetNextGroupNumber() override;
  Count GetTaskNumber(const Count &gid) override;
  void FillTask(const Count &gid, const Count &tid, const Count &node_id,
                RepairTask &rt, Count *src_ids) override;
  BwType get_capacity() override;
  Count GetRid() override;

  //TaskReader is neither copyable nor movable
  TaskReader(const TaskReader&) = delete;
  TaskReader& operator=(const TaskReader&) = delete;

 private:
  Path path_;
  std::ifstream task_file_;
  Count group_num_;
  Count cur_num_;

  Count task_num_;
  std::unique_ptr<TaskInfo[]> task_infos_;
  BwType capacity_;

  void Close();
};

} // namespace exr

#endif // EXR_TASK_TASKREADER_HH_

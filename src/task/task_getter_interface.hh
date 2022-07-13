#ifndef EXR_TASK_TASKGETTERINTERFACE_HH_
#define EXR_TASK_TASKGETTERINTERFACE_HH_

#include "util/typedef.hh"
#include "util/types.hh"

namespace exr {

const Count kMaxGroupNum = 1000;

/* A interface that can get repair task groups from it */
class TaskGetterInterface
{
 public:
  //Interfaces
  //Load or calculate the next group of tasks
  //    return the group number, kMaxGroupNum if no more tasks
  virtual Count GetNextGroupNumber() = 0;
  //Get task number of a group
  virtual Count GetTaskNumber(const Count &gid) = 0;
  //Get each task and node's content
  virtual void FillTask(const Count &gid, const Count &tid,
                        const Count &node_id,
                        RepairTask &rt, Count *src_ids) = 0;
  //Get capacity of the result
  virtual BwType get_capacity() = 0;
  //Get rid
  virtual Count GetRid() = 0;

  //Virtual Destructor
  virtual ~TaskGetterInterface() {}
};

} // namespace exr

#endif // EXR_TASK_TASKGETTERINTERFACE_HH_

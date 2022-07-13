#ifndef EXR_TASK_CONTROLLER_HH_
#define EXR_TASK_CONTROLLER_HH_

#include <memory>
#include <mutex>
#include <vector>

#include "data/access/access_center.hh"
#include "task/task_getter_interface.hh"
#include "util/typedef.hh"

namespace exr {

/* Control all the repair work like arranging routes and sending tasks */
class Controller
{
 public:
  Controller(const Count &total,
             const DataSize &size, const DataSize &psize);
  ~Controller();

  void Connect(const IPAddressList &ip_addresses);
  void ChangeAlg(const Alg &alg, const Count *args, const Path &path);

  bool GetTasks();
  BwType GetCapacity();
  Count DoTaskGroups(const Count &total);
  void Close(const Count &total);

  void ReloadNodeBandwidth(const Count &total);
  void SetNewNodeBandwidth(const Count &total);

  //Controller is neither copyable nor movable
  Controller(const Controller&) = delete;
  Controller& operator=(const Controller&) = delete;

 private:
  DataSize size_;
  DataSize psize_;
  AccessCenter ac_;
  using pTaskGetter = std::unique_ptr<TaskGetterInterface>;
  pTaskGetter ptg_;

  Count cur_tid_;
  Count gnum_;
  Count task_num_;
  std::unique_ptr<std::unique_ptr<Count[]>[]> src_lists_;
  std::vector<Count> waits_;
  std::mutex mtx_;

  void DeliverTasks_(const Count &gid, const Count &nid);
  void WaitForFinish_();
};

} // namespace exr

#endif // EXR_TASK_CONTROLLER_HH_

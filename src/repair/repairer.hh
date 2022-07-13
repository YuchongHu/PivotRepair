#ifndef EXR_REPAIR_REPAIRER_HH_
#define EXR_REPAIR_REPAIRER_HH_

#include <memory>
#include <mutex>
#include <thread>

#include "config/bandwidth_solver.hh"
#include "data/access/access_center.hh"
#include "repair/procs/compute_processor.hh"
#include "repair/procs/receive_processor.hh"
#include "repair/procs/proceed_processor.hh"
#include "util/memory_pool.hh"
#include "util/typedef.hh"
#include "util/types.hh"

namespace exr {

/* A class that receive tasks from master and repairing */
class Repairer
{
 public:
  Repairer(const Count &id, const Count &total,
           const Path &load_path, const Path &store_path,
           const Count &block_num, const DataSize &size,
           const Path &bandwidth_path, const Name &eth_name,
           const bool &if_print, const Count &recv_thr_num,
           const Count &comp_thr_num, const Count &proc_thr_num);
  ~Repairer();

  //Connect to other nodes and prepare for repairing
  void Prepare(const IPAddressList &ip_addresses);
  //Wait Master to send close signal and wait for the repairer to be closed
  void WaitForFinish();

  //Repairer is neither copyable nor movable
  Repairer(const Repairer&) = delete;
  Repairer& operator=(const Repairer&) = delete;

 private:
  Count id_;
  AccessCenter ac_;
  MemoryPool mp_;
  ProceedProcessor proceeder_;
  ComputeProcessor computer_;
  ReceiveProcessor receiver_;

  BandwidthSolver bs_;
  Path bandwidth_path_;

  bool on_run_;
  std::mutex mtx_;
  std::thread task_getter_;
  void GetTaks();
};

} // namespace exr

#endif // EXR_REPAIR_REPAIRER_HH_

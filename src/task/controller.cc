#include "task/controller.hh"

#include <sys/time.h>
#include <thread>

#include "task/algorithm/best_flow.hh"
#include "task/algorithm/eva_pipe.hh"
#include "task/algorithm/ftp_repair.hh"
#include "task/algorithm/ppr.hh"
#include "task/task_reader.hh"
#include "util/types.hh"

namespace exr {

Controller::Controller(const Count &total,
                       const DataSize &size, const DataSize &psize)
    : size_(size), psize_(psize), ac_(0, total), ptg_(nullptr),
      cur_tid_(0), gnum_(0), task_num_(0) {
  src_lists_ = std::make_unique<std::unique_ptr<Count[]>[]>(total - 1);
  for (Count i = 0; i < total - 1; ++i)
    src_lists_[i] = std::make_unique<Count[]>(total - 2);
}

Controller::~Controller() = default;

void Controller::Connect(const IPAddressList &ip_addresses) {
  ac_.Connect(ip_addresses);
}

void Controller::ChangeAlg(const Alg &alg, const Count *args,
                           const Path &path) {
  if (alg == 't') {
    ptg_ = pTaskGetter(new TaskReader(path));
  } else if (alg == 'b') {
    ptg_ = pTaskGetter(new BestFlow(
            args[0], args[1], args[2], args[3] == 1, args[4] * 1000, path));
  } else if (alg == 'v') {
    ptg_ = pTaskGetter(new EvaPipe(
            args[0], args[1], args[2], args[3], path));
  } else if (alg == 'j') {
    ptg_ = pTaskGetter(new PPR(
            args[0], args[1], args[2], args[3] * 1000, path));
  } else {
    ptg_ = pTaskGetter(new FTPRepair(
            args[0], args[1], args[2], alg, args[3] * 1000, path));
  }
}

//Calculate or load the path
bool Controller::GetTasks() {
  gnum_ = ptg_->GetNextGroupNumber();
  if (gnum_ == kMaxGroupNum) {
    return false;
  }
  return true;
}

BwType Controller::GetCapacity() { return ptg_->get_capacity(); }

Count Controller::DoTaskGroups(const Count &total) {
  Count max_task_num = 0;
  auto t = std::make_unique<std::thread[]>(total - 1);
  for (Count i = 0; i < gnum_; ++i) {
    task_num_ = ptg_->GetTaskNumber(i);
    //Send tasks of one group
    for (Count j = 1; j < total; ++j)
      t[j-1] = std::thread([&, i, j] { DeliverTasks_(i, j); });
    for (Count j = 0; j < total - 1; ++j) t[j].join();
    //Wait for finishing
    if (task_num_ > max_task_num) max_task_num = task_num_;
    WaitForFinish_();
  }
  return max_task_num;
}

void Controller::Close(const Count &total) {
  RepairTask end_task{0, 0, 0, 0, 0, 0, 0, 0};
  for (Count i = 1; i < total; ++i)
    ac_.Send(i, sizeof(end_task), &end_task);
}

void Controller::ReloadNodeBandwidth(const Count &total) {
  RepairTask reload_info{0, 0, 0, 1, 0, 1, 0, 0};
  for (Count i = 1; i < total; ++i)
    ac_.Send(i, sizeof(reload_info), &reload_info);
  Count r;
  for (Count i = 1; i < total; ++i) ac_.Receive(i, sizeof(r), &r);
}

void Controller::SetNewNodeBandwidth(const Count &total) {
  RepairTask set_new_info{0, 0, 0, 0, 0, 1, 0, 1};
  for (Count i = 1; i < total; ++i) {
    if (i == ptg_->GetRid()) set_new_info.bandwidth = 0;
    ac_.Send(i, sizeof(set_new_info), &set_new_info);
    set_new_info.bandwidth = 1;
  }
  Count r;
  for (Count i = 1; i < total; ++i) ac_.Receive(i, sizeof(r), &r);
}

void Controller::DeliverTasks_(const Count &gid, const Count &nid){
  auto &srcs = src_lists_[nid - 1];
  for (Count j = 0, tid = cur_tid_; j < task_num_; ++j, ++tid) {
    //Get task's content
    RepairTask rt{tid, 0, 0, 0, size_, psize_, 1, 0};
    ptg_->FillTask(gid, j, nid, rt, srcs.get());

    //Send to the node
    if (rt.size > 0) {
      ac_.Send(nid, sizeof(rt), &rt);
      for (Count k = 0; k < rt.src_num; ++k)
        ac_.Send(nid, sizeof(srcs[k]), &(srcs[k]));
      if (rt.tar_id == nid) {
        std::unique_lock<std::mutex> lck(mtx_);
        waits_.push_back(nid);
        lck.unlock();
      }
    }
  }
}

void Controller::WaitForFinish_() {
  //Wait for finish
  Count recv;
  for (auto &x: waits_)
    ac_.Receive(x, sizeof(recv), &recv);
  cur_tid_ += task_num_;
  waits_.clear();
}

} // namespace exr

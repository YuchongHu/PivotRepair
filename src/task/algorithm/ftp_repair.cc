#include "task/algorithm/ftp_repair.hh"

namespace exr {

//Constructor and destructor
FTPRepair::FTPRepair(const Count &k, const Count &num, const Count &rid,
                     const Alg &alg, const BwType &min_bw,
                     const Path &bw_path)
    : RouteCalculator(rid, bw_path), alg_(alg), num_(num), min_bw_(min_bw),
      rid_(rid), ptb_(new TreeBuilder(k, num - k)) {}

FTPRepair::~FTPRepair() = default;

//Fill the information
Count FTPRepair::GetTaskNumber(const Count &gid) {
  return gid == 0 ? 1 : 0;
}

void FTPRepair::FillTask(const Count &gid, const Count &tid,
                         const Count &node_id,
                         RepairTask &rt, Count *src_ids) {
  if (node_id > num_ || gid > 0) {
    rt.size = 0;
    return;
  }
  //Check if is chosen
  Count nid = node_id == rid_ ? 0 : node_id;
  if (nid != 0 && !(ptb_->selected[nid])) {
    rt.size = 0;
    return;
  }

  //Fill the target and sources
  rt.tar_id = nid == 0 ? rid_ : ptb_->nodes[nid].father->node_index;
  if (rt.tar_id == 0) rt.tar_id = 1;
  rt.src_num = 0;
  for (Count i = 1; i <= num_; ++i)
    if (ptb_->selected[i] && ptb_->nodes[i].father->node_index == nid)
      src_ids[(rt.src_num)++] = i;
  rt.bandwidth = capacity_;
}

BwType FTPRepair::get_capacity() { return capacity_; }

//Calculate and get the repair route
Count FTPRepair::CalculateRoute(const Bandwidth *bws, const Count &rid) {
  //Set the bandwidth to the tree_builder
  auto up_src = std::make_unique<double[]>(num_);
  auto down_src = std::make_unique<double[]>(num_);
  for (Count i = 0; i < num_; ++i) {
    up_src[i] = bws[i].upload;
    down_src[i] = bws[i].download;
    if (i != rid - 1 && (up_src[i] < min_bw_ || down_src[i] < min_bw_ )) {
      up_src[i] = 0;
      down_src[i] = 0;
    }
  }
  ptb_->set_bandwidth(up_src.get(), down_src.get());

  //Calculate the route
  double result = 0;
  if (alg_ == 'f')
    result = ptb_->build_repairing_tree(rid);
  else if (alg_ == 'r')
    result = ptb_->build_repair_pipeline(rid);
  else if (alg_ == 'p')
    result = ptb_->find_best_ppt_tree(rid);
  capacity_ = result;

  return capacity_ >= min_bw_ ? 1 : 0;
}

} // namespace exr

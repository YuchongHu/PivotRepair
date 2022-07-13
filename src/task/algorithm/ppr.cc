#include "task/algorithm/ppr.hh"

#include <algorithm>
#include <cstring>

namespace exr {

PPR::PPR(const Count &k, const Count &n, const Count &rid,
         const BwType &min_bw, const Path &path)
    : RouteCalculator(rid, path), n_(n), k_(k),
      min_bw_(min_bw), capacity_(0) {}
PPR::~PPR() = default;

//Fill the information
Count PPR::GetTaskNumber(const Count &gid) {
  return gid < task_groups_.size() ? 1 : 0;
}

void PPR::FillTask(const Count &gid, const Count &tid,
                   const Count &node_id, RepairTask &rt, Count *src_ids) {
  //Check if is chosen
  Count nid = node_id - 1;
  if (node_id > n_ || gid >= task_groups_.size() || tid > 0 ||
      task_groups_[gid][nid] == n_) {
    rt.size = 0;
    return;
  }
  rt.bandwidth = 0;

  //Fill the target and source
  rt.tar_id = task_groups_[gid][nid] + 1;
  if (rt.tar_id != node_id) {
    rt.src_num = 0;
  } else {
    for (Count i = 0; i < n_; ++i) {
      if (i != nid && task_groups_[gid][i] == nid) {
        src_ids[(rt.src_num)++] = i + 1;
        break;
      }
    }
  }
}

BwType PPR::get_capacity() { return capacity_; }

//Calculate and get the repair route
Count PPR::CalculateRoute(const Bandwidth *bws, const Count &rid) {
  //Init for calculation
  task_groups_.clear();
  std::vector<Count> unselected;
  auto cbws = std::make_unique<Bandwidth[]>(n_);
  auto min_uds = std::make_unique<BwType[]>(n_);
  for (Count i = 0; i < n_; ++i) {
    min_uds[i] = bws[i].upload > bws[i].download ? bws[i].download :
                                                   bws[i].upload;
    cbws[i].upload = bws[i].upload;
    cbws[i].download = bws[i].download;
    if (i != rid - 1) {
      unselected.push_back(i);
      if (min_uds[i] < min_bw_) {
        cbws[i].upload = 0;
        cbws[i].download = 0;
      }
    }
  }
  auto default_layer = std::make_unique<Count[]>(n_);
  for (Count i = 0; i < n_; ++i) default_layer[i] = n_;
  double min_cap = 0, sum_pac = 0;
  //Select best k/2 inter nodes
  std::vector<Count> remains;
  std::sort(unselected.begin(), unselected.end(),
            [&](const Count &i, const Count &j) {
    if (min_uds[i] == min_uds[j]) return i > j;
    return min_uds[i] < min_uds[j];
  });
  for (Count i = 0; i < k_ / 2; ++i) {
    remains.push_back(unselected.back());
    unselected.pop_back();
  }
  remains.push_back(rid - 1);
  //Select best (k+1)/2 start nodes
  auto first_layer = std::make_unique<Count[]>(n_);
  std::memcpy(first_layer.get(), default_layer.get(), sizeof(Count) * n_);
  std::sort(unselected.begin(), unselected.end(),
            [&](const Count &i, const Count &j) {
    if (cbws[i].upload == cbws[j].upload) return i < j;
    return cbws[i].upload > cbws[j].upload;
  });
  min_cap = cbws[unselected[0]].upload;
  for (Count i = 0; i < (k_ + 1) / 2; ++i) {
    first_layer[unselected[i]] = remains[i];
    first_layer[remains[i]] = remains[i];
    if (cbws[unselected[i]].upload < min_cap)
      min_cap = cbws[unselected[i]].upload;
    if (cbws[remains[i]].download < min_cap)
      min_cap = cbws[remains[i]].download;
  }
  if (min_cap == 0) {
    task_groups_.clear();
    capacity_ = 0;
    return 0;
  }
  sum_pac += 1 / min_cap;
  task_groups_.push_back(std::move(first_layer));
  //Finish the remain nodes
  while (remains.size() > 1) {
    auto new_layer = std::make_unique<Count[]>(n_);
    std::memcpy(new_layer.get(), default_layer.get(), sizeof(Count) * n_);
    Count i;
    min_cap = cbws[remains[0]].upload;
    for (i = 1; i < remains.size(); i += 2) {
      new_layer[remains[i - 1]] = remains[i];
      new_layer[remains[i]] = remains[i];
      if (cbws[remains[i - 1]].upload < min_cap)
        min_cap = cbws[remains[i - 1]].upload;
      if (cbws[remains[i]].download < min_cap)
        min_cap = cbws[remains[i]].download;
    }
    if (min_cap == 0) {
      task_groups_.clear();
      capacity_ = 0;
      return 0;
    }
    sum_pac += 1 / min_cap;
    for (; i > 2; i -= 2) {
      remains.erase(remains.begin() + (i - 3));
    }
    task_groups_.push_back(std::move(new_layer));
  }
  capacity_ = 1 / sum_pac;
  return task_groups_.size();
}

} // namespace exr

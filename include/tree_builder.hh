#ifndef FTPR_TREE_BUILDER_HH
#define FTPR_TREE_BUILDER_HH

#include <algorithm>
#include <memory>
#include <vector>
#include <string>

#include "bandwidth_info.hh"

#define EPS 1e-5

class TreeNode
{
public:
  int node_index = -1;
  double upload = 0.0;
  double download = 0.0;
  double theoretical_bw = 0.0;
  double if_insert = 0.0;
  double avg_downlink = 0.0;
  std::vector<TreeNode *> children;
  TreeNode *father = nullptr;
  int sibling_index = -1;

  TreeNode() = default;
  TreeNode(int node_index, double up, double down);
  ~TreeNode() = default;

  void add_child(TreeNode *new_child);
  void update_child(TreeNode *new_child, int child_index);
  void attach_father(TreeNode *father, int sibling_index);
  bool is_leaf();
};

class TreeBuilder
{
private:
  int rs_n_;
  int rs_k_;
  BandwidthInfo *nodes_bw_;

  int pow(int x, int y);
  bool build_ppt_tree(double limit, std::vector<int> &last_ly);
  bool build_ppt_tree(double limit, std::vector<int> &last_ly, std::vector<int> &new_ly);
  void extend_repair_pipeline(int *p, int *op_p, int count, double &global_min, double cur_min);

public:
  TreeNode *nodes;
  bool *selected;
  TreeBuilder(int rs_k, int rs_m);
  ~TreeBuilder();

  void set_bandwidth(char *upload_raw, char *download_raw);
  void set_bandwidth(double *upload_src, double *download_src);
  void load_bandwidth(const std::string &bw_addr);
  double build_repairing_tree(int fail_node);
  double find_best_ppt_tree(int fail_node);
  double build_repair_pipeline(int fail_node);
};

#endif
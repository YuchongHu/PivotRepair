#include "task/algorithm/old_alg/tree_builder.hh"

#include <iostream>
#include <cstring>
#include <queue>
#include <unordered_set>

namespace exr {

TreeNode::TreeNode(int node, double up, double down)
    : node_index(node), upload(up), download(down)
{
  if_insert = theoretical_bw = std::min(upload, download);
}

void TreeNode::add_child(TreeNode *new_child)
{
  if (new_child) {
    children.push_back(new_child);
    avg_downlink = upload > if_insert ? if_insert : download / children.size();
    double if_ins_avg_down = download / (children.size() + 1);
    if_insert = std::min(upload, if_ins_avg_down);
    new_child->attach_father(this, children.size() - 1);
  }
}

void TreeNode::update_child(TreeNode *new_child, int child_index)
{
  if (new_child && child_index >= 0 && child_index < children.size()) {
    TreeNode *old_child = children[child_index];
    old_child->father = nullptr;
    old_child->sibling_index = -1;
    children[child_index] = new_child;
    new_child->attach_father(this, child_index);
  }
}

void TreeNode::attach_father(TreeNode *f, int sib_index)
{
  father = f;
  sibling_index = sib_index;
}

bool TreeNode::is_leaf() { return father && !children.size(); }

int TreeBuilder::pow(int x, int y)
{
  int result = 1;
  for (int i = 0; i < y; i++)
  {
    result *= x;
  }
  return result;
}

TreeBuilder::TreeBuilder(int rs_k, int rs_m) : rs_n_(rs_m + rs_k), rs_k_(rs_k)
{
  nodes_bw_ = new BandwidthInfo(rs_n_);
  nodes = new TreeNode[rs_n_ + 1];
  selected = new bool[rs_n_ + 1];
}

TreeBuilder::~TreeBuilder()
{
  if (nodes_bw_) {
    delete nodes_bw_;
  }
  if (nodes) {
    delete[] nodes;
  }
  if (selected) {
    delete selected;
  }
}

void TreeBuilder::set_bandwidth(char *upload_raw, char *download_raw)
{
  nodes_bw_->get_bandwidth(upload_raw, download_raw);
}

void TreeBuilder::set_bandwidth(double *upload_src, double *download_src)
{
  nodes_bw_->copy_bandwidth(upload_src, download_src);
}

void TreeBuilder::load_bandwidth(const std::string &bw_addr)
{
  nodes_bw_->load_bandwidth(bw_addr);
}

inline bool double_equals(const double &a, const double &b)
{
  return std::abs(a - b) < EPS;
}

// for max heap
struct if_insert_cmp
{
  // whether should go down
  bool operator()(TreeNode *a, TreeNode *b)
  {
    return a->if_insert < b->if_insert ||
           (double_equals(a->if_insert, b->if_insert) &&
               a->avg_downlink < b->avg_downlink);
  }
};

/*
  use our algorithm to build repairing tree
 */
double TreeBuilder::build_repairing_tree(int fail_node)
{
  memset(selected, false, (rs_n_ + 1) * sizeof(bool));
  int helpers_num = rs_n_ - 1;
  int candidate[helpers_num];
  int i, j;
  // preparation
  for (i = j = 0; i <= rs_n_; ++i)
  {
    // requestor | n-1 potential helpers
    if (i == fail_node) {
      j = 1;
      continue;
    } else if (i) {
      // only helpers, without requestor
      candidate[i - 1 - j] = i;
    }
    nodes[i] = TreeNode(i, nodes_bw_->upload[i], nodes_bw_->download[i]);
  }

  /*
    1. sort by desc order of theoretical_bw (min {ul, dl})
   */
  std::sort(candidate, candidate + helpers_num, [&](int i, int j) {
    if (double_equals(nodes[i].theoretical_bw, nodes[j].theoretical_bw)) {
      if (nodes[i].download > nodes[j].download) {
        return true;
      } else if (double_equals(nodes[i].download, nodes[j].download)) {
        return nodes[i].upload > nodes[j].upload;
      }
      return false;
    }
    return nodes[i].theoretical_bw > nodes[j].theoretical_bw;
  });

  /*
    2. insert top rs_k candidates to the tree, reaching max B_nonleaf
   */
  std::priority_queue<TreeNode *, std::vector<TreeNode *>, if_insert_cmp>
      nonleaf_queue;
  nonleaf_queue.push(nodes); // init priority_queue with requestor

  double min_non_leaf = 1e8;
  int max_leaf_index = 0;
  for (i = 0; i < rs_k_; ++i)
  {
    TreeNode *father_to_insert = nullptr, *max_nonleaf = nonleaf_queue.top();
    int max_leaf = candidate[max_leaf_index];
    if (!i || max_nonleaf->if_insert >= nodes[max_leaf].if_insert) {
      nonleaf_queue.pop();
      father_to_insert = max_nonleaf;
    } else {
      father_to_insert = nodes + max_leaf;
      ++max_leaf_index;
    }
    if (min_non_leaf > father_to_insert->if_insert) {
      min_non_leaf = father_to_insert->if_insert;
    }
    int new_node_index = candidate[i];
    father_to_insert->add_child(nodes + new_node_index);
    nonleaf_queue.push(father_to_insert);
    selected[new_node_index] = true;
  }

  /*
    3. replace weak leafnodes with strong leafnodes
   */
  int *leaf_nodes = candidate + max_leaf_index;
  int leaf_num = rs_k_ - max_leaf_index;
  std::unordered_set<int> weak_leafnodes(leaf_nodes, leaf_nodes + leaf_num);
  // sort to get strong leafnodes
  std::sort(leaf_nodes, candidate + helpers_num,
            [&](int i, int j) { return nodes[i].upload > nodes[j].upload; });
  // get real weak leafnodes and unused strong nodes
  std::queue<int> unused_strong_nodes;
  for (i = 0; i < leaf_num; ++i)
  {
    j = leaf_nodes[i];
    if (selected[j]) {
      weak_leafnodes.erase(j);
    } else {
      unused_strong_nodes.push(j);
    }
  }
  // replace weak leafnodes
  for (int w : weak_leafnodes)
  {
    selected[w] = false;
    int sib_index = nodes[w].sibling_index;
    int new_leaf_index = unused_strong_nodes.front();
    TreeNode *father = nodes[w].father;
    TreeNode *new_leaf = nodes + new_leaf_index;
    unused_strong_nodes.pop();
    father->update_child(new_leaf, sib_index);
    selected[new_leaf_index] = true;
  }
  return std::min(min_non_leaf, nodes[leaf_nodes[leaf_num - 1]].upload);
}

double TreeBuilder::find_best_ppt_tree(int fail_node)
{
  double lower, upper;
  lower = BW_CEILING;
  upper = 0;
  //getting lower bandwith and upper bandwith
  for (int i = 0; i <= rs_n_; ++i)
  {
    for (int j = 1; j <= rs_n_; ++j)
    {
      if (i == fail_node || j == fail_node || i == j) {
        continue;
      }
      if (nodes_bw_->matrix[j][i] < lower) {
        lower = nodes_bw_->matrix[j][i];
      } else if (nodes_bw_->matrix[j][i] > upper) {
        upper = nodes_bw_->matrix[j][i];
      }
    }
    nodes[i] = TreeNode(i, 0, 0);
    selected[i] = false;
  }

  //initialize
  std::vector<int> origin;
  origin.push_back(0);
  selected[0] = true;
  selected[fail_node] = true;

  //trying limits that can create a tree
  while (upper - lower > EPS)
  {
    //try the limit
    double mid = (upper + lower) / 2;
    if (build_ppt_tree(mid, origin)) {
      lower = mid;
    } else {
      upper = mid;
    }

    //clearing
    for (int i = 1; i <= rs_n_; ++i)
    {
      selected[i] = (i == fail_node);
      nodes[i].father = nullptr;
      nodes[i].children.clear();
    }
    nodes[0].children.clear();
  }

  //get the final tree
  build_ppt_tree(lower, origin);
  selected[fail_node] = false;
  return lower;
}

bool TreeBuilder::build_ppt_tree(double limit, std::vector<int> &last_ly)
{
  //get the number of the nodes which is not used
  std::vector<int> not_used;
  for (int i = 0; i <= rs_n_; ++i)
  {
    if (!selected[i]) {
      not_used.push_back(i);
    }
  }
  //check if compelete the tree
  if (rs_k_ + 2 - (rs_n_ + 1 - not_used.size()) == 0) {
    return true;
  }

  //get all possibilities of the next layer
  for (int i = 1; i < (1 << not_used.size()); ++i)
  {
    //create the new layer
    std::vector<int> new_ly;
    for (int k = 0; k < not_used.size(); ++k)
    {
      if (((1 << k) & i) > 0) {
        new_ly.push_back(not_used[k]);
        selected[not_used[k]] = true;
      }
    }

    //matching the new layer to the last layer if the number is legal
    if ((rs_n_ + 1 - not_used.size() + new_ly.size() <= rs_k_ + 2) &&
        build_ppt_tree(limit, last_ly, new_ly))
    {
      return true;
    }

    //clearing
    for (auto it = new_ly.begin(); it != new_ly.end(); ++it)
    {
      selected[*it] = false;
    }
  }

  return false;
}

bool TreeBuilder::build_ppt_tree(double limit, std::vector<int> &last_ly, std::vector<int> &new_ly)
{
  //creating all match possibilities between the new layer and the last layer
  for (int i = 0; i < pow(last_ly.size(), new_ly.size()); ++i)
  {
    bool flag = true;
    //link one of the new nodes to one of the nodes from the last layer
    for (int k = 0; k < new_ly.size(); ++k)
    {
      int pos = (i / pow(last_ly.size(), k)) % last_ly.size();
      //if the bandwith is lower than the limit, skip
      if (nodes_bw_->matrix[new_ly[k]][last_ly[pos]] / (nodes[last_ly[pos]].children.size() + 1) < limit) {
        flag = false;
        break;
      }
      //link
      nodes[last_ly[pos]].children.push_back(nodes + new_ly[k]);
      nodes[new_ly[k]].father = nodes + last_ly[pos];
    }

    //build tree from the new layer if it is compeletly matched
    if (flag && build_ppt_tree(limit, new_ly)) {
      return true;
    }

    //unlinking
    for (auto it = last_ly.begin(); it != last_ly.end(); ++it)
    {
      nodes[*it].children.clear();
    }
    for (auto it = new_ly.begin(); it != new_ly.end(); ++it)
    {
      nodes[*it].father = nullptr;
    }
  }
  return false;
}

double TreeBuilder::build_repair_pipeline(int fail_node)
{
  int p[rs_n_ + 1], op_p[rs_n_ + 1];
  double global_min = 0;

  //initialize
  memset(p + 1, -1, rs_n_ * sizeof(int));
  p[0] = 0;
  memset(selected + 1, false, rs_n_ * sizeof(bool));
  selected[0] = true;
  for (int i = 0; i <= rs_n_; i++)
  {
    nodes[i] = TreeNode(i, 0, 0);
  }

  //find path
  selected[fail_node] = true;
  extend_repair_pipeline(p, op_p, 0, global_min, BW_CEILING);
  selected[fail_node] = false;

  //link
  if (global_min > 0) {
    memset(selected + 1, false, rs_n_ * sizeof(bool));
    for (int i = 0; i < rs_k_; i++)
    {
      nodes[op_p[i]].children.push_back(nodes + op_p[i + 1]);
      nodes[op_p[i + 1]].father = nodes + op_p[i];
      selected[op_p[i + 1]] = true;
    }
  }
  return global_min;
}

void TreeBuilder::extend_repair_pipeline(int *p, int *op_p, int count, double &global_min, double cur_min)
{
  if (count == rs_k_) {
    memcpy(op_p, p, sizeof(int) * (rs_n_ + 1));
    global_min = cur_min;
  } else {
    for (int i = 1; i <= rs_n_; ++i)
    {
      if (!selected[i]) {
        double new_cur_min = nodes_bw_->matrix[i][p[count]];
        if (new_cur_min > cur_min) {
          new_cur_min = cur_min;
        }

        //early stop
        if (new_cur_min <= global_min) {
          continue;
        }

        p[count + 1] = i;
        selected[i] = true;
        //further calculation
        extend_repair_pipeline(p, op_p, count + 1, global_min, new_cur_min);
        //restore
        selected[i] = false;
      }
    }
  }
}

void TreeBuilder::set_rdownload(double rdownload)
{
  nodes_bw_->set_rdownload(rdownload);
}

} // namespace exr

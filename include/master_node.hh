#ifndef FTPR_MASTER_NODE_HH
#define FTPR_MASTER_NODE_HH

#include <memory>
#include <string>

#include "compute_tool.hh"
#include "tree_builder.hh"

#include "connection_solver.hh"
#include "address_handler.hh"

class MasterNode
{
private:
  uint16_t k, m;

  ComputeTool *pct;
  TreeBuilder *ptb;

  std::unique_ptr<ConnectionSolver *[]> nodes;
  std::unique_ptr<node_address[]> naddrs;

  int *helper_list;
  uint8_t *decode_matrix;

public:
  MasterNode(const std::string &aconf_addr);
  ~MasterNode();

  void set_bandwidth(double *up_src, double *down_src);
  void load_bandwidth(const std::string &bw_addr);
  double build_repair_path(int fail_node, char alg);

  void start_repairing(uint16_t task_id, int *frag_err_list, int err_cnt, ssize_t _size, ssize_t _psize);
  void close();
};

#endif

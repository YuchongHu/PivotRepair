#include "master_node.hh"

#include <memory>

#include "tasks.hh"

MasterNode::MasterNode(const std::string &aconf_addr)
{
  //get address info
  AddressHandler *pah = new AddressHandler(aconf_addr);
  k = pah->get_k();
  m = pah->get_m();

  //about computing and routing
  pct = new ComputeTool(0, k, m);
  ptb = new TreeBuilder(k, m);

  //connecting
  nodes = std::unique_ptr<ConnectionSolver *[]>(new ConnectionSolver *[k + m]);

  uint16_t ui = 0;
  naddrs = pah->get_node_addresses();
  for (uint16_t i = 0; i < k + m; ++i)
  {
    nodes[i] = new ConnectionSolver(naddrs[i + 1].host, naddrs[i + 1].port);
    nodes[i]->write(&ui, sizeof(ui));
  }

  //waiting for nodes to be ready
  for (uint16_t i = 0; i < k + m; ++i)
  {
    nodes[i]->read(&ui, sizeof(ui));
  }

  //clearing
  delete pah;
}

MasterNode::~MasterNode()
{
  delete pct;
  delete ptb;
}

void MasterNode::set_bandwidth(double *up_src, double *down_src)
{
  ptb->set_bandwidth(up_src, down_src);
}

void MasterNode::load_bandwidth(const std::string &bw_addr)
{
  ptb->load_bandwidth(bw_addr);
}

double MasterNode::build_repair_path(int fail_node, char alg)
{
  if (alg == 'f') {
    //ftp repair
    return ptb->build_repairing_tree(fail_node);
  } else if (alg == 'r') {
    //repair pipeline
    return ptb->build_repair_pipeline(fail_node);
  } else if (alg == 'p') {
    //ppt
    return ptb->find_best_ppt_tree(fail_node);
  }

  std::cerr << "wrong repair path alg" << std::endl;
  exit(-1);
}

void MasterNode::start_repairing(uint16_t task_id, int *frag_err_list, int err_cnt, ssize_t _size, ssize_t _psize)
{
  uint16_t ui = 0;
  int *err_list = new int[err_cnt];
  for (int i = 0; i < err_cnt; ++i)
  {
    err_list[i] = frag_err_list[i];
  }

  //calculate decode matrix
  int *helper_list = new int[k];
  for (uint16_t i = 1; i <= k + m; ++i)
  {
    if (ptb->selected[i]) {
      helper_list[ui] = i;
      ui++;
    }
  }
  decode_matrix = new uint8_t[k * m];
  pct->get_decode_matrix(helper_list, err_list, err_cnt, decode_matrix);

  //send tasks to the selected helpers
  ntask task;
	task.id = task_id;
  task.size = _size;
  task.psize = _psize;
  for (uint16_t i = 0; i < k; ++i)
  {
    TreeNode *node = &(ptb->nodes[helper_list[i]]);

    task.tid = node->father->node_index;
    if (task.tid == 0) {
      task.tid = frag_err_list[0];
    }
    task.coef = decode_matrix[k * 0 + i];

    task.sid_num = node->children.size();
    nodes[node->node_index - 1]->write(&task, sizeof(task));

    for (uint16_t j = 0; j < task.sid_num; ++j)
    {
      ui = node->children[j]->node_index;
      nodes[node->node_index - 1]->write(&ui, sizeof(ui));
    }
  }

  //send message to the requestor
  task.tid = frag_err_list[0];
  task.sid_num = ptb->nodes[0].children.size();
  nodes[task.tid - 1]->write(&task, sizeof(task));

  for (uint16_t i = 0; i < task.sid_num; ++i)
  {
    ui = ptb->nodes[0].children[i]->node_index;
    nodes[task.tid - 1]->write(&ui, sizeof(ui));
  }

  //wait for finish
  nodes[frag_err_list[0] - 1]->read(&ui, sizeof(ui));

  //clearing
  delete[] helper_list;
  delete[] decode_matrix;
}

void MasterNode::close()
{
  //told nodes to close
  ntask etask;
  for (uint16_t i = 0; i < k + m; ++i)
  {
    nodes[i]->write(&etask, sizeof(etask));
  }
  uint16_t ui;
  for (uint16_t i = 0; i < k + m; ++i)
  {
    nodes[i]->read(&ui, sizeof(ui));
    delete nodes[i];
  }
}

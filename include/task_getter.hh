#ifndef FTPR_TASK_GETTER_HH
#define FTPR_TASK_GETTER_HH

#include <fstream>

#include "connection_solver.hh"

class TaskGetter
{
private:
  ConnectionSolver *pcs;
  std::string tfile_addr;
  std::ifstream tf;
  std::ofstream wdf;

  int err_cnt;
  int *frag_err_list;
  ssize_t rsize, psize;
  char build_alg;
  bool has_new_bw;
  char message;

  int remain;
  int repeat;
  void load_next();

public:
  TaskGetter(const std::string &taddr, uint16_t port, const std::string &_tfile_addr, const std::string &wdfile_addr);
  ~TaskGetter();

  bool loaded_new();

  int get_err_cnt();
  int *get_frag_err_list();
  ssize_t get_request_size();
  ssize_t get_piece_size();
  char get_build_alg();

  bool need_load_bw();

  void store_result(double global_min, double compute_time, double repair_time);
};

#endif
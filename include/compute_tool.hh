#ifndef FTPR_COMPUTE_TOOL_H
#define FTPR_COMPUTE_TOOL_H

#include <isa-l.h>

#include <unordered_map>

class ComputeTool
{
private:
  uint8_t *xor_gftbl_ = nullptr;
  uint8_t *encode_matrix_ = nullptr;
  uint8_t *invert_matrix_ = nullptr;
  uint8_t *temp_matrix_ = nullptr;
  int rs_k_;
  int rs_m_;
  int node_index_;
  std::unordered_map<int, uint8_t *> coef_map_;

  void init_xor_table();

public:
  ComputeTool(int node_id, int rs_k, int rs_m);
  ~ComputeTool();

  void mul_local(int task_id, uint8_t *data, int size);
  void xor_data(uint8_t *src, uint8_t *dst, int size);
  int get_decode_matrix(int *helper_list, int *frag_err_list, int nerrs,
                        uint8_t *decode_matrix);
  void add_task(int task_id, uint8_t coef);
  void release_task(int task_id);
};

#endif
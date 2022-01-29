#include "compute_tool.hh"

#include <cstring>
#include <iostream>

ComputeTool::ComputeTool(int node_id, int rs_k, int rs_m)
    : rs_k_(rs_k), rs_m_(rs_m), node_index_(node_id - 1)
{
  if (node_id > 0) {
    xor_gftbl_ = new uint8_t[64]();
    init_xor_table();
  } else {
    int rs_n = rs_k_ + rs_m_;
    int matrix_size = rs_k_ * rs_n;
    encode_matrix_ = new uint8_t[matrix_size];
    invert_matrix_ = new uint8_t[matrix_size];
    temp_matrix_ = new uint8_t[matrix_size];
    // generate default cauchy matrix
    gf_gen_cauchy1_matrix(encode_matrix_, rs_n, rs_k_);
  }
}

void ComputeTool::init_xor_table()
{
  uint8_t compute_matrix[2] = {1, 1};
  ec_init_tables(2, 1, compute_matrix, xor_gftbl_);
}

ComputeTool::~ComputeTool()
{
  if (xor_gftbl_) {
    delete[] xor_gftbl_;
  }
  if (encode_matrix_) {
    delete[] encode_matrix_;
  }
  if (invert_matrix_) {
    delete[] invert_matrix_;
  }
  if (temp_matrix_) {
    delete[] temp_matrix_;
  }
}

int ComputeTool::get_decode_matrix(int *helper_list, int *frag_err_list,
                                   int nerrs, uint8_t *decode_matrix)
{
  if (node_index_ >= 0) {
    std::cerr << "need to execute on master!" << std::endl;
    return -1;
  }

  int i, j, p, r;
  uint8_t s, *b = temp_matrix_;

  // Construct b (matrix that encoded remaining frags) by removing erased rows
  for (i = 0, r = 0; i < rs_k_; i++)
  {
    r = helper_list[i] - 1;
    for (j = 0; j < rs_k_; j++)
    {
      b[rs_k_ * i + j] = encode_matrix_[rs_k_ * r + j];
    }
  }

  // Invert matrix to get recovery matrix
  if (gf_invert_matrix(b, invert_matrix_, rs_k_) < 0) {
    return -1;
  }

  // Get decode matrix with only wanted recovery rows
  for (i = 0; i < nerrs; i++)
  {
    if (--frag_err_list[i] < rs_k_) {
      // A src err
      memcpy(decode_matrix + rs_k_ * i,
             invert_matrix_ + rs_k_ * frag_err_list[i], rs_k_);
    }
  }

  // For non-src (parity) erasures need to multiply encode matrix * invert
  for (p = 0; p < nerrs; p++)
  {
    if (frag_err_list[p] >= rs_k_) {
      // A parity err
      for (i = 0; i < rs_k_; i++)
      {
        s = 0;
        for (j = 0; j < rs_k_; j++)
        {
          s ^= gf_mul(invert_matrix_[j * rs_k_ + i],
                      encode_matrix_[rs_k_ * frag_err_list[p] + j]);
        }
        decode_matrix[rs_k_ * p + i] = s;
      }
    }
  }
  return 0;
}

void ComputeTool::mul_local(int task_id, uint8_t *data, int size)
{
  uint8_t *cur_gftbl = nullptr;
  auto it = coef_map_.find(task_id);
  if (it != coef_map_.end()) {
    cur_gftbl = it->second;
  } else {
		return;
    std::cerr << "get mul gftbl error" << std::endl;
    return;
  }
  gf_vect_mul(size, cur_gftbl, data, data);
}

void ComputeTool::xor_data(uint8_t *src, uint8_t *dst, int size)
{
  uint8_t *data[2] = {src, dst};
  uint8_t *target[1] = {dst};
  ec_encode_data(size, 2, 1, xor_gftbl_, data, target);
}

void ComputeTool::add_task(int task_id, uint8_t coef)
{
  auto it = coef_map_.find(task_id);
  if (it == coef_map_.end()) {
    uint8_t *gftbl = new uint8_t[32]();
    ec_init_tables(1, 1, &coef, gftbl);
    coef_map_[task_id] = gftbl;
  } else {
    std::cerr << "add task error" << std::endl;
  }
}

void ComputeTool::release_task(int task_id)
{
  auto it = coef_map_.find(task_id);
  if (it != coef_map_.end()) {
    delete[] it->second;
    coef_map_.erase(it);
  } else {
    std::cerr << "release task error" << std::endl;
  }
}

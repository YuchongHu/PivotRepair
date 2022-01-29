#ifndef FTRP_CALCULATOR_HH
#define FTRP_CALCULATOR_HH

#include <mutex>
#include <unordered_map>
#include <memory>

#include "tasks.hh"
#include "compute_tool.hh"

class Calculator
{
private:
  struct piece
  {
    uint16_t remain_num;
    char *buf;
    ssize_t size;
    std::mutex mtx;

    piece(uint16_t _remain_num, char *_buf, ssize_t _size)
        : remain_num(_remain_num), buf(_buf), size(_size){};
    piece() = default;
  };
  std::unordered_map<uint16_t, uint16_t> snum_map;
  std::mutex snum_map_mtx;
  std::unordered_map<uint16_t, std::unordered_map<uint16_t, piece*>*> pcp_map;
  std::mutex pcp_map_mtx;

  uint16_t id;
  ComputeTool *pct;

public:
  Calculator(uint16_t _id, uint16_t k, uint16_t m);
  ~Calculator();

  void mul_piece(uint16_t task_id, char* buf, ssize_t size);
  char* cal_piece(data_piece dp);

  void add_task(uint16_t task_id, uint8_t coef, uint16_t remain_num);
  void release_task(uint16_t task_id);
};

#endif

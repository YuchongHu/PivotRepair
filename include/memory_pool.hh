#ifndef FTPR_MEMORY_POOL_HH
#define FTPR_MEMORY_POOL_HH

#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <unordered_map>

class MemoryPool
{
private:
  uint16_t block_num;
  ssize_t block_size;

  std::unique_ptr<char *[]> pool;

  std::queue<uint16_t> free_queue;
  std::mutex queue_mtx;
  std::condition_variable queue_cv;

  std::unordered_map<char *, uint16_t> ptr_map;

public:
  MemoryPool(uint16_t n, ssize_t s);
  ~MemoryPool();

  char *get();
  void release(char *ptr);
  size_t get_remain();
};

#endif
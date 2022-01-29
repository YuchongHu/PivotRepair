#include "memory_pool.hh"

MemoryPool::MemoryPool(uint16_t _block_num, ssize_t _block_size) : block_num{_block_num}, block_size{_block_size}
{
  pool.reset(new char *[block_num]);
  for (uint16_t i = 0; i < block_num; ++i)
  {
    pool[i] = new char[block_size]();
    free_queue.push(i);
    ptr_map[pool[i]] = i;
  }
}

MemoryPool::~MemoryPool()
{
  for (uint16_t i = 0; i < block_num; ++i)
  {
    delete[] pool[i];
  }
}

char *MemoryPool::get()
{
  std::unique_lock<std::mutex> lck(queue_mtx);
  while (free_queue.empty())
  {
    queue_cv.wait(lck, [&] { return !free_queue.empty(); });
  }
  uint16_t i = free_queue.front();
  free_queue.pop();
  return pool[i];
}

void MemoryPool::release(char *ptr)
{
  auto it = ptr_map.find(ptr);
  if (it == ptr_map.end()) {
    std::cerr << "bad ptr in release_block\n";
    return;
  }
  std::unique_lock<std::mutex> lck(queue_mtx);
  free_queue.push(it->second);
  lck.unlock();
  queue_cv.notify_one();
}

size_t MemoryPool::get_remain()
{
  return free_queue.size();
}
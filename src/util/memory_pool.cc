#include "util/memory_pool.hh"

namespace exr {

//Constructor and destructor
MemoryPool::MemoryPool(const Count &num, const DataSize &size)
    : bufs_(new std::unique_ptr<BufUnit[]>[num]) {
  for (Count i = 0; i < num; ++i)
    bufs_[i] = std::make_unique<BufUnit[]>(size);
}

MemoryPool::~MemoryPool() = default;

BufUnit* MemoryPool::Get(const Count &id, const DataSize &offset) {
  return bufs_[id].get() + offset;
}

} // namespace exr

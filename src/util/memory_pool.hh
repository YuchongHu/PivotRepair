#ifndef EXR_UTIL_MEMORYPOOL_HH_
#define EXR_UTIL_MEMORYPOOL_HH_

#include <memory>

#include "util/typedef.hh"
#include "util/waiting_queue.hh"

namespace exr {

/* Allocate several memory bufs in advandce. Get them when needed */
class MemoryPool
{
 public:
  MemoryPool(const Count &num, const DataSize &size);
  ~MemoryPool();

  BufUnit* Get(const Count &id, const DataSize &offset);

  //MemoryPool is neither copyable nor movable
  MemoryPool(const MemoryPool&) = delete;
  MemoryPool& operator=(const MemoryPool&) = delete;

 private:
  std::unique_ptr<std::unique_ptr<BufUnit[]>[]> bufs_; //Memory units
};

} // namespace exr

#endif // EXR_UTIL_MEMORYPOOL_HH_

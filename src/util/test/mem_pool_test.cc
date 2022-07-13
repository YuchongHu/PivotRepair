#include <iostream>
#include <memory>
#include <thread>

#include "util/memory_pool.hh"
#include "util/typedef.hh"

int main()
{
  const exr::Count buf_num = 17;
  const exr::DataSize size = 1 << 26;
  //
  //Init
  std::cout << "Initiate -- allocate blocks" << std::endl;
  exr::MemoryPool mp(buf_num, size);
  std::cout << "Allocated " << buf_num << " blocks with size: " << size
            << std::endl;
  return 0;
}

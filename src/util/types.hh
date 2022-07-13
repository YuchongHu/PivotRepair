#ifndef EXR_UTIL_TYPES_HH_
#define EXR_UTIL_TYPES_HH_

#include <iostream>

#include "util/typedef.hh"

namespace exr {

struct RepairTask {
  Count task_id;
  Count src_num;
  Count tar_id;
  DataSize offset;      // BANDWIDTH_MESSAGE: =0, set; >0, load
  DataSize size;        // =0, SPECIAL(end | BANDWIDTH_MESSAGE)
  DataSize piece_size;  // SPECIAL: =0, end; >0, BANDWIDTH_MESSAGE
  RSUnit coef;
  BwType bandwidth;     // BANDWIDTH_MESSAGE: =0, set_full

  void show() const {
    std::cout << std::endl
              << "task_id:   " << task_id << std::endl
              << "src_num:   " << src_num << std::endl
              << "tar_id:    " << tar_id << std::endl
              << "offset:    " << offset << std::endl
              << "size:      " << size << std::endl
              << "psize:     " << piece_size << std::endl
              << "coef:      " << static_cast<int>(coef) << std::endl
              << "bandwidth: " << bandwidth << std::endl;
  }
};

struct ReceiveTask {
  RepairTask rt;
  Count src_id;

  void show() const {
    rt.show();
    std::cout << "src_id:    " << src_id << std::endl;
  }
};

struct DataPiece {  // *  MESSAGE  *         LOCAL         *  NETWORK  * //
  Count task_id;    // *  task_id  *        task_id        *  task_id  * //
  DataSize offset;  // *     0     *        off-set        *  off-set  * //
  DataSize size;    // * task_size *   psize   |     0     *   psize   * //
  BufUnit *buf;     // *  nullptr  *    buf    |  nullptr  *    buf    * //
  Count tar_id;     // *     0     *       target_id       *     0     * //
  Count src_num;    // *     0     *        src_num        *     0     * //
  TTime delay_time; // *     0     *       delaytime       *     0     * //

  void show() const {
    std::cout << std::endl
              << "task_id:   " << task_id << std::endl
              << "offset:    " << offset << std::endl
              << "size:      " << size << std::endl
              << "tar_id:    " << tar_id << std::endl
              << "time:      " << delay_time << std::endl
              << "buf:       ";
    if (buf)
      std::cout << "length of " << size;
    else
      std::cout << "NULL";
    std::cout << std::endl << std::endl;
  }
};

} // namespace exr

#endif // EXR_UTIL_TYPES_HH_

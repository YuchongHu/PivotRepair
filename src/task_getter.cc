#include "task_getter.hh"

TaskGetter::TaskGetter(const std::string &taddr, uint16_t port, const std::string &_tfile_addr, const std::string &wdfile_addr)
    : tfile_addr{_tfile_addr},
      err_cnt{0},
      frag_err_list{nullptr},
      rsize{0}, psize{0},
      build_alg{'f'},
      has_new_bw{false},
      remain{0},
      repeat{0},
      message{'y'}
{
  pcs = new ConnectionSolver(taddr, port);
  wdf = std::ofstream(wdfile_addr);
  if (!wdf.is_open()) {
    std::cerr << "open write data file error" << std::endl;
    exit(-1);
  }
}

TaskGetter::~TaskGetter()
{
  delete pcs;
  wdf.close();
}

bool TaskGetter::loaded_new()
{
  if (repeat > 0) {
    repeat--;
    has_new_bw = false;
    return true;
  }
  if (remain == 0) {
    //clearing
    if (frag_err_list) {
      delete frag_err_list;
    }
    if (tf) {
      tf.close();
    }
    pcs->write(&message, sizeof(message));

    //get task option
    pcs->read(&message, sizeof(message));
    if (message == 'n') {
      return false;
    }

    //open file
    tf = std::ifstream(tfile_addr);
    if (!tf.is_open()) {
      std::cerr << "no test task file" << std::endl;
      exit(-1);
    }

    tf >> remain;
  }

  load_next();
  return true;
}

void TaskGetter::load_next()
{
  remain--;
  tf >> repeat;
  repeat -= 1;

  tf >> err_cnt;
  frag_err_list = new int[err_cnt];
  for (int i = 0; i < err_cnt; ++i)
  {
    tf >> frag_err_list[i];
  }

  tf >> rsize >> psize;
  tf >> build_alg;

  int flag;
  tf >> flag;
  has_new_bw = flag == 1;
}

int TaskGetter::get_err_cnt()
{
  return err_cnt;
}

int *TaskGetter::get_frag_err_list()
{
  return frag_err_list;
}

ssize_t TaskGetter::get_request_size()
{
  return rsize;
}

ssize_t TaskGetter::get_piece_size()
{
  return psize;
}

char TaskGetter::get_build_alg()
{
  return build_alg;
}

bool TaskGetter::need_load_bw()
{
  return has_new_bw;
}

void TaskGetter::store_result(double global_min, double compute_time, double repair_time)
{
  wdf << global_min << ", ";
  wdf << compute_time << ", ";
  wdf << repair_time << std::endl;
}
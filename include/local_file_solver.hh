#ifndef FTPR_LOCAL_FILE_SOLVER_HH
#define FTPR_LOCAL_FILE_SOLVER_HH

#include <string>

#include "data_solver_interface.hh"

class LocalFileSolver : public DataSolverInterface
{
private:
  std::string load_ad;
  std::string store_ad;

public:
  LocalFileSolver(const std::string &_load_ad, const std::string &_store_ad);
  ~LocalFileSolver();

  //implement the DataSolverInterface, to load/store data to local files
  void read(void *buf, ssize_t size);
  void write(void *buf, ssize_t size);
};

#endif
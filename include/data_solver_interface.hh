#ifndef FTPR_DATA_SOLVER_INTERFACE_HH
#define FTPR_DATA_SOLVER_INTERFACE_HH

class DataSolverInterface
{
public:
  //to get data from it
  virtual void read(void *buf, ssize_t size) = 0;
  //let the solver to save the data or to send to another solver
  virtual void write(void *buf, ssize_t size) = 0;
};

#endif
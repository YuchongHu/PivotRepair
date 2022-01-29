#ifndef FTPR_TASKS_HH
#define FTPR_TASKS_HH

struct ntask
{
  uint16_t id;
  uint16_t sid_num;
  uint16_t tid;
  ssize_t size;
  ssize_t psize;
  uint8_t coef;

  ntask(uint16_t _id, uint16_t _sid_num, uint16_t _tid, ssize_t _size, ssize_t _psize, uint8_t _coef)
      : id(_id), sid_num(_sid_num), tid(_tid), size(_size), psize(_psize), coef(_coef){};
  ntask() : ntask(0, 0, 0, 0, 0, 0){};
};

struct load_task
{
  uint16_t task_id;
  uint16_t cur_pid;
  ssize_t remain;
  ssize_t psize;
  char* buf;

  load_task(uint16_t _task_id, ssize_t _remain, ssize_t _psize)
      : task_id(_task_id), cur_pid(0), remain(_remain), psize(_psize), buf(nullptr){};
  load_task() : load_task(0, 0, 0){};
};

struct store_task
{
  ssize_t size;
  ssize_t psize;
  ssize_t remain;
  char* buf;

  store_task(ssize_t _size, ssize_t _psize)
      : size(_size), psize(_psize), remain(_size), buf(nullptr){};
  store_task() : store_task(0, 0){};
};

struct data_piece
{
  uint16_t task_id;
  uint16_t pid;
  ssize_t size;
  char* buf;

  data_piece(uint16_t _task_id, uint16_t _pid, ssize_t _size, char* _buf)
      : task_id(_task_id), pid(_pid), size(_size), buf(_buf){};
  data_piece() : data_piece(0, 0, 0, nullptr){};
};

struct itask
{
	uint16_t sid;
	uint16_t index;
	itask(uint16_t _sid, uint16_t _index)
			: sid(_sid), index(_index){};
	itask() : itask(0, 0){};
};

struct otask
{
  uint16_t id;
  otask(uint16_t _id) : id(_id){};
  otask() : otask(0){};
};

#endif

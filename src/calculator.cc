#include "calculator.hh"
#include <iostream>

#include <thread>
#include <cstring>

Calculator::Calculator(uint16_t _id, uint16_t k, uint16_t m)
    : id{_id}
{
  pct = new ComputeTool(id, k, m);
}

Calculator::~Calculator()
{
  delete pct;
}

void Calculator::mul_piece(uint16_t task_id, char* buf, ssize_t size)
{
  pct->mul_local(task_id, reinterpret_cast<uint8_t*>(buf), size);
}

char* Calculator::cal_piece(data_piece dp)
{
  std::unique_lock<std::mutex> slck(snum_map_mtx);
  uint16_t remain_num = snum_map.at(dp.task_id);
  slck.unlock();

  if (remain_num == 1) {
    return dp.buf;
  }

  std::unique_lock<std::mutex> plck(pcp_map_mtx);
  auto tpmap = pcp_map.at(dp.task_id);
  auto it = tpmap->find(dp.pid);
  piece* pcp;
	char* buf = nullptr;
  if (it == tpmap->end()) {
    pcp = new piece(remain_num - 1, dp.buf, dp.size);
		tpmap->insert({dp.pid, pcp});
		plck.unlock();
  } else {
    pcp = it->second;
		plck.unlock();

		std::unique_lock<std::mutex> lck(pcp->mtx);
		pct->xor_data(reinterpret_cast<uint8_t *>(dp.buf),
                  reinterpret_cast<uint8_t *>(pcp->buf),
									dp.size);
		if (pcp->remain_num == 1) {
			buf = pcp->buf;
		} else {
			pcp->remain_num -= 1;
		}
		lck.unlock();
  }

  if (buf != nullptr) {
    plck.lock();
    tpmap->erase(dp.pid);
    delete pcp;
    plck.unlock();
  }

  return buf;
}

void Calculator::add_task(uint16_t task_id, uint8_t coef, uint16_t remain_num)
{
  pct->add_task(task_id, coef);

  std::unique_lock<std::mutex> slck(snum_map_mtx);
  snum_map[task_id] = remain_num;
  slck.unlock();

  std::unique_lock<std::mutex> plck(pcp_map_mtx);
  pcp_map[task_id] = new std::unordered_map<uint16_t, piece*>();
  plck.unlock();
}

void Calculator::release_task(uint16_t task_id)
{
  pct->release_task(task_id);

  std::unique_lock<std::mutex> slck(snum_map_mtx);
  snum_map.erase(task_id);
  slck.unlock();

  std::unique_lock<std::mutex> plck(pcp_map_mtx);
  delete pcp_map.at(task_id);
  pcp_map.erase(task_id);
  plck.unlock();
}

#include "node_repairer.hh"

#include <utility>

NodeRepairer::NodeRepairer(uint16_t _din_num, uint16_t _prc_num, uint16_t _out_num,
                           uint16_t _id,
                           uint16_t k, uint16_t m, std::unique_ptr<node_address[]> node_addresses,
                           uint16_t block_num, ssize_t block_size,
                           const std::string &load_ad, const std::string &store_ad,
                           ssize_t _max_load_size)
    : din_num{_din_num},
      prc_num{_prc_num},
      out_num{_out_num},
      id{_id},
      shut_down_flag{false},
      max_load_size{_max_load_size}
{
	n = k + m;
  //creating thread ptrs
  din_thrs = std::unique_ptr<std::thread[]>(new std::thread[din_num]);
  prc_thrs = std::unique_ptr<std::thread[]>(new std::thread[prc_num]);
  out_thrs = std::unique_ptr<std::thread[]>(new std::thread[out_num]);

	//initialize remain tables
	input_remains = std::unique_ptr<ssize_t[]>(new ssize_t[n + 1]);
	output_remains = std::unique_ptr<ssize_t[]>(new ssize_t[n + 1]);
	for (uint16_t i = 0; i <= n; ++i) {
		input_remains[i] = 0;
		output_remains[i] = 0;
	}

  //connect to other nodes
  pnc = new NodeConnector(id, n + 1, std::move(node_addresses), load_ad, store_ad, block_num, block_size);

  //initialize the calculator
  pc = new Calculator(id, k, m);
}

NodeRepairer::~NodeRepairer()
{
  mthr.join();

  delete pnc;
  delete pc;
}

//the functions of the threads
void NodeRepairer::get_data()
{
  while (true)
  {
    //get a input task. sid=0 means finished
    itask task = itask_q.pop();
    if (task.sid == 0) {
      break;
    }

    std::unique_lock<std::mutex> ilck(im_mtx);
    while (true)
    {
	if (input_remains[task.sid] == 0) {
    //std::cout << "getted from " << task.sid << std::endl;
		ilck.unlock();
		break;
	}
	ilck.unlock();

      data_piece dp = pnc->read(task.sid, task.index);
      if (task.sid == id) {
        pc->mul_piece(dp.task_id, dp.buf, dp.size);
      }
      ptask_q.push(dp);

			ilck.lock();
      input_remains[task.sid] -= dp.size;
    }
  }
}

void NodeRepairer::process_data()
{
  while (true)
  {
    //get a process task. null-buff means finished
    data_piece dp = ptask_q.pop();
    if (dp.buf == nullptr) {
      break;
    }

    //process the data. return a address means the process is finished
    char* buf = pc->cal_piece(dp);

    if (buf != nullptr) {
      dp.buf = buf;
			std::unique_lock<std::mutex> tmlck(tm_mtx);
			uint16_t tid = task_tid_map.at(dp.task_id);
			tmlck.unlock();
      std::unique_lock<std::mutex> olck(omap_mtx);
      auto odata_q = output_wq_map.at(tid);
      olck.unlock();
      odata_q->push(dp);
    }
  }
}

void NodeRepairer::send_data()
{
  while (true)
  {
    //get a output task. tid=0 means finished
    uint16_t tid = otask_q.pop().id;
    if (tid == 0) {
      break;
    }

    std::unique_lock<std::mutex> olck(omap_mtx);
    auto odata_q = output_wq_map.at(tid);
    olck.unlock();

		std::unique_lock<std::mutex> omlck(om_mtx);
    while (true)
    {
      if (output_remains[tid] == 0) {
        //std::cout << "sended to " << tid << std::endl;
        olck.lock();
        delete odata_q;
        output_wq_map.erase(tid);
        olck.unlock();

        omlck.unlock();
        break;
      }
      omlck.unlock();

      data_piece dp = odata_q->pop();
      pnc->write(tid, dp);

      std::unique_lock<std::mutex> tmlck(tm_mtx);
      task_remains.at(dp.task_id) -= dp.size;
      if (task_remains.at(dp.task_id) == 0) {
        task_remains.erase(dp.task_id);
        task_tid_map.erase(dp.task_id);
				pc->release_task(dp.task_id);
				pnc->release_task(dp.task_id);

        std::unique_lock<std::mutex> mslck(master_send_mtx);
        if (tid == id)
        {
          pnc->write(0, &(dp.task_id), sizeof(dp.task_id));
        }
        //std::cout << "task " << dp.task_id << " ended" << std::endl;
        mslck.unlock();
      }
      tmlck.unlock();

			omlck.lock();
      output_remains[tid] -= dp.size;
    }
  }
}

void NodeRepairer::get_tasks()
{
  //tell master that this node is ready for the tasks
  pnc->write(0, &id, sizeof(id));
  while (true)
  {
    //get task from master
    ntask task;
    pnc->read(0, &task, sizeof(ntask));
    //std::cout << "\n\n" << id << " getted task: " << task.id
              //<< "\nsize: " << task.size
              //<< "\npsize: " << task.psize
              //<< "\nsid_num: " << task.sid_num
              //<< "\ntid: " << task.tid
              //<< std::endl;
    //no source id means ended
    if (task.size == 0) {
      break;
    }

		uint16_t remain = task.tid == id ? task.sid_num : task.sid_num + 1;
		pnc->prepare(task.id, task.sid_num, task.tid == id, task.size, task.psize);

		std::unique_lock<std::mutex> tmlck(tm_mtx);
    task_tid_map[task.id] = task.tid;
    task_remains[task.id] = task.size;
    tmlck.unlock();
		pc->add_task(task.id, task.coef, remain);

    std::unique_lock<std::mutex> olck(om_mtx);
    if (output_remains[task.tid] == 0) {
      std::unique_lock<std::mutex> omaplck(omap_mtx);
      output_wq_map[task.tid] = new WaitingQueue<data_piece>();
      omaplck.unlock();
      otask_q.push(otask(task.tid));
    }
		output_remains[task.tid] += task.size;
    olck.unlock();

    for (uint16_t i = 0; i < task.sid_num; ++i)
    {
      uint16_t sid;
      pnc->read(0, &sid, sizeof(sid));

      std::unique_lock<std::mutex> ilck(im_mtx);
      if (input_remains[sid] == 0) {
        itask_q.push(itask(sid, i));
      }
			input_remains[sid] += task.size;
      ilck.unlock();
    }

    //if needed load from local
    if (task.tid != id) {
      std::unique_lock<std::mutex> ilck(im_mtx);
      if (input_remains[id] == 0) {
        itask_q.push(itask(id, task.sid_num));
      }
			input_remains[id] += task.size;
      ilck.unlock();
    }
  }
  shut_down();
}

void NodeRepairer::shut_down()
{
	std::unique_lock<std::mutex> ilck(im_mtx);
	std::unique_lock<std::mutex> olck(om_mtx);
	for (uint16_t i = 0; i <= n; ++i) {
		input_remains[i] = 0;
		output_remains[i] = 0;
	}
	olck.unlock();
	ilck.unlock();

  itask_q.close();
  ptask_q.close();
  otask_q.close();

  for (uint16_t i = 0; i < din_num; ++i) {
    din_thrs[i].join();
  }
  for (uint16_t i = 0; i < prc_num; ++i) {
    prc_thrs[i].join();
  }
  for (uint16_t i = 0; i < out_num; ++i) {
    out_thrs[i].join();
  }

  for (auto &x: output_wq_map) {
		delete x.second;
  }

  shut_down_flag = true;
}

//start listening tasks and run
void NodeRepairer::start()
{
  for (uint16_t i = 0; i < din_num; ++i) {
    din_thrs[i] = std::thread([&] { get_data(); });
  }
  for (uint16_t i = 0; i < prc_num; ++i) {
    prc_thrs[i] = std::thread([&] { process_data(); });
  }
  for (uint16_t i = 0; i < out_num; ++i) {
    out_thrs[i] = std::thread([&] { send_data(); });
  }

  mthr = std::thread([&] { get_tasks(); });
}

bool NodeRepairer::is_shut_down()
{
  return shut_down_flag;
}

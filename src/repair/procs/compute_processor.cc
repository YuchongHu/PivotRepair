#include "repair/procs/compute_processor.hh"

#include <utility>
#include <vector>

namespace exr {

//Constructor and destructor
ComputeProcessor::ComputeProcessor(const Count &thr_n, MemoryPool &mp,
                                   DataProcessor<DataPiece> &next_prc)
    : DataProcessor<DataPiece>(1, thr_n), mp_(mp), next_prc_(next_prc),
      rc_(2, 1) {
  RSUnit coefs[2] = {1, 1};
  rc_.InitForEncode(coefs);
}

ComputeProcessor::~ComputeProcessor() { Close(); }

//Distribute
Count ComputeProcessor::Distribute(const DataPiece &data) { return 0; }

//Process the data
void ComputeProcessor::Process(DataPiece data, Count qid) {
  //Get Group, create one if not exist
  auto task_id = data.task_id;
  auto size = data.size;
  std::unique_lock<std::mutex> lck(mtx_);
  auto &pg = task_pieces_[task_id];
  lck.unlock();

  //Deal with the content
  if (!(data.buf) && size > 0) {
    //Task info
    next_prc_.PushData(std::move(data));
    size = 0 - size;
  } else if (!AddPiece_(pg, std::move(data))) {
    //Data piece not sended out
    size = 0;
  }

  //Check if task ended
  std::unique_lock<std::mutex> rlck(pg.remain_mtx);
  if (size < 0)
    pg.total = 0 - size;
  else
    pg.sum += size;
  if (pg.sum == pg.total && pg.total > 0) {
    rlck.unlock();
    lck.lock();
    task_pieces_.erase(task_id);
  }
}

bool ComputeProcessor::AddPiece_(PieceGroup &pg, DataPiece data) {
  //Get piece, create one if not exist
  auto offset = data.offset;
  std::unique_lock<std::mutex> glck(pg.map_mtx);
  auto &ptp = pg.pieces[offset];
  if (!ptp) {
    ptp = std::make_unique<TempPiece>(std::move(data), 0);
    ptp->temp_buf = mp_.Get(0, offset);
    glck.unlock();
  } else {
    glck.unlock();
    //XOR two pieces and gather the infomation
    std::unique_lock<std::mutex> plck(ptp->mtx);
    ptp->dp.tar_id += data.tar_id;
    ptp->dp.delay_time += data.delay_time;
    if (!(ptp->dp.buf)) {
      ptp->dp.size = data.size;
      ptp->dp.buf = data.buf;
    } else if (data.buf) {
      BufUnit *srcs[2] = {ptp->dp.buf, data.buf},
              *tars[1] = {ptp->temp_buf};
      rc_.Encode(data.size, srcs, tars);
      ptp->dp.buf = ptp->temp_buf;
      ptp->temp_buf = data.buf;
    }
    plck.unlock();
  }

  //Check if need to send the data out
  std::unique_lock<std::mutex> plck(ptp->mtx);
  ++(ptp->num);
  ptp->src_num += data.src_num;
  if (ptp->src_num == ptp->num) {
    next_prc_.PushData(std::move(ptp->dp));
    plck.unlock();
    glck.lock();
    pg.pieces.erase(offset);
    return true;
  }
  return false;
}

} // namespace exr

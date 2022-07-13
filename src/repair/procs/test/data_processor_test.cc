#include <iostream>
#include <mutex>

#include "repair/procs/data_processor.hh"

//Complete the DataProcessor to a integer adder
class DPTestAdder : public exr::DataProcessor<int> {
 public:
  DPTestAdder(const exr::Count &thr_n, const int base,
              exr::DataProcessor<int> *next_prc)
      : exr::DataProcessor<int>(1, thr_n), base_(base),
        last_(!next_prc), next_prc_(next_prc) {}
  ~DPTestAdder() = default;

 protected:
  exr::Count Distribute(const int &data) override { return 0; }
  void Process(int data, exr::Count pid) override {
    std::unique_lock<std::mutex> lck(mtx_);
    if (last_)
      std::cout << base_ << " + " << data << " = " << base_ + data
                << std::endl;
    base_ += data;
    data = base_;
    if (!last_)
      next_prc_->PushData(std::move(data));
  }

 private:
  int base_;
  bool last_;
  std::mutex mtx_;
  exr::DataProcessor<int> *next_prc_;
};

//Test
int main()
{
  DPTestAdder adder2(4, 100, nullptr);
  adder2.Run();
  DPTestAdder adder1(3, 10, &adder2);
  adder1.Run();

  for (int i = 0; i < 10; ++i) {
    adder1.PushData(i + 1);
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  return 0;
}

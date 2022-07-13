#include "util/rs_computer.hh"

#include <cstring>
#include <iostream>

#include "isa-l.h"

namespace exr {

//Constructor and destructor
RSComputer::RSComputer(const Count cn, const Count ck) : cn_(cn), ck_(ck) {}

RSComputer::~RSComputer() = default;

//Matrix decoding -- Get coefs
void RSComputer::InitForDecode() {
  matrix_ = std::make_unique<RSUnit[]>((cn_ + ck_ * 2) * ck_);
  gf_gen_cauchy1_matrix(matrix_.get(), cn_, ck_);
}

void RSComputer::Decode(const Count &tar_n, const Count *tars,
                        const Count *srcs, RSUnit* results) {
  RSUnit *cauchy = matrix_.get();
  RSUnit *tempc = cauchy + cn_ * ck_;
  RSUnit *tempi = tempc + ck_ * ck_;
  for (Count i = 0; i < ck_; ++i)
    memcpy(tempc + i * ck_, cauchy + srcs[i] * ck_, sizeof(RSUnit) * ck_);

  if (gf_invert_matrix(tempc, tempi, ck_) < 0) {
    std::cout << "error when inverting matrix" << std::endl;
    exit(-1);
  }

  for (Count i = 0; i < tar_n; ++i) {
    if (tars[i] < ck_) {
      memcpy(results + i * ck_, tempi + tars[i] * ck_,
             sizeof(RSUnit) * ck_);
    } else {
      for (Count j = 0; j < ck_; ++j) {
        RSUnit s = 0;
        for (Count k = 0; k < ck_; ++k)
          s ^= gf_mul(tempi[k * ck_ + j], cauchy[tars[i] * ck_ + k]);
        results[i * ck_ + j] = s;
      }
    }
  }
}

//Matrix encoding -- Multiply and XOR data
void RSComputer::InitForEncode(RSUnit *coefs) {
  matrix_ = std::make_unique<RSUnit[]>(32 * cn_ * ck_);
  ec_init_tables(cn_, ck_, coefs, matrix_.get());
}

void RSComputer::Encode(const DataSize &size, void *srcs, void *tars) {
  ec_encode_data(size, cn_, ck_, matrix_.get(),
                 reinterpret_cast<RSUnit**>(srcs),
                 reinterpret_cast<RSUnit**>(tars));
}

} // namespace exr

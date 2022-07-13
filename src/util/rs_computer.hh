#ifndef EXR_UTIL_RSCOMPUTER_HH_
#define EXR_UTIL_RSCOMPUTER_HH_

#include <memory>

#include "util/typedef.hh"

namespace exr {

/* Used for erasure code computation */
class RSComputer
{
 public:
  //(cn, ck) represents: (n, k) for Erasure Code when decoding,
  //                     (in, out) when encoding
  RSComputer(Count cn, Count ck);
  ~RSComputer();

  //Compute decode matrix to get coefs
  void InitForDecode();
  void Decode(const Count &tar_n, const Count *tars, const Count *srcs,
              RSUnit* results);

  //Encode data using Multiply and XOR
  void InitForEncode(RSUnit *coefs); //lenth of coefs is cn * ck
  void Encode(const DataSize &size, void *srcs, void *tars);

  //RSComputer is neither copyable nor movable
  RSComputer(const RSComputer&) = delete;
  RSComputer& operator=(const RSComputer&) = delete;

 private:
  Count cn_;
  Count ck_;
  std::unique_ptr<RSUnit[]> matrix_;
};

} // namespace exr

#endif // EXR_UTIL_RSCOMPUTER_HH_

#ifndef EXR_CONFIG_ALGLOADER_HH_
#define EXR_CONFIG_ALGLOADER_HH_

#include <fstream>
#include <memory>

#include "util/typedef.hh"

namespace exr {

class AlgLoader
{
 public:
  AlgLoader(const Path &task_path, const Path &band_path);
  ~AlgLoader();

  void Open(const Path &path);
  bool LoadNext();
  void Close();

  Alg GetAlg();
  Count* GetArgs();
  Path& GetPath();

  //AlgLoader is neither copyable nor movable
  AlgLoader(const AlgLoader&) = delete;
  AlgLoader& operator=(const AlgLoader&) = delete;

 private:
  std::fstream af_;
  Count alg_num_;
  Count cur_num_;

  Alg alg_;
  Count arg_num_;
  std::unique_ptr<Count[]> args_;
  Path tpath_;
  Path bpath_;
};

} // namespace exr

#endif // EXR_CONFIG_ALGLOADER_HH_

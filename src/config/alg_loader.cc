#include "config/alg_loader.hh"

#include <iostream>

namespace exr {

//Constructor and destructor
AlgLoader::AlgLoader(const Path &task_path, const Path &band_path)
    : tpath_(task_path), bpath_(band_path) {}
AlgLoader::~AlgLoader() { Close(); }

//Open a algorithm file
void AlgLoader::Open(const Path &path) {
  Close(); //Close the previos file

  //Try to open the new file
  af_ = std::fstream(path, std::ios::in);
  if (!af_.is_open()) {
    std::cerr << "no algorithm file: " << path << std::endl;
    exit(-1);
  }

  //Get infomation of the total number
  af_ >> alg_num_;
  cur_num_ = 0;
}

//Load next algorithm's infomation
bool AlgLoader::LoadNext() {
  //Check whether it is the end
  if (++cur_num_ > alg_num_) return false;

  //Load
  af_ >> alg_ >> arg_num_;
  if (arg_num_ > 0) {
    args_ = std::make_unique<Count[]>(arg_num_);
    for (Count i = 0; i < arg_num_; ++i)
      af_ >> args_[i];
  }
  return true;
}

//Close if file if has opened
void AlgLoader::Close() { if (af_.is_open()) af_.close(); }

//Get loaded infomation
Alg AlgLoader::GetAlg() { return alg_; }
Count* AlgLoader::GetArgs() { return arg_num_ > 0 ? args_.get() : nullptr; }
Path& AlgLoader::GetPath() { return alg_ == 't' ? tpath_ : bpath_; }

} // namespace exr

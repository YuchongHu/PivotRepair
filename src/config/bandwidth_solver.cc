#include "config/bandwidth_solver.hh"

#include <iostream>
#include <sstream>

namespace exr {

//Constructor and destructor
BandwidthSolver::BandwidthSolver(const Name &eth, const bool &if_print)
    : is_pure_load_(eth == ""), if_print_(if_print),
      bw_num_(0), node_num_(0), cur_(0),
      eth_(eth), reset_cmd_(kResetCmd + eth) {}

BandwidthSolver::~BandwidthSolver() { ResetBandwidth(); Close();}

//Open a bandwidth file
void BandwidthSolver::Open(const Path &path) {
  Close(); //Close the file if has opened

  //Try to open the new file
  bwf_ = std::fstream(path, std::ios::in);
  if (!bwf_.is_open()) {
    std::cerr << "no bw file: " << path << std::endl;
    exit(-1);
  }

  //Get infomation
  bwf_ >> bw_num_ >> node_num_;
  bandwidths_ = std::make_unique<Bandwidth[]>(node_num_);
  cur_ = 0;
}

//Load next group of data
bool BandwidthSolver::LoadNext() {
  //Check whether the file is ended
  if (++cur_ > bw_num_) return false;

  //Load
  LoadBwType load_bw;
  for (Count i = 0; i < node_num_; ++i) {
    bwf_ >> load_bw;
    bandwidths_[i].upload = load_bw * 1000;
  }
  for (Count i = 0; i < node_num_; ++i) {
    bwf_ >> load_bw;
    bandwidths_[i].download = load_bw * 1000;
  }

  return true;
}

//Close the file
void BandwidthSolver::Close() {
  if (bwf_.is_open()) bwf_.close();
}


//Set full bandwidth to a node
void BandwidthSolver::SetFull(const Count &id) {
  bandwidths_[id - 1].upload = 0;
  bandwidths_[id - 1].download = kFullBandwidth;
}

//Get all the nodes' bandwidth
Bandwidth* BandwidthSolver::GetBandwidths() {
  return bandwidths_.get();
}

//Set bandwidth to one of the bandwidth values
void BandwidthSolver::SetBandwidth(const Count &id, const bool &is_full) {
  BwType upload = bandwidths_[id - 1].upload;
  BwType download = bandwidths_[id - 1].download;

  if (is_full) {
    upload = kFullBandwidth;
    download = kFullBandwidth;
  } else {
    if (upload < kMinSetBandwidth) upload = kMinSetBandwidth;
    if (download < kMinSetBandwidth) download = kMinSetBandwidth;
  }

  ResetBandwidth();
  std::stringstream fmt;
  fmt << kSetCmd << eth_ << kUploadPara << upload
                         << kDownloadPara << download;
  if (if_print_) {
    std::cout << cur_ << ": " << fmt.str() << std::endl;
  } else {
    auto _ = system(fmt.str().c_str());
    ++_;
  }
}

//Clear all modification to the bandwidth
void BandwidthSolver::ResetBandwidth() {
  if (is_pure_load_) return;
  if (if_print_) {
    std::cout << reset_cmd_ << std::endl;
  } else {
    auto _ = system(reset_cmd_.c_str());
    ++_;
  }
}

//Static values for bandwidth setting commands
const CmdType BandwidthSolver::kSetCmd = "wondershaper -a ";
const CmdType BandwidthSolver::kResetCmd = "wondershaper -c -a ";
const CmdType BandwidthSolver::kUploadPara = " -u ";
const CmdType BandwidthSolver::kDownloadPara = " -d ";
const BwType BandwidthSolver::kFullBandwidth = 1000000;
const BwType BandwidthSolver::kMinSetBandwidth = 5000;

} // namespace exr

#ifndef EXR_CONFIG_BANDWIDTHSOLVER_HH_
#define EXR_CONFIG_BANDWIDTHSOLVER_HH_

#include <fstream>
#include <memory>
#include <string>

#include "util/typedef.hh"

namespace exr {

using LoadBwType = double;
using CmdType = std::string;

class BandwidthSolver
{
 public:
  BandwidthSolver(const Name &eth_name, const bool &if_print);
  ~BandwidthSolver();

  void Open(const Path &path);
  bool LoadNext();
  void Close();

  void SetFull(const Count &id);
  Bandwidth* GetBandwidths();

  void SetBandwidth(const Count &id, const bool &is_full);
  void ResetBandwidth();

  //BandwidthSolver is neither copyable nor movable
  BandwidthSolver(const BandwidthSolver&) = delete;
  BandwidthSolver& operator=(const BandwidthSolver&) = delete;

 private:
  bool is_pure_load_;
  bool if_print_;
  std::fstream bwf_;
  Count bw_num_;
  Count node_num_;
  Count cur_;
  std::unique_ptr<Bandwidth[]> bandwidths_;
  Name eth_;
  CmdType reset_cmd_;

  static const CmdType kSetCmd;
  static const CmdType kResetCmd;
  static const CmdType kUploadPara;
  static const CmdType kDownloadPara;
  static const BwType kFullBandwidth;
  static const BwType kMinSetBandwidth;
};

} // namespace exr

#endif // EXR_CONFIG_BANDWIDTHSOLVER_HH_

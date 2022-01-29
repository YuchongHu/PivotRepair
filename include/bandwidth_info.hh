#ifndef FTPR_BANDWIDTH_HELPER_HH
#define FTPR_BANDWIDTH_HELPER_HH

#include <string>

#define BW_CEILING 1000.0

class BandwidthInfo
{
public:
  double *upload = nullptr;
  double *download = nullptr;
  double **matrix = nullptr;
  int num = 0;

  BandwidthInfo() = default;
  BandwidthInfo(int rs_n);
  ~BandwidthInfo();

  void get_bandwidth(char *upload_raw, char *download_raw);
  void copy_bandwidth(double *upload_src, double *download_src);
  void get_bandwith_matrix();
  void load_bandwidth(const std::string &bw_addr);
};

#endif
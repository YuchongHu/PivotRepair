#include "bandwidth_info.hh"

#include <cstdio>
#include <cstring>

#include <iostream>
#include <fstream>

BandwidthInfo::BandwidthInfo(int rs_n)
{
  num = rs_n + 1;
  upload = new double[num];
  download = new double[num];
  // reserve for requestor, at [0]
  download[0] = upload[0] = BW_CEILING;
  matrix = new double *[num];
  for (int i = 0; i < num; i++)
  {
    matrix[i] = new double[num];
  }
}

BandwidthInfo::~BandwidthInfo()
{
  if (upload) {
    delete[] upload;
  }
  if (download) {
    delete[] download;
  }
  if (matrix) {
    delete[] matrix;
  }
}

/*
  Assume that the raw lines of data nodes'bandwidths (excluding requestor) are
  provided
 */
void BandwidthInfo::get_bandwidth(char *upload_raw, char *download_raw)
{
  int upload_raw_len = strlen(upload_raw);
  int download_raw_len = strlen(download_raw);
  // parse bandwidths for data nodes
  int i = 1, j = 0, k = 0, t;
  while (j < upload_raw_len && k < download_raw_len)
  {
    for (t = j; upload_raw[t] != ',' && upload_raw[t] != '\0'; ++t)
      ; // just walk through
    upload_raw[t] = '\0';
    sscanf(upload_raw + j, "%lf", upload + i);
    j = ++t;

    for (t = k; download_raw[t] != ',' && download_raw[t] != '\0'; ++t)
      ; // just walk through
    download_raw[t] = '\0';
    sscanf(download_raw + k, "%lf", download + i);
    k = ++t;
    if (++i >= num) {
      break;
    }
  }
  get_bandwith_matrix();
}

/*
  Assume that the bandwidths of data nodes (excluding requestor) are provided
 */
void BandwidthInfo::copy_bandwidth(double *upload_src, double *download_src)
{
  memcpy(upload + 1, upload_src, (num - 1) * sizeof(double));
  memcpy(download + 1, download_src, (num - 1) * sizeof(double));
  get_bandwith_matrix();
}

void BandwidthInfo::load_bandwidth(const std::string &bw_addr)
{
  std::ifstream bwf(bw_addr);
  if (!bwf.is_open()) {
    std::cerr << "no bandwith file" << std::endl;
    exit(-1);
  }

  for (int i = 1; i < num; ++i)
  {
    bwf >> upload[i];
  }
  for (int i = 1; i < num; ++i)
  {
    bwf >> download[i];
  }
  get_bandwith_matrix();
}

void BandwidthInfo::get_bandwith_matrix()
{
  for (int i = 0; i < num; i++)
  {
    matrix[i] = new double[num];
    for (int j = 0; j < num; j++)
    {
      if (i == j) {
        matrix[i][j] = BW_CEILING;
      } else {
        matrix[i][j] = upload[i] > download[j] ? download[j] : upload[i];
      }
    }
  }
}
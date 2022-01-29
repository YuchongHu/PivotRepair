#ifndef FTPR_CONFIG_READER_HH
#define FTPR_CONFIG_READER_HH

#include <string>

class ConfigReader
{
private:
  //node address info
  std::string aconf_addr;
  std::string bw_addr;
  std::string tfile_addr;
  std::string wdfile_addr;
  std::string tc_addr;
  uint16_t tc_port;

  //node setting
  uint16_t din_num;
  uint16_t prc_num;
  uint16_t out_num;

  uint16_t mem_num;
  ssize_t mem_size;

  std::string read_addr;
  std::string write_addr;
  uint16_t width;
  char fillc;
  std::string suffix;

  ssize_t max_load_size;

  double min_global_min;

public:
  ConfigReader(const std::string &conf_addr);
  ~ConfigReader();

  std::string get_aconf_addr();
  std::string get_bw_addr();
  std::string get_tfile_addr();
  std::string get_wdfile_addr();
  std::string get_tc_addr();
  uint16_t get_tc_port();

  uint16_t get_din_num();
  uint16_t get_prc_num();
  uint16_t get_out_num();

  uint16_t get_mem_num();
  ssize_t get_mem_size();

  std::string get_read_addr(uint16_t id);
  std::string get_write_addr(uint16_t id);

  ssize_t get_max_load_size();

  double get_min_global_min();
};

#endif
#include "config_reader.hh"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

ConfigReader::ConfigReader(const std::string &conf_addr)
{
  std::ifstream cf(conf_addr);
  if (!cf.is_open()) {
    std::cerr << "no config file" << std::endl;
    exit(-1);
  }

  cf >> aconf_addr;
  cf >> bw_addr;
  cf >> tfile_addr;
  cf >> wdfile_addr;
  cf >> tc_addr >> tc_port;
  cf >> din_num >> prc_num >> out_num;
  cf >> mem_num >> mem_size;
  cf >> read_addr >> write_addr >> suffix;
  cf >> width >> fillc;
  cf >> max_load_size;
  cf >> min_global_min;

  cf.close();
}

ConfigReader::~ConfigReader() = default;

std::string ConfigReader::get_aconf_addr()
{
  return aconf_addr;
}

std::string ConfigReader::get_bw_addr()
{
  return bw_addr;
}

std::string ConfigReader::get_tfile_addr()
{
  return tfile_addr;
}

std::string ConfigReader::get_wdfile_addr()
{
  return wdfile_addr;
}

std::string ConfigReader::get_tc_addr()
{
  return tc_addr;
}

uint16_t ConfigReader::get_tc_port()
{
  return tc_port;
}

uint16_t ConfigReader::get_din_num()
{
  return din_num;
}

uint16_t ConfigReader::get_prc_num()
{
  return prc_num;
}

uint16_t ConfigReader::get_out_num()
{
  return out_num;
}

uint16_t ConfigReader::get_mem_num()
{
  return mem_num;
}

ssize_t ConfigReader::get_mem_size()
{
  return mem_size;
}

double ConfigReader::get_min_global_min()
{
  return min_global_min;
}

std::string ConfigReader::get_read_addr(uint16_t id)
{
  std::ostringstream ost;
  ost << read_addr;
  ost << std::setw(width) << std::setfill(fillc) << id;
  ost << suffix;
  return ost.str();
}

std::string ConfigReader::get_write_addr(uint16_t id)
{
  std::ostringstream ost;
  ost << write_addr;
  ost << std::setw(width) << std::setfill(fillc) << id;
  ost << suffix;
  return ost.str();
}

ssize_t ConfigReader::get_max_load_size()
{
  return max_load_size;
}
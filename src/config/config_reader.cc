#include "config/config_reader.hh"

#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>

namespace exr {

ConfigReader::ConfigReader() = default;
ConfigReader::~ConfigReader() = default;

void ConfigReader::Load(const Path &path, const Count &id) {
  std::ifstream config_file(path);
  if (!config_file.is_open()) {
    std::cerr << "Read config file error: " << path << std::endl;
    exit(-1);
  }
  config_file >> size_ >> psize_
              >> addr_conf_path_ >> bw_conf_path_
              >> recv_thr_num_ >> comp_thr_num_ >> proc_thr_num_
              >> mem_num_ >> mem_size_
              >> algorithm_file_ >> task_file_ >> result_file_;

  Path rw_file_folder, read_file, write_file;
  Count expand_width;
  char fill_char;
  config_file >> rw_file_folder >> expand_width >> fill_char
              >> read_file >> write_file;

  std::ostringstream ost;
  ost << rw_file_folder;
  ost << std::setw(expand_width) << std::setfill(fill_char) << id;
  read_file_ = ost.str() + read_file;
  write_file_ = ost.str() + write_file;

  Count ifp;
  config_file >> ifp >> eth_;
  if_print_ = (ifp == 1);
  config_file.close();
}

DataSize ConfigReader::get_size() { return size_; }
DataSize ConfigReader::get_psize() { return psize_; }

const Path& ConfigReader::get_addr_conf_path() { return addr_conf_path_; }
const Path& ConfigReader::get_bw_conf_path() { return bw_conf_path_; }

Count ConfigReader::get_recv_thr_num() { return recv_thr_num_; }
Count ConfigReader::get_comp_thr_num() { return comp_thr_num_; }
Count ConfigReader::get_proc_thr_num() { return proc_thr_num_; }

Count ConfigReader::get_mem_num() { return mem_num_; }
DataSize ConfigReader::get_mem_size() { return mem_size_; }

const Path& ConfigReader::get_algorithm_file() { return algorithm_file_; }
const Path& ConfigReader::get_task_file() { return task_file_; }
const Path& ConfigReader::get_result_file() { return result_file_; }

const Path& ConfigReader::get_read_file() { return read_file_; }
const Path& ConfigReader::get_write_file() { return write_file_; }

bool ConfigReader::get_if_print() { return if_print_; }
const Name& ConfigReader::get_eth_name() { return eth_; }

} // namespace exr

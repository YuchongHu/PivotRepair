#ifndef EXR_CONFIG_CONFIGREADER_HH_
#define EXR_CONFIG_CONFIGREADER_HH_

#include "util/typedef.hh"

namespace exr {

/* Load configuratiions from a config file */
class ConfigReader
{
 public:
  ConfigReader();
  ~ConfigReader();

  void Load(const Path &path, const Count &id);

  DataSize get_size();
  DataSize get_psize();

  const Path& get_addr_conf_path();
  const Path& get_bw_conf_path();

  Count get_recv_thr_num();
  Count get_comp_thr_num();
  Count get_proc_thr_num();

  Count get_mem_num();
  DataSize get_mem_size();

  const Path& get_algorithm_file();
  const Path& get_task_file();
  const Path& get_result_file();

  const Path& get_read_file();
  const Path& get_write_file();

  bool get_if_print();
  const Name& get_eth_name();

  //ConfigReader is neither copyable nor movable
  ConfigReader(const ConfigReader&) = delete;
  ConfigReader& operator=(const ConfigReader&) = delete;

 private:
  DataSize size_;
  DataSize psize_;

  Path addr_conf_path_;
  Path bw_conf_path_;

  Count recv_thr_num_;
  Count comp_thr_num_;
  Count proc_thr_num_;

  Count mem_num_;
  DataSize mem_size_;

  Path algorithm_file_;
  Path task_file_;
  Path result_file_;

  Path read_file_;
  Path write_file_;

  bool if_print_;
  Name eth_;
};

} // namespace exr

#endif // EXR_CONFIG_CONFIGREADER_HH_

#ifndef EXR_CONFIG_ADDRESSREADER_HH_
#define EXR_CONFIG_ADDRESSREADER_HH_

#include "util/typedef.hh"

namespace exr {

/* Read IP Addresses from a address config file */
class AddressReader
{
 public:
  AddressReader();
  ~AddressReader();

  void Load(const Path &path);
  IPAddressList GetAddresses();

  Count get_total();

  //AddressReader is neither copyable nor movable
  AddressReader(const AddressReader&) = delete;
  AddressReader& operator=(const AddressReader&) = delete;

 private:
  Path path_;
  Count total_;
  IPAddressList ip_addresses_;

  void Load();
};

} // namespace exr

#endif // EXR_CONFIG_ADDRESSREADER_HH_

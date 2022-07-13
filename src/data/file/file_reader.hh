#ifndef EXR_DATA_FILE_FILEREADER_HH_
#define EXR_DATA_FILE_FILEREADER_HH_

#include <fstream>

#include "util/typedef.hh"

namespace exr {

/* Local file Reader */
class FileReader
{
 public:
  FileReader();
  ~FileReader();

  //File reading...
  void Open(const Path &path);
  void SetOffset(const DataSize &offset);
  DataSize Read(const DataSize &size, void *buf);
  void Close();

  //FileReader is neither copyable nor movable
  FileReader(const FileReader&) = delete;
  FileReader& operator=(const FileReader&) = delete;

 private:
  std::fstream in_;
};

} // namespace exr

#endif // EXR_DATA_FILE_FILEREADER_HH_

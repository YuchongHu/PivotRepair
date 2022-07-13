#ifndef EXR_DATA_FILE_FILEWRITER_HH_
#define EXR_DATA_FILE_FILEWRITER_HH_

#include <fstream>
#include <memory>

#include "util/typedef.hh"

namespace exr {

/* Local file writer */
class FileWriter
{
 public:
  FileWriter();
  ~FileWriter();

  //File writing
  void Open(const Path &path);
  void Write(const DataSize &offset, const DataSize &size, void *buf);
  void Close();
  bool is_open();

  //FileWriter is neither copyable nor movable
  FileWriter(const FileWriter&) = delete;
  FileWriter& operator=(const FileWriter&) = delete;

 private:
  std::fstream out_;
};

} // namespace exr

#endif // EXR_DATA_FILE_FILEWRITER_HH_

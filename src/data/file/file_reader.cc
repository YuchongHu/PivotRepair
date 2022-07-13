#include "data/file/file_reader.hh"

#include <iostream>

namespace exr {

//Constructor and destructor
FileReader::FileReader() = default;

FileReader::~FileReader() { Close(); }

//Open a file
void FileReader::Open(const Path &path) {
  //Close the file if has opened
  if (in_.is_open()) Close();

  //Try to open the new file
  in_ = std::fstream(path, std::ios::in | std::ios::binary);
  if (!in_.is_open()) {
    std::cerr << "Open file \"" << path << "\" error" << std::endl;
    exit(-1);
  }
}

//Jump to a place to read
void FileReader::SetOffset(const DataSize &offset) {
  in_.seekg(offset, std::ios::beg);
}

//Read data
DataSize FileReader::Read(const DataSize &size, void *buf) {
  in_.read(static_cast<BufUnit*>(buf), size);
  return in_.gcount();
}

//Close the file
void FileReader::Close() {
  if (in_.is_open()) in_.close();
}

} // namespace exr

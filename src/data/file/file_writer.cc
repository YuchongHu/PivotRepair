#include "data/file/file_writer.hh"

#include <iostream>

namespace exr {

//Constructor and destructor
FileWriter::FileWriter() = default;

FileWriter::~FileWriter() { if (out_.is_open()) Close(); }

//Open a file
void FileWriter::Open(const Path &path) {
  //Close the file if has opened
  if (out_.is_open()) Close();

  //Create a new file if doesn't exist'
  if (system(("touch " + path).c_str()) == -1) {
    std::cerr << "Create file \"" << path << "\" error" << std::endl;
    exit(-1);
  }

  //Try to open the new file
  out_ = std::fstream(path, std::ios::out | std::ios::in |
                            std::ios::binary);
  if (!out_.is_open()) {
    std::cerr << "Open file \"" << path << "\" error" << std::endl;
    exit(-1);
  }
}

//Write data
void FileWriter::Write(const DataSize &offset, const DataSize &size,
                       void *buf) {
  out_.seekg(offset, std::ios::beg);
  out_.write(static_cast<BufUnit*>(buf), size);
}

//Close and save the file
void FileWriter::Close() {
  out_.close();
}

//Check if the file is opened
bool FileWriter::is_open() { return out_.is_open(); }

} // namespace exr

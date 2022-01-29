#include "local_file_solver.hh"

#include <fstream>
#include <iostream>

LocalFileSolver::LocalFileSolver(const std::string &_load_ad, const std::string &_store_ad)
    : load_ad{_load_ad}, store_ad{_store_ad} {}

LocalFileSolver::~LocalFileSolver() = default;

void LocalFileSolver::read(void *buf, ssize_t size)
{
  char *buffer = static_cast<char *>(buf);
  std::fstream in(load_ad, std::ios::in | std::ios::binary);
  if (!in.is_open()) {
    std::cerr << "open file error" << std::endl;
    std::cerr << load_ad << std::endl;
    std::cerr << store_ad << std::endl;
    exit(-1);
  }

  //check if the file is enough for the size
  ssize_t begin, end;
  begin = in.tellg();
  in.seekg(0, std::ios::end);
  end = in.tellg();
  if (end - begin < size) {
    in.close();
    std::cerr << "the load file is too small, only: " << end - begin
              << "needed: " << size << std::endl;
    exit(-1);
  }

  //read and get the data
  in.seekg(0, std::ios::beg);
  in.read(buffer, size);
  in.close();
}

void LocalFileSolver::write(void *buf, ssize_t size)
{
  char *buffer = static_cast<char *>(buf);
  std::fstream out(store_ad, std::ios::out | std::ios::binary);
  if (!out.is_open()) {
    std::cerr << "open file error" << std::endl;
    exit(-1);
  }

  //write data into the file
  out.write(buffer, size);
  out.close();
}
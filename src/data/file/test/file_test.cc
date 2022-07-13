#include <iostream>
#include <memory>

#include "data/file/file_reader.hh"
#include "data/file/file_writer.hh"
#include "util/typedef.hh"

int main()
{
  exr::Path path = "src/data/file/test/file_test.txt";

  exr::FileWriter writer;
  exr::FileReader reader;

  char a[100] = "Hello, hello..";
  exr::DataSize size = 14;
  exr::DataSize offset = 5;

  //Write
  writer.Open(path);
  writer.Write(offset, size, a);
  writer.Close();
  std::cout << "Writen to file: \"" << path << "\"" << std::endl
            << "  with content: \"" << a << "\"" << std::endl
            << "     by offset: " << offset << std::endl
            << "       of size: " << size << std::endl << std::endl;

  //Read
  reader.Open(path);
  char b[100] = "";

  // from begining
  offset = 0;
  reader.SetOffset(offset);
  auto s = reader.Read(size, b);
  std::cout << "Read from file: \"" << path << "\"" << std::endl
            << "  using offset: " << offset << std::endl
            << "   trying size: " << size << std::endl
            << "   actual size: " << s << std::endl
            << "       content: \"";
  std::cout.write(b, s);
  std::cout << "\"" << std::endl << std::endl;

  // from 12
  offset = 12;
  reader.SetOffset(offset);
  s = reader.Read(size, b);
  std::cout << "Read from file: \"" << path << "\"" << std::endl
            << "  using offset: " << offset << std::endl
            << "   trying size: " << size << std::endl
            << "   actual size: " << s << std::endl
            << "  with content: \"";
  std::cout.write(b, s);
  std::cout << "\"" << std::endl << std::endl;

  std::cout << "Test ended." << std::endl;
  return 0;
}

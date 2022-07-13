#include <iostream>

#include "config/alg_loader.hh"
#include "util/typedef.hh"

int main()
{
  const exr::Path task_path = "config/tasks.txt",
                  band_path = "config/bandwidths.txt";
  const exr::Path alg_path = "src/config/test/algorithms.txt";

  //Init
  exr::AlgLoader al(task_path, band_path);
  al.Open(alg_path);

  //Load & print
  while (al.LoadNext()) {
    std::cout << "Loaded new:" << std::endl
              << "    alg:  " << al.GetAlg() << std::endl
              << "    path: " << al.GetPath() << std::endl;
    if (al.GetArgs())
      std::cout << "  with args." << std::endl << std::endl;
    else
      std::cout << "  with no arg." << std::endl << std::endl;
  }

  //Close
  al.Close();
  return 0;
}

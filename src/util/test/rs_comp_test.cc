#include <iostream>
#include <memory>

#include "util/rs_computer.hh"

int main()
{
  int a[3] = {619, 1103, 410}, b[3] = {1, 3, 5}, c[3], d[3];

  //Get Encode Coefs
  exr::RSComputer rc(4, 2);
  rc.InitForDecode();

  exr::RSUnit coefs[4];
  exr::Count src[2] = {0, 1};
  exr::Count tar[2] = {2, 3};

  rc.Decode(2, tar, src, coefs);
  std::cout << "encoded coefs:\n\t";
  for (int i = 0; i < 4; ++i)
    std::cout << static_cast<int>(coefs[i]) << " ";
  std::cout << std::endl;

  //Encode
  exr::RSComputer rc2(2, 2);
  rc2.InitForEncode(coefs);
  int *srcs[2] = {a, b};
  int *tars[2] = {c, d};
  rc2.Encode(3 * sizeof(int), srcs, tars);

  std::cout << "encode:" << std::endl
            << "\ta: " << a[0] << " " << a[1] << " " << a[2] << std::endl
            << "\tb: " << b[0] << " " << b[1] << " " << b[2] << std::endl
            << "to:" << std::endl
            << "\tc: " << c[0] << " " << c[1] << " " << c[2] << std::endl
            << "\td: " << d[0] << " " << d[1] << " " << d[2] << std::endl;
  std::cout << "coefs after encode:" << std::endl << "\t";
  for (int i = 0; i < 4; ++i)
    std::cout << static_cast<int>(coefs[i]) << " ";
  std::cout << std::endl;

  //Repair
  int e[3];

  exr::RSUnit coefs2[2];
  exr::Count src2[2] = {1, 3};
  exr::Count tar2[2] = {0};
  rc.Decode(1, tar2, src2, coefs2);

  exr::RSComputer rc3(2, 1);
  rc3.InitForEncode(coefs2);
  int *srcs2[2] = {b, d};
  int *tars2[1] = {e};
  rc3.Encode(3 * sizeof(int), srcs2, tars2);

  std::cout << std::endl << "repair a from b and d:" << std::endl
            << "\te:" << e[0] << " " << e[1] << " " << e[2] << std::endl
            << "coefs:" << std::endl
            << "\t" << static_cast<int>(coefs2[0]) << " "
                    << static_cast<int>(coefs2[1]) << std::endl;
  return 0;
}

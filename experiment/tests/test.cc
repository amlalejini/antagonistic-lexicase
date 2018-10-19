#include <iostream>
#include <vector>
#include <string>

int main() {

  std::vector<size_t> v1(10, 2);
  std::vector<size_t> v2;

  std::string hello("BLAH");
  std::string there = "";

  if (hello) {
    std::cout << "V2" << std::endl;
  }

  // if (v1) {
  //   std::cout << "V1" << std::endl;
  // }

  return 0;
}
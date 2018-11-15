#define CATCH_CONFIG_MAIN
#include "third-party/Catch/single_include/catch.hpp"

#include <iostream>

#include "base/Ptr.h"
#include "base/vector.h"
#include "tools/Random.h"
#include "hardware/BitSorter.h"

#include "../source/Mutators.h"
#include "../source/BitTestOrg.h"

TEST_CASE("BitSorter", "[bitsorters]") {

  emp::BitSorter sorter;
  // Test small n=4 sorter
  sorter.AddCompare(0,1);
  sorter.AddCompare(2,3);
  sorter.AddCompare(0,3);
  sorter.AddCompare(1,2);
  sorter.AddCompare(0,1);
  sorter.AddCompare(2,3);
  REQUIRE(sorter.CountSortable(4) == 16);
  REQUIRE(sorter.IsCorrect(4));
  std::pair<size_t, size_t> pos = {0,1};
  REQUIRE(sorter.GetComparator(0) == pos);

  // Test n=16, size 60 sorter.
  sorter.Clear();
  sorter.AddCompare(7,13);
  sorter.AddCompare(2,15);
  sorter.AddCompare(0,11);
  sorter.AddCompare(10,4);
  sorter.AddCompare(6,1);
  sorter.AddCompare(12,9);
  sorter.AddCompare(7,9);
  sorter.AddCompare(3,14);
  sorter.AddCompare(8,5);
  sorter.AddCompare(12,13);
  sorter.AddCompare(0,3);
  sorter.AddCompare(2,1);
  sorter.AddCompare(0,7);
  sorter.AddCompare(5,4);
  sorter.AddCompare(10,8);
  sorter.AddCompare(8,3);
  sorter.AddCompare(4,1);
  sorter.AddCompare(6,15);
  sorter.AddCompare(10,15);
  sorter.AddCompare(14,11);
  sorter.AddCompare(14,13);
  sorter.AddCompare(12,6);
  sorter.AddCompare(4,7);
  sorter.AddCompare(5,11);
  sorter.AddCompare(9,2);
  sorter.AddCompare(0,1);
  sorter.AddCompare(11,12);
  sorter.AddCompare(15,14);
  sorter.AddCompare(10,13);
  sorter.AddCompare(14,13);
  sorter.AddCompare(2,3);
  sorter.AddCompare(9,8);
  sorter.AddCompare(10,7);
  sorter.AddCompare(5,6);
  sorter.AddCompare(5,1);
  sorter.AddCompare(8,6);
  sorter.AddCompare(2,4);
  sorter.AddCompare(12,9);
  sorter.AddCompare(3,11);
  sorter.AddCompare(12,14);
  sorter.AddCompare(8,11);
  sorter.AddCompare(9,13);
  sorter.AddCompare(9,10);
  sorter.AddCompare(5,4);
  sorter.AddCompare(7,6);
  sorter.AddCompare(12,13);
  sorter.AddCompare(5,3);
  sorter.AddCompare(5,7);
  sorter.AddCompare(1,2);
  sorter.AddCompare(9,8);
  sorter.AddCompare(6,3);
  sorter.AddCompare(6,5);
  sorter.AddCompare(8,6);
  sorter.AddCompare(2,4);
  sorter.AddCompare(7,8);
  sorter.AddCompare(4,3);
  sorter.AddCompare(10,11);
  sorter.AddCompare(9,10);
  sorter.AddCompare(9,8);
  sorter.AddCompare(11,12);
  REQUIRE(sorter.IsCorrect(16));
}

TEST_CASE("Mutators", "[bitsorters]") {
  emp::Random rnd(2);
  BitSorterMutator sorter_mutator;
  sorter_mutator.SORT_SEQ_SIZE = 16;


  emp::BitSorter sorter(sorter_mutator.GenRandomBitSorter(rnd));
  std::cout << "Random bit sorter: " << std::endl;
  std::cout << sorter.AsString() << std::endl;

  // todo - test sorter mutator

  // todo - test bit test mutator
  uint32_t bit_vals = rnd.GetUInt(0, 1 << 16); // This will generate a random bit string
  std::cout << "Bit as number: " << bit_vals << std::endl;
  std::cout << "Bit values: " << std::endl;
  std::cout << emp::BitSorter::ToString(bit_vals, 16) << std::endl;

  emp::BitVector bit_vec = emp::BitSorter::ToBitVector(bit_vals);
  std::cout << "Bit values (as vec): " << std::endl;
  bit_vec.Print(); std::cout << std::endl;
  std::cout << "Bit vec as uint: " << bit_vec.GetUInt(0) << std::endl;

}
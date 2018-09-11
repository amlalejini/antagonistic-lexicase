#define CATCH_CONFIG_MAIN
#include "third-party/Catch/single_include/catch.hpp"

#include <iostream>

#include "base/Ptr.h"
#include "base/vector.h"
#include "tools/Random.h"

#include "SortingNetwork.h"
#include "SortingTest.h"

/*

To test:

- SortingNetwork
  - [ ] Validation function (handwrite some good/bad genomes)
- SortingTest
  - [ ] Evaluate

*/

TEST_CASE("SortingNetwork", "[sorting_network]") {
 
  using network_t = SortingNetwork;

  constexpr int seed = 2;

  emp::Ptr<emp::Random> random = emp::NewPtr<emp::Random>(seed);

  std::cout << "Building a random network (n=16, minsize=8, maxsize=16)." << std::endl;
  network_t network0(*random, 16, 8, 16);

  std::cout << "Network 0 (size=" << network0.GetSize() << "): ";
  network0.Print();
  std::cout << std::endl;
  REQUIRE(network0.Validate(16, 8, 16));

  // Generate many networks, validate.
  for (size_t i = 0; i < 1000; ++i) {
    network_t network(*random, 64, 1, 1024);
    REQUIRE(network.Validate(64, 1, 1024));

    // Copy network
    network_t network_cp(network);
    REQUIRE(network == network_cp);

    // Modify original network
    network[0][0] = 512; network[0][1] = 1024;
    REQUIRE(network.GetNetwork()[0][0] == 512);
    REQUIRE(network.GetNetwork()[0][1] == 1024);
    REQUIRE(network != network_cp);
    REQUIRE(network.Validate(64, 1, 1024) == false);
    // Break network_cp
    network_cp.GetNetwork().resize(2048);
    REQUIRE(network.Validate(64, 1, 1024) == false);
  }

  random.Delete();

}

TEST_CASE("SortingTest", "[sorting_network]") {

  using network_t = SortingNetwork;
  using test_t = SortingTest;

  constexpr int seed = 2;
  emp::Ptr<emp::Random> random = emp::NewPtr<emp::Random>(seed);

  // Network: [(0,1)]
  network_t sorter_n2(1);
  sorter_n2[0] = {0, 1};
  std::cout << "n=2 sorting network: ";
  sorter_n2.Print(); std::cout << std::endl;

  // Test on all possible n=2 test vectors
  test_t test_n2(2);
  REQUIRE(test_n2.GetSize() == 2);
  test_n2[0] = 0;
  test_n2[1] = 0;
  REQUIRE(test_n2.Evaluate(sorter_n2));
  test_n2[0] = 0;
  test_n2[1] = 1;
  REQUIRE(test_n2.Evaluate(sorter_n2));
  test_n2[0] = 1;
  test_n2[1] = 0;
  REQUIRE(test_n2.Evaluate(sorter_n2));
  test_n2[0] = 1;
  test_n2[1] = 1;
  REQUIRE(test_n2.Evaluate(sorter_n2));
  
  
  // Network: [(0,1), (1,2), (0,1)]
  network_t sorter_n3(3);
  sorter_n3[0] = {0,1};
  sorter_n3[1] = {1,2};
  sorter_n3[2] = {0,1};
  std::cout << "n=3 sorting network: ";
  sorter_n3.Print(); std::cout << std::endl;

  // Create a test for n=3
  test_t test_n3(3);
  // Make sure evaluate returns false when sort is incomplete
  test_n3[0] = 1;
  test_n3[1] = 1;
  test_n3[2] = 0;
  REQUIRE(test_n3.Evaluate(sorter_n2) == false);

  // Test n=3 test evaluator
  for (size_t i = 0; i < 100; ++i) {
    test_n3.RandomizeTest(*random);
    REQUIRE(test_n3.Evaluate(sorter_n3));
  }

  // Network: [(0,1), (2,3), (0,3), (1,2), (0,1), (2,3)]
  network_t sorter_n4(6);
  sorter_n4[0] = {0,1};
  sorter_n4[1] = {2,3};
  sorter_n4[2] = {0,3};
  sorter_n4[3] = {1,2};
  sorter_n4[4] = {0,1};
  sorter_n4[5] = {2,3};
  std::cout << "n=4 sorting network: ";
  sorter_n4.Print(); std::cout << std::endl;
  
  // Create a test for n=4
  test_t test_n4(4);

  // Test n=3 evaluator
  for (size_t i = 0; i < 1000; ++i) {
    test_n4.RandomizeTest(*random);
    REQUIRE(test_n4.Evaluate(sorter_n4));      
  }

}

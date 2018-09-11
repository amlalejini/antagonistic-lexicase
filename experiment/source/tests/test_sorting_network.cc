#define CATCH_CONFIG_MAIN
#include "third-party/Catch/single_include/catch.hpp"

#include <iostream>

#include "base/Ptr.h"
#include "base/vector.h"
#include "tools/Random.h"

#include "SortingNetwork.h"

/*

To test:

- [ ] Validation function (handwrite some good/bad genomes)
- [ ] Copy constructor/genome constructor

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

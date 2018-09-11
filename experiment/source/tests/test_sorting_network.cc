#define CATCH_CONFIG_MAIN
#include "third-party/Catch/single_include/catch.hpp"

#include <iostream>

#include "base/Ptr.h"
#include "base/vector.h"
#include "tools/Random.h"

#include "SortingNetworkOrg.h"

/*

To test:

- [ ] Validation function (handwrite some good/bad genomes)
- [ ] Copy constructor/genome constructor

*/

TEST_CASE("SortingNetworkOrg", "[sorting_network]") {
 
  using org_t = SortingNetworkOrg;

  constexpr int seed = 2;

  emp::Ptr<emp::Random> random = emp::NewPtr<emp::Random>(seed);

  std::cout << "Building a random network (n=16, minsize=8, maxsize=16)." << std::endl;
  org_t network0(*random, 16, 8, 16);

  std::cout << "Network 0 (size=" << network0.GetSize() << "): ";
  network0.Print();
  std::cout << std::endl;
  REQUIRE(network0.ValidateNetwork(16, 8, 16));

  // Generate many networks, validate.
  for (size_t i = 0; i < 1000; ++i) {
    org_t org(*random, 64, 0, 1024);
    REQUIRE(org.ValidateNetwork(64, 0, 1024));
  }

  random.Delete();

}

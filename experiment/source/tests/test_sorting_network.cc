#define CATCH_CONFIG_MAIN
#include "third-party/Catch/single_include/catch.hpp"

#include "base/vector.h"
#include "tools/Random.h"

#include "SortingNetworkOrg.h"

TEST_CASE("SortingNetworkOrg_DE", "[sorting_network]") {
 
  using org_t = SortingNetworkOrg_DE;

  org_t network_org(4);

  REQUIRE(network_org.GetInputSize() == 4);

}

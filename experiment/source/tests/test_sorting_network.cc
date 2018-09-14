#define CATCH_CONFIG_MAIN
#include "third-party/Catch/single_include/catch.hpp"

#include <iostream>

#include "base/Ptr.h"
#include "base/vector.h"
#include "tools/Random.h"

#include "Mutators.h"
#include "SortingNetwork.h"
#include "SortingTest.h"
#include "SortingNetworkConfig.h"
#include "SortingNetworkExperiment.h"

/*

To test:

- SortingNetwork
  - [ ] Validation function (handwrite some good/bad genomes)
- SortingTest
  - [ ] Evaluate

*/

/*
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
*/

TEST_CASE("SortingNetworkExperiment", "[sorting_network]") {
  SortingNetworkConfig config;

  config.SEED(2);
  config.NETWORK_POP_SIZE(16);
  config.TEST_POP_SIZE(16);
  config.SORT_SIZE(8);
  config.SORTS_PER_TEST(1);
  config.MAX_NETWORK_SIZE(24);
  config.MIN_NETWORK_SIZE(1);
  config.GENERATIONS(100);

  SortingNetworkExperiment exp;
  exp.Setup(config);

  auto & network_world = exp.GetNetworkWorld();
  auto & test_world = exp.GetTestWorld();

  std::cout << "== Network world population ===" << std::endl;
  for (size_t i = 0; i < config.NETWORK_POP_SIZE(); ++i) {
    network_world[i].Print(); std::cout << std::endl;
  }

  std::cout << "== Test world population ===" << std::endl;
  for (size_t i = 0; i < config.TEST_POP_SIZE(); ++i) {
    test_world[i].Print(); std::cout << std::endl;
  }

  exp.Run();

  std::cout << "== Network world population ===" << std::endl;
  for (size_t i = 0; i < config.NETWORK_POP_SIZE(); ++i) {
    network_world[i].Print(); std::cout << std::endl;
  }

  std::cout << "== Test world population ===" << std::endl;
  for (size_t i = 0; i < config.TEST_POP_SIZE(); ++i) {
    test_world[i].Print(); std::cout << std::endl;
  }

}

/*
TEST_CASE("SortingNetworkMutator", "[sorting_network]") {
  using network_t = SortingNetwork;
  SortingNetworkMutator mutator;
  
  constexpr size_t SEED = 2;

  constexpr size_t INPUT_SIZE = 16;
  constexpr size_t MIN_NETWORK_SIZE = 8;
  constexpr size_t MAX_NETWORK_SIZE = 16;

  emp::Random rand(SEED);

  mutator.MAX_NETWORK_SIZE = MAX_NETWORK_SIZE;
  mutator.MIN_NETWORK_SIZE = MIN_NETWORK_SIZE;
  mutator.SORT_SEQ_SIZE = INPUT_SIZE;
  mutator.PER_INDEX_SUB = 0.0;
  mutator.PER_PAIR_DUP = 0.0;
  mutator.PER_PAIR_INS = 0.0;
  mutator.PER_PAIR_DEL = 0.0;
  mutator.PER_PAIR_SWAP = 0.0;

  // Make sure 0'd out mutator does nothing.
  network_t empty_network(12);
  for (size_t i = 0; i < 100; ++i) {
    network_t mutable_network(empty_network);
    mutator.Mutate(rand, mutable_network);
    REQUIRE(mutable_network == empty_network);  
  }
  // TODO: Have actual REQUIREs here for real tests
  // TODO: make sure mutator produces valid networks
  // TODO: setup index substitution such that it cannot (playing with min/max values) produce same vector
  mutator.PER_INDEX_SUB = 1.0;
  network_t mutable_network(empty_network);
  std::cout << "Pre-mutations:" << std::endl;
  mutable_network.Print(); std::cout << std::endl;
  mutator.Mutate(rand, mutable_network);
  std::cout << "Post-mutations (index subs):" << std::endl;
  mutable_network.Print(); std::cout << std::endl;
  mutator.PER_INDEX_SUB = 0.0;

  mutator.PER_PAIR_INS = 1.0;
  mutator.Mutate(rand, mutable_network);
  std::cout << "Post-mutations (ins):" << std::endl;
  mutable_network.Print(); std::cout << std::endl;
  mutator.PER_PAIR_INS = 0.0;

  mutator.PER_PAIR_DEL = 1.0;
  mutator.Mutate(rand, mutable_network);
  std::cout << "Post-mutations (dels):" << std::endl;
  mutable_network.Print(); std::cout << std::endl;
  mutator.PER_PAIR_DEL = 0.0;

  mutator.PER_PAIR_DUP = 1.0;
  mutator.Mutate(rand, mutable_network);
  std::cout << "Post-mutations (dels):" << std::endl;
  mutable_network.Print(); std::cout << std::endl;
  mutator.PER_PAIR_DUP = 0.0;

  mutator.PER_PAIR_SWAP = 1.0;
  mutator.Mutate(rand, mutable_network);
  std::cout << "Post-mutations (dels):" << std::endl;
  mutable_network.Print(); std::cout << std::endl;
  mutator.PER_PAIR_SWAP = 0.0;
}

// TODO: make this an actual test..
TEST_CASE("SortingTestMutator", "[sorting_network]") {
  using genome_t = SortingTestOrg::genome_t;

  constexpr size_t SEED = 2;
  constexpr size_t TEST_SIZE = 16;
  constexpr size_t NUM_TESTS = 4;
  
  emp::Random rand(SEED);
  
  SortingTestMutator mutator;

  mutator.bit_mode = true;
  mutator.MAX_VALUE = 1;
  mutator.MIN_VALUE = 0;
  mutator.PER_SITE_SUB = 0.0;
  mutator.PER_SEQ_INVERSION = 0.0;

  genome_t tests_genome(TEST_SIZE, NUM_TESTS);
  SortingTestOrg org(tests_genome);
  std::cout << "Pre-mutations:" << std::endl;
  org.Print();


  mutator.PER_SITE_SUB = 1.0;
  mutator.Mutate(rand, org.GetGenome());
  std::cout << "Post-mutations (subs):" << std::endl;
  org.Print();
  mutator.PER_SITE_SUB = 0.0;


  mutator.PER_SITE_SUB = 0.5;
  mutator.Mutate(rand, org.GetGenome());
  std::cout << "Post-mutations (subs):" << std::endl;
  org.Print();
  mutator.PER_SITE_SUB = 0.0;

  mutator.PER_SEQ_INVERSION = 1.0;
  mutator.Mutate(rand, org.GetGenome());
  std::cout << "Post-mutations (invs):" << std::endl;
  org.Print();
  mutator.PER_SEQ_INVERSION = 0.0;


}
*/

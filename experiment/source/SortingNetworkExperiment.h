#ifndef SORTING_NETWORK_EXP_H
#define SORTING_NETWORK_EXP_H

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <utility>

#include "base/Ptr.h"
#include "base/vector.h"
#include "control/Signal.h"
#include "Evolve/World.h"
#include "tools/Random.h"
#include "tools/random_utils.h"
#include "tools/math.h"
#include "tools/string_utils.h"
#include "tools/stats.h"

#include "SortingNetworkConfig.h"
#include "SortingNetworkOrg.h"
#include "SortingTestOrg.h"

/*

SortingNetworkOrgs - Phenotype:

- [test org 0 perf, test org 1 perf, ...]
  - where each performance score is a pass count
- Num passes
- Network size

SortingTestOrgs - Phenotype

- [network org 0 perf, ]

Major components:

- SortingNetworkWorld
- SortingTestWorld

Experiment configuration/setup:

- InitConfigs

...
- SetupMut, SetupFitness

Experiment execution flow:

- (1) Evaluation
- (2) Selection
- (3) World update

*/

class SortingNetworkExperiment {
public:

  // Setup some convenient aliases.
  using network_org_t = SortingNetworkOrg;
  using network_genome_t = SortingNetworkOrg::genome_t;
  
  using test_org_t = SortingTestOrg;
  using test_genome_t = SortingTestOrg::genome_t;
  
  using network_world_t = emp::World<network_org_t>;
  using test_world_t = emp::World<test_org_t>;
  

protected:

  // Localized experiment parameters
  int SEED;
  size_t MAX_NETWORK_SIZE;
  size_t MIN_NETWORK_SIZE;
  size_t TEST_SEQ_SIZE;
  size_t SEQ_PER_TEST;
  size_t NETWORK_POP_SIZE;
  size_t TEST_POP_SIZE;

  // Experiment variables
  bool setup; ///< Has setup been run?

  emp::Ptr<emp::Random> random;

  emp::Ptr<network_world_t> network_world;
  emp::Ptr<test_world_t> test_world;

  // Experiment signals
  emp::Signal<void(void)> do_evaluation_sig;  ///< Trigger network/test evaluations.
  emp::Signal<void(void)> do_selection_sig;   ///< Trigger selection
  emp::Signal<void(void)> do_update_sig;      ///< Trigger world updates

  void InitConfigs(const SortingNetworkConfig & config);

  void InitNetworkPop_Random();
  void InitTestPop_Random();

public:

  SortingNetworkExperiment() : setup(false) { ; }

  ~SortingNetworkExperiment() {
    if (setup) {
      network_world.Delete();
      test_world.Delete();
      random.Delete();
    }
  }

  /// Configure the experiment
  void Setup(const SortingNetworkConfig & config);

  /// Run the experiment start->finish
  /// - Initialize population
  /// - For each generation: RunStep()
  void Run();     

  /// Progress experiment by single time step (generation):
  /// - (1) evaluation population(s)
  /// - (2) select who gets to reproduce
  /// - (3) update world(s)
  void RunStep(); 

  // -- Accessors --
  emp::Random & GetRandom() { return *random; }
  network_world_t & GetNetworkWorld() { return *network_world; }
  test_world_t & GetTestWorld() { return *test_world; }

};

void SortingNetworkExperiment::Setup(const SortingNetworkConfig & config) {
  std::cout << "Running SortingNetworkExperiment setup." << std::endl;
  // Initialize localized configs
  InitConfigs(config);

  if (!setup) {
    // Create a random number generator
    random = emp::NewPtr<emp::Random>(SEED);
    // Create the world(s)
    network_world = emp::NewPtr<network_world_t>(*random, "Network World");
    test_world = emp::NewPtr<test_world_t>(*random, "Sorting Test World");
  } else {
    network_world->Reset();
    test_world->Reset();
  }

  // Initialize populations
  InitNetworkPop_Random();
  InitTestPop_Random();

  setup = true;
}

void SortingNetworkExperiment::Run() {

}

void SortingNetworkExperiment::RunStep() {
  do_evaluation_sig.Trigger();
  do_selection_sig.Trigger();
  do_update_sig.Trigger();
}

void SortingNetworkExperiment::InitConfigs(const SortingNetworkConfig & config) {
  // Default group
  SEED = config.SEED();
  MAX_NETWORK_SIZE = config.MAX_NETWORK_SIZE();
  MIN_NETWORK_SIZE = config.MIN_NETWORK_SIZE();
  TEST_SEQ_SIZE = config.TEST_SEQ_SIZE();
  SEQ_PER_TEST = config.SEQ_PER_TEST();
  NETWORK_POP_SIZE = config.NETWORK_POP_SIZE();
  TEST_POP_SIZE = config.TEST_POP_SIZE();
}

void SortingNetworkExperiment::InitNetworkPop_Random() {
  std::cout << "Randomly initializing sorting network population...";
  // Inject random networks into network world up to population size.
  for (size_t i = 0; i < NETWORK_POP_SIZE; ++i) {
    network_world->Inject(network_genome_t(*random, TEST_SEQ_SIZE, MIN_NETWORK_SIZE, MAX_NETWORK_SIZE), 1);
  }
  std::cout << " Done." << std::endl;
}

void SortingNetworkExperiment::InitTestPop_Random() {
  std::cout << "Random initializing sorting test population...";
  for (size_t i = 0; i < TEST_POP_SIZE; ++i) {
    test_world->Inject(test_genome_t(*random, TEST_SEQ_SIZE, SEQ_PER_TEST), 1);
  }
  std::cout << " Done." << std::endl;
}

#endif
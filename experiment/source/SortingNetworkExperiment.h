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

  // Experiment toggles
  enum EVALUATION_METHODS { RANDOM_COHORT=0, WELL_MIXED=1 };
  
protected:

  // Localized experiment parameters
  int SEED;
  size_t MAX_NETWORK_SIZE;
  size_t MIN_NETWORK_SIZE;
  size_t TEST_SEQ_SIZE;
  size_t SEQ_PER_TEST;
  size_t NETWORK_POP_SIZE;
  size_t TEST_POP_SIZE;
  size_t EVALUATION_METHOD;
  size_t COHORT_SIZE;

  // Experiment variables
  bool setup; ///< Has setup been run?

  emp::Ptr<emp::Random> random;

  emp::Ptr<network_world_t> network_world;
  emp::Ptr<test_world_t> test_world;

  struct Cohorts {
    emp::vector<size_t> population_ids;
    emp::vector<emp::vector<size_t>> cohorts;

    size_t cohort_size;

    size_t GetOrgID(size_t cohortID, size_t memberID) {
      return (cohortID * cohort_size) + memberID;
    }

    size_t GetNumCohorts() const { return cohorts.size(); }
    size_t GetCohortSize() const { return cohort_size; }
    const emp::vector<size_t> & GetCohort(size_t id) const { 
      emp_assert(id < cohorts.size());
      return cohorts[id]; 
    }

    void Init(size_t _pop_size, size_t _cohort_size) {
      emp_assert(_cohort_size > 0);
      cohort_size = _cohort_size;
      const size_t num_cohorts = _pop_size / _cohort_size;
      // Initialize population ids
      for (size_t i = 0; i < _pop_size; ++i) population_ids.emplace_back(i);
      // Initialize cohort vectors
      for (size_t cID = 0; cID < num_cohorts; ++cID) {
        cohorts.emplace_back(cohort_size);
        for (size_t i = 0; i < cohort_size; ++i) cohorts[cID][i] = population_ids[GetOrgID(cID, i)];
      }
    }

    void Randomize(emp::Random & rnd) {
      // Shuffle population ids
      emp::Shuffle(rnd, population_ids);
      // Reassign cohorts
      for (size_t cID = 0; cID < cohorts.size(); ++cID) {
        cohorts.emplace_back(cohort_size);
        for (size_t i = 0; i < cohorts[cID].size(); ++i) cohorts[cID][i] = population_ids[GetOrgID(cID, i)];
      }
    }

    void Print(std::ostream & out=std::cout) const {
      for (size_t cID = 0; cID < cohorts.size(); ++cID) {
        if (cID) out << "\n";
        out << "Cohort[" << cID << "]: [";
        for (size_t i = 0; i < cohorts[cID].size(); ++i) {
          if (i) out << ", ";
          out << cohorts[cID][i];
        }
        out << "]";
      }
    }

  } cohorts;

  // Experiment signals
  emp::Signal<void(void)> do_evaluation_sig;  ///< Trigger network/test evaluations.
  emp::Signal<void(void)> do_selection_sig;   ///< Trigger selection
  emp::Signal<void(void)> do_update_sig;      ///< Trigger world updates

  void InitConfigs(const SortingNetworkConfig & config);

  void InitNetworkPop_Random();
  void InitTestPop_Random();

public:

  SortingNetworkExperiment() : setup(false), cohorts() { ; }

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

  // Setup population evaluation
  switch (EVALUATION_METHOD) {
    case (size_t)EVALUATION_METHODS::RANDOM_COHORT: {
      // Make sure settings abide by expectations.
      emp_assert(NETWORK_POP_SIZE == TEST_POP_SIZE, "Network and test population sizes must match in random cohort evaluation mode.");
      emp_assert(NETWORK_POP_SIZE % COHORT_SIZE == 0, "Population sizes must be evenly divisible by cohort size in random cohort evaluation mode.");
      // Setup cohorts
      cohorts.Init(NETWORK_POP_SIZE, COHORT_SIZE);
      std::cout << "=== Cohorts after initialization ===" << std::endl;
      cohorts.Print();
      std::cout << "\n=========" << std::endl;
      // Setup world to reset phenotypes on organism placement      
      network_world->OnPlacement([this](size_t pos){ network_world->GetOrg(pos).GetPhenotype().Reset(COHORT_SIZE); });
      test_world->OnPlacement([this](size_t pos){ test_world->GetOrg(pos).GetPhenotype().Reset(COHORT_SIZE); });
      // What should happen on evaluation?
      do_evaluation_sig.AddAction([this]() {
        // Randomize the cohorts.
        cohorts.Randomize(*random);
        // For each cohort, evaluate all networks in cohort against all tests in cohort.
        for (size_t cID = 0; cID < cohorts.GetNumCohorts(); ++cID) {
          for (size_t nID = 0; nID < COHORT_SIZE; ++nID) {
            network_org_t & network = network_world->GetOrg(cohorts.GetOrgID(cID, nID));
            for (size_t tID = 0; tID < COHORT_SIZE; ++tID) {
              test_org_t & test = test_world->GetOrg(cohorts.GetOrgID(cID, tID));
              // Evaluate network, nID, 
              const size_t passes = test.Evaluate(network);
              network.GetPhenotype().test_results[tID] = passes;
              test.GetPhenotype().test_results[nID] = passes;
            }
          }
        }
      });
      break;
    }
    case (size_t)EVALUATION_METHODS::WELL_MIXED: {
      break;
    }
    default: {
      std::cout << "Unrecognized EVALUATION_METHOD (" << EVALUATION_METHOD << "). Exiting..." << std::endl;
      exit(-1);
    }
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
  EVALUATION_METHOD = config.EVALUATION_METHOD();
  COHORT_SIZE = config.COHORT_SIZE();
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
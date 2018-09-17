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
#include "Selection.h"
#include "Mutators.h"

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

////////////////////////////////////////////////////////////////////////////////
// TODOS
// - [ ] Make experiment Setup safe to call twice
// - [ ] Setup 'absolute accuracy' (evaluate against all)
// - [ ] Setup 'approximate accuracy' (evaluate against randomly sampled)
// - [ ] Setup data collection
//  - Data files:
//    - Lexicase
//      - Depth
// - [ ] Implement network crossover (as a separate function in mutator - Crossover(p0, p1))
// - [x] Add selective pressure for being short
// - [x] Implement 'print network pop' => snapshot
// - [x] Implement 'print test pop' => snapshot
// - [x] Fix test scores; add more information to phenotype
// - [x] Setup network testing (e.g., coevolution, static, random)
////////////////////////////////////////////////////////////////////////////////


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
  // enum EVALUATION_METHODS { WELL_MIXED=0, RANDOM_COHORT=1 };
  enum SELECTION_METHODS { LEXICASE=0, COHORT_LEXICASE=1, TOURNAMENT=2 };
  enum TEST_MODES { COEVOLVE=0, STATIC=1, RANDOM=2 };
  
protected:

  // Localized experiment parameters
  int SEED;
  size_t GENERATIONS;
  size_t NETWORK_POP_SIZE;
  size_t TEST_POP_SIZE;
  size_t TEST_MODE;
  size_t COHORT_SIZE;

  size_t SELECTION_MODE;
  size_t LEX_MAX_FUNS;
  size_t COHORTLEX_MAX_FUNS;
  size_t TOURNAMENT_SIZE;

  size_t MAX_NETWORK_SIZE;
  size_t MIN_NETWORK_SIZE;

  double PER_INDEX_SUB;
  double PER_PAIR_DUP;
  double PER_PAIR_INS;
  double PER_PAIR_DEL;
  double PER_PAIR_SWAP;

  size_t SORT_SIZE;
  size_t SORTS_PER_TEST;

  double PER_SITE_SUB;
  double PER_SEQ_INVERSION;

  std::string DATA_DIRECTORY;
  size_t SNAPSHOT_INTERVAL;
  size_t DOMINANT_STATS_INTERVAL;

  // Experiment variables
  bool setup;                 ///< Has setup been run?
  size_t update;
  size_t dominant_network_id;
  size_t dominant_test_id;

  emp::Ptr<emp::Random> random;

  emp::Ptr<network_world_t> network_world;
  emp::Ptr<test_world_t> test_world;

  struct Cohorts {
    emp::vector<size_t> population_ids;
    emp::vector<emp::vector<size_t>> cohorts;

    size_t cohort_size;

    /// Used internally to access population_ids
    size_t GetOrgID(size_t cohortID, size_t memberID) const {
      return (cohortID * cohort_size) + memberID;
    }

    /// Used to get world ID of organism #memberID belonging to given cohortID
    size_t GetWorldID(size_t cohortID, size_t memberID) const {
      emp_assert(cohortID < cohorts.size());
      emp_assert(memberID < cohorts[cohortID].size());
      return cohorts[cohortID][memberID];
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
        for (size_t i = 0; i < cohorts[cID].size(); ++i) {
          cohorts[cID][i] = population_ids[GetOrgID(cID, i)];
        }
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

  };

  Cohorts network_cohorts;
  Cohorts test_cohorts;

  emp::vector<std::function<double(network_org_t &)>> lexicase_network_fit_set;
  emp::vector<std::function<double(test_org_t &)>> lexicase_test_fit_set;

  // Mutators
  SortingNetworkMutator network_mutator;
  SortingTestMutator test_mutator;

  struct StatID {
    size_t networkID;
    size_t testID;
    StatID(size_t nID=0, size_t tID=0) : networkID(nID), testID(tID) { ; }
  } curIDs;

  // Network stats
  std::function<size_t(void)> get_networkID;
  std::function<double(void)> get_network_fitness;
  std::function<size_t(void)> get_network_pass_total;
  std::function<size_t(void)> get_network_size;
  std::function<size_t(void)> get_network_antagonist_cnt;
  std::function<size_t(void)> get_network_sorts_per_antagonist;
  std::function<std::string(void)> get_network_passes_by_antagonist;
  std::function<std::string(void)> get_network;
  // Test stats
  std::function<size_t(void)> get_testID;
  std::function<double(void)> get_test_fitness;
  std::function<size_t(void)> get_test_pass_total;
  std::function<size_t(void)> get_test_fail_total;
  std::function<size_t(void)> get_test_size;
  std::function<std::string(void)> get_test_passes_by_antagonist;
  std::function<size_t(void)> get_test_sorts_per_antagonist;
  std::function<std::string(void)> get_test;

  // Data files
  emp::Ptr<emp::DataFile> dom_network_file; 
  emp::Ptr<emp::DataFile> dom_test_file; 

  // Experiment signals
  emp::Signal<void(void)> do_evaluation_sig;  ///< Trigger network/test evaluations.
  emp::Signal<void(void)> do_selection_sig;   ///< Trigger selection
  emp::Signal<void(void)> do_update_sig;      ///< Trigger world updates

  emp::Signal<void(void)> do_pop_snapshot_sig; ///< Trigger population snapshot

  emp::Signal<void(void)> end_setup_sig;    ///< Triggered at beginning of a run.

  void InitConfigs(const SortingNetworkConfig & config);  ///< Initialize (localize) configuration settings.

  void InitNetworkPop_Random(); ///< Randomly initialize network population.
  void InitTestPop_Random();    ///< Randomly initialize testing population.

  void SetupEvaluation();         ///< Setup evaluation for networks and tests.
  void SetupNetworkSelection();   ///< Configure network selection.
  void SetupNetworkMutation();    ///< Configure network mutations.
  void SetupNetworkTesting();     ///< Configure sorting network testing (coevolving vs. static vs. random)
  void SetupTestSelection();      ///< Configure selection for sorting network tests. Only called if co-evolving tests.
  void SetupTestMutation();       ///< Configure mutation for sorting network tests. Only called if coevolving tests.
  void SetupDataCollection();

  void SetupNetworkStats();
  void SetupTestStats();

  void SnapshotNetworks();  ///< Output a snapshot of network population.
  void SnapshotTests();     ///< Output a snapshot of test population.

  void SetupDominantNetworkFile();
  void SetupDominantTestFile();

  /// Evaluate SortingNetworkOrg network against SortingTestOrg test,
  /// return number of passes.
  size_t EvaluateNetworkOrg(const SortingNetworkOrg & network, const SortingTestOrg & test) const;

public:

  SortingNetworkExperiment() 
    : setup(false), update(0), 
      network_cohorts(), test_cohorts(),
      network_mutator(), test_mutator(),
      curIDs(0, 0)
    { ; }

  ~SortingNetworkExperiment() {
    if (setup) {
      dom_network_file.Delete();
      dom_test_file.Delete();
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
    // TODO: clear all of the things that need to be cleared! (signals, fit sets, etc)
  }

  network_world->SetPopStruct_Mixed(true);
  test_world->SetPopStruct_Mixed(true);

  // Add network world updates
  do_update_sig.AddAction([this]() {
    std::cout << "Update: " << update << ", ";
    std::cout << "best-network (size=" << network_world->GetOrg(dominant_network_id).GetSize()<<"): " << network_world->CalcFitnessID(dominant_network_id) << ", ";
    std::cout << "best-test: " << test_world->CalcFitnessID(dominant_test_id) << std::endl;
    
    if (update % SNAPSHOT_INTERVAL == 0) do_pop_snapshot_sig.Trigger();
    
    curIDs.networkID = dominant_network_id;
    curIDs.testID = dominant_test_id;
    dom_network_file->Update(update);
    dom_test_file->Update(update);
    
    network_world->Update();
    network_world->ClearCache();

    test_world->Update();
    test_world->ClearCache();

  });

  end_setup_sig.AddAction([this]() {
    // Initialize populations
    InitNetworkPop_Random();
    InitTestPop_Random();
  });
  
  SetupEvaluation();       // Setup population evaluation
  SetupNetworkSelection(); // Setup network selection
  SetupNetworkMutation();  // Setup network mutations
  SetupNetworkTesting();   // Setup testing mode 
  #ifndef EMSCRIPTEN
  SetupDataCollection();
  #endif 

  // Setup fitness function
  network_world->SetFitFun([this](network_org_t & network) {
    return (double)network.GetPhenotype().num_passes + ((double)(MAX_NETWORK_SIZE - network.GetSize())/(double)MAX_NETWORK_SIZE);
  });

  test_world->SetFitFun([](test_org_t & test) {
    return (double)test.GetPhenotype().num_fails;
  });
  
  end_setup_sig.Trigger();
  setup = true;
  std::cout << "Done with experiment setup." << std::endl;
}

void SortingNetworkExperiment::SetupDataCollection() {
  std::cout << "Setting up data collection!" << std::endl;
  // Make a data directory
  mkdir(DATA_DIRECTORY.c_str(), ACCESSPERMS);
  if (DATA_DIRECTORY.back() != '/') DATA_DIRECTORY += '/';  
  
  // Setup stats getters
  SetupNetworkStats();
  SetupTestStats();

  // Setup dominant data file
  SetupDominantNetworkFile();
  SetupDominantTestFile();

  // Setup fitness files
  // SetupFitnessFile
  
  // Setup systematics file
  // SetupSystematicsFile

  // Setup population file
  // SetupPopulationFile

  // Setup network/test snapshots
  do_pop_snapshot_sig.AddAction([this]() { 
    SnapshotNetworks(); 
    SnapshotTests();
  });
}

void SortingNetworkExperiment::SetupNetworkStats() {
  
  get_networkID = [this]() { return curIDs.networkID; };
  
  get_network_fitness = [this]() { return network_world->CalcFitnessID(curIDs.networkID); };
  
  get_network_pass_total = [this]() { 
    return network_world->GetOrg(curIDs.networkID).GetPhenotype().num_passes; 
  };
  
  get_network_size = [this]() {
    return network_world->GetOrg(curIDs.networkID).GetSize();
  };
  
  get_network_antagonist_cnt = [this]() {
    return network_world->GetOrg(curIDs.networkID).GetPhenotype().test_results.size();
  };
  
  get_network_sorts_per_antagonist = [this]() {
    return SORTS_PER_TEST;
  };
  
  get_network_passes_by_antagonist = [this]() {
    network_org_t & network = network_world->GetOrg(curIDs.networkID);
    std::string scores = "\"[";
    for (size_t i = 0; i < network.GetPhenotype().test_results.size(); ++i) {
      if (i) scores += ",";
      scores += emp::to_string(network.GetPhenotype().test_results[i]);
    }
    scores += "]\"";
    return scores;
  };
  
  get_network = [this]() {
    std::ostringstream stream;
    stream << "\"";
    network_world->GetOrg(curIDs.networkID).GetGenome().Print(stream, ",");
    stream << "\"";
    return stream.str();
  };
}

void SortingNetworkExperiment::SetupTestStats() {
  get_testID = [this]() {
    return curIDs.testID;
  };
  get_test_fitness = [this]() {
    return test_world->CalcFitnessID(curIDs.testID);
  };
  get_test_pass_total = [this]() {
    return test_world->GetOrg(curIDs.testID).GetPhenotype().num_passes;
  };
  get_test_fail_total = [this]() {
    return test_world->GetOrg(curIDs.testID).GetPhenotype().num_fails;
  };
  get_test_size = [this]() {
    return test_world->GetOrg(curIDs.testID).GetTestSize();
  };
  get_test_passes_by_antagonist = [this]() {
    test_org_t & test = test_world->GetOrg(curIDs.testID);
    std::string scores = "\"[";
    for (size_t i = 0; i < test.GetPhenotype().test_results.size(); ++i) {
      if (i) scores += ",";
      scores += emp::to_string(test.GetPhenotype().test_results[i]);
    }
    scores += "]\"";
    return scores;
  };
  get_test_sorts_per_antagonist = [this]() {
    return test_world->GetOrg(curIDs.testID).GetNumTests();
  };
  get_test = [this]() {
    std::ostringstream stream;
    stream << "\"";
    test_world->GetOrg(curIDs.testID).PrintMin(stream);
    stream << "\"";
    return stream.str();
  };
}

void SortingNetworkExperiment::SetupEvaluation() {
  // Setup population evaluation.
  const bool cohort_eval = SELECTION_MODE == SELECTION_METHODS::COHORT_LEXICASE;
  if (cohort_eval) {  // We're evaluating networks with tests in cohorts.
    // Make sure settings abide by expectations.
    emp_assert(NETWORK_POP_SIZE == TEST_POP_SIZE, "Network and test population sizes must match in random cohort evaluation mode.");
    emp_assert(NETWORK_POP_SIZE % COHORT_SIZE == 0, "Population sizes must be evenly divisible by cohort size in random cohort evaluation mode.");
    // Setup cohorts
    network_cohorts.Init(NETWORK_POP_SIZE, COHORT_SIZE);
    test_cohorts.Init(TEST_POP_SIZE, COHORT_SIZE);
    // Setup world to reset phenotypes on organism placement      
    network_world->OnPlacement([this](size_t pos){ network_world->GetOrg(pos).GetPhenotype().Reset(COHORT_SIZE); });
    test_world->OnPlacement([this](size_t pos){ test_world->GetOrg(pos).GetPhenotype().Reset(COHORT_SIZE); });
    // What should happen on evaluation?
    do_evaluation_sig.AddAction([this]() {
      // Randomize the cohorts.
      network_cohorts.Randomize(*random);
      test_cohorts.Randomize(*random);
      // For each cohort, evaluate all networks in cohort against all tests in cohort.
      for (size_t cID = 0; cID < network_cohorts.GetNumCohorts(); ++cID) {
        for (size_t nID = 0; nID < COHORT_SIZE; ++nID) {
          network_org_t & network = network_world->GetOrg(network_cohorts.GetWorldID(cID, nID));
          for (size_t tID = 0; tID < COHORT_SIZE; ++tID) {
            test_org_t & test = test_world->GetOrg(test_cohorts.GetWorldID(cID, tID));
            // Evaluate network, nID, on test, tID.
            const size_t passes = EvaluateNetworkOrg(network, test);
            network.GetPhenotype().test_results[tID] = passes;
            test.GetPhenotype().test_results[nID] = passes;
          }
        }
      }
    });
  } else { // Evaluate all networks on all tests.
    // Setup world to reset phenotypes on organism placement (each phenotype will need spots to store scores against antagonist's pop size)
    network_world->OnPlacement([this](size_t pos){ network_world->GetOrg(pos).GetPhenotype().Reset(TEST_POP_SIZE); });
    test_world->OnPlacement([this](size_t pos){ test_world->GetOrg(pos).GetPhenotype().Reset(NETWORK_POP_SIZE); });
    // What should happen on evaluation?
    do_evaluation_sig.AddAction([this]() {
      for (size_t nID = 0; nID < network_world->GetSize(); ++nID) {
        network_org_t & network = network_world->GetOrg(nID);
        for (size_t tID = 0; tID < test_world->GetSize(); ++tID) {
          test_org_t & test = test_world->GetOrg(tID);
          // Evaluate network, nID, on test, tID.
          const size_t passes = EvaluateNetworkOrg(network, test);
          network.GetPhenotype().test_results[tID] = passes;
          test.GetPhenotype().test_results[nID] = passes;
        }
      }
    });
  }

  // Add evaluation action to calculate num_passes for tests and networks.
  do_evaluation_sig.AddAction([this]() {
    // Sum pass totals for networks.
    dominant_network_id = 0;
    double cur_best = 0;
    for (size_t nID = 0; nID < network_world->GetSize(); ++nID) {
      if (!network_world->IsOccupied(nID)) continue;
      network_org_t & network = network_world->GetOrg(nID);
      const size_t pass_total = emp::Sum(network.GetPhenotype().test_results);
      network.GetPhenotype().num_passes = pass_total;
      if (pass_total > cur_best || nID == 0) {
        dominant_network_id = nID;
        cur_best = pass_total;
      } 
    }
    // Sum pass totals for tests.
    dominant_test_id = 0;
    cur_best = 0;
    for (size_t tID = 0; tID < test_world->GetSize(); ++tID) {
      if (!test_world->IsOccupied(tID)) continue;
      test_org_t & test = test_world->GetOrg(tID);
      const size_t pass_total = emp::Sum(test.GetPhenotype().test_results);
      const size_t fail_total = (test.GetPhenotype().test_results.size() * SORTS_PER_TEST) - pass_total;
      test.GetPhenotype().num_passes = pass_total;
      test.GetPhenotype().num_fails = fail_total;
      if (fail_total > cur_best || tID == 0) { 
        dominant_test_id = tID;
        cur_best = fail_total;
      }
    }
  });

}

void SortingNetworkExperiment::SetupNetworkSelection() {
  // Configure network selection.
  switch (SELECTION_MODE) {
    
    case (size_t)SELECTION_METHODS::COHORT_LEXICASE: {
      // Setup network fit funs
      // - 1 function for every cohort member
      for (size_t i = 0; i < COHORT_SIZE; ++i) {
        lexicase_network_fit_set.push_back([i](network_org_t & network) {
          return network.GetPhenotype().test_results[i];
        });
      }
      // - 1 test case for being small
      // lexicase_network_fit_set.push_back([this](network_org_t & network) {
      //   if (network.GetPhenotype().num_passes) return (double)MAX_NETWORK_SIZE - (double)network.GetSize();
      //   return 0.0;
      // });
      // Add selection action
      emp_assert(COHORT_SIZE * network_cohorts.GetNumCohorts() == NETWORK_POP_SIZE);
      do_selection_sig.AddAction([this]() {
        // For each cohort, run selection
        for (size_t cID = 0; cID < network_cohorts.GetNumCohorts(); ++cID) {
          emp::CohortLexicaseSelect(*network_world, 
                                    lexicase_network_fit_set,
                                    network_cohorts.GetCohort(cID),
                                    COHORT_SIZE,
                                    COHORTLEX_MAX_FUNS);
        }
      });
      break;
    }
    
    case (size_t)SELECTION_METHODS::LEXICASE: {
      // For lexicase selection, one function for every test organism.
      for (size_t i = 0; i < TEST_POP_SIZE; ++i) {
        lexicase_network_fit_set.push_back([i](network_org_t & network) {
          return network.GetPhenotype().test_results[i];
        });
      }
      lexicase_network_fit_set.push_back([this](network_org_t & network) {
        if (network.GetPhenotype().num_passes >= TEST_POP_SIZE) return (double)MAX_NETWORK_SIZE - (double)network.GetSize();
        return 0.0;
      });
      do_selection_sig.AddAction([this]() {
        emp::LexicaseSelect(*network_world,
                            lexicase_network_fit_set,
                            NETWORK_POP_SIZE,
                            LEX_MAX_FUNS
                            );
      });
      break;
    }
    
    case (size_t)SELECTION_METHODS::TOURNAMENT: {
      do_selection_sig.AddAction([this]() {
        emp::TournamentSelect(*network_world, TOURNAMENT_SIZE, NETWORK_POP_SIZE);
      });
      break;
    }

    default: {
      std::cout << "Unrecognized SELECTION_MODE(" << SELECTION_MODE << "). Exiting..." << std::endl;
      exit(-1);
    }

  }
}

void SortingNetworkExperiment::SetupNetworkTesting() {
  
  switch (TEST_MODE) {
    case (size_t)TEST_MODES::COEVOLVE: {
      std::cout << "Setting up network testing - CO-EVOLUTION MODE" << std::endl;
      // Sorting tests coevolve with sorting networks.
      // - Evaluation already done. 
      // - Setup selection
      SetupTestSelection();
      SetupTestMutation();
      break;
    }
    case (size_t)TEST_MODES::STATIC: {
      std::cout << "Setting up network testing - STATIC MODE" << std::endl;
      // Sorting tests are static over time.
      test_world->SetPopStruct_Mixed(false);
      // ... do nothing ...
      break;
    }
    case (size_t)TEST_MODES::RANDOM: {
      std::cout << "Setting up network testing - RANDOM MODE" << std::endl;
      test_world->SetPopStruct_Mixed(false);
      // On world update, randomize test cases.
      do_update_sig.AddAction([this]() {
        for (size_t tID = 0; tID < test_world->GetSize(); ++tID) {
          if (!test_world->IsOccupied(tID)) continue;
          test_world->GetOrg(tID).GetGenome().Randomize(*random);
        }
      });
      break;
    }
    default: {
      std::cout << "Unrecognized TEST_MODE (" << TEST_MODE << "). Exiting..." << std::endl;
      exit(-1);
    }
  }
}

void SortingNetworkExperiment::SetupNetworkMutation() {
  // Setup network mutation functions
  network_mutator.MAX_NETWORK_SIZE = MAX_NETWORK_SIZE;
  network_mutator.MIN_NETWORK_SIZE = MIN_NETWORK_SIZE;
  network_mutator.SORT_SEQ_SIZE = SORT_SIZE;
  network_mutator.PER_INDEX_SUB = PER_INDEX_SUB;
  network_mutator.PER_PAIR_DUP = PER_PAIR_DUP;
  network_mutator.PER_PAIR_INS = PER_PAIR_INS;
  network_mutator.PER_PAIR_DEL = PER_PAIR_DEL;
  network_mutator.PER_PAIR_SWAP = PER_PAIR_SWAP;
  network_world->SetMutFun([this](network_org_t & network, emp::Random & rnd) {
    return network_mutator.Mutate(rnd, network.GetGenome());
  });
  end_setup_sig.AddAction([this]() {
    network_world->SetAutoMutate(); // After we've initialized populations, turn auto-mutate on.
  });
}

void SortingNetworkExperiment::SetupTestSelection() {
  std::cout << "Setting up test selection!" << std::endl;
  // Configure test selection scheme (will match network selection mode).
  switch (SELECTION_MODE) {
    case (size_t)SELECTION_METHODS::COHORT_LEXICASE: {
      // Setup test fit funs
      // - 1 function for every cohort member
      for (size_t i = 0; i < COHORT_SIZE; ++i) {
        lexicase_test_fit_set.push_back([i](test_org_t & test) {
          return test.GetNumTests() - test.GetPhenotype().test_results[i];
        });
      }
      // Add selection action.
      emp_assert(COHORT_SIZE * test_cohorts.GetNumCohorts() == TEST_POP_SIZE);
      do_selection_sig.AddAction([this]() {
        // For each cohort, run selection.
        for (size_t cID = 0; cID < test_cohorts.GetNumCohorts(); ++cID) {
          emp::CohortLexicaseSelect(*test_world,
                                    lexicase_test_fit_set,
                                    test_cohorts.GetCohort(cID),
                                    COHORT_SIZE,
                                    COHORTLEX_MAX_FUNS);
        }
      });
      break;
    }
    case (size_t)SELECTION_METHODS::LEXICASE: {
      // For lexicase selection, one function for every network organism.
      for (size_t i = 0; i < NETWORK_POP_SIZE; ++i) {
        lexicase_test_fit_set.push_back([i](test_org_t & test) {
          return test.GetNumTests() - test.GetPhenotype().test_results[i]; // Incorrect!
        });
      }
      do_selection_sig.AddAction([this]() {
        emp::LexicaseSelect(*test_world, lexicase_test_fit_set, TEST_POP_SIZE, LEX_MAX_FUNS);
      });
      break;
    }
    case (size_t)SELECTION_METHODS::TOURNAMENT: {
      do_selection_sig.AddAction([this]() {
        emp::TournamentSelect(*test_world, TOURNAMENT_SIZE, TEST_POP_SIZE);
      });
      break;
    }
    default: {
      std::cout << "Unrecognized SELECTION_MODE (" << SELECTION_MODE << "). Exiting..." << std::endl;
      exit(-1);
    }
  }
}

void SortingNetworkExperiment::SetupTestMutation() {
  std::cout << "Setting up test mutation!" << std::endl;
  test_mutator.bit_mode = true;
  test_mutator.MAX_VALUE = 1;
  test_mutator.MIN_VALUE = 0;
  test_mutator.PER_SITE_SUB = PER_SITE_SUB;
  test_mutator.PER_SEQ_INVERSION = PER_SEQ_INVERSION;
  test_world->SetMutFun([this](test_org_t & test, emp::Random & rnd) {
    return test_mutator.Mutate(rnd, test.GetGenome());
  });
  end_setup_sig.AddAction([this]() {
    test_world->SetAutoMutate();    // After populations have been initialized, set world to auto mutate.
  });
}

void SortingNetworkExperiment::Run() {
  for (update = 0; update <= GENERATIONS; ++update) {
    RunStep();
  }
}

void SortingNetworkExperiment::RunStep() {
  do_evaluation_sig.Trigger();
  do_selection_sig.Trigger();
  do_update_sig.Trigger();
}

void SortingNetworkExperiment::InitConfigs(const SortingNetworkConfig & config) {
  // Default group
  SEED = config.SEED();
  GENERATIONS = config.GENERATIONS();
  NETWORK_POP_SIZE = config.NETWORK_POP_SIZE();
  TEST_POP_SIZE = config.TEST_POP_SIZE();
  TEST_MODE = config.TEST_MODE();
  COHORT_SIZE = config.COHORT_SIZE();

  SELECTION_MODE = config.SELECTION_MODE();
  LEX_MAX_FUNS = config.LEX_MAX_FUNS();
  COHORTLEX_MAX_FUNS = config.COHORTLEX_MAX_FUNS();
  TOURNAMENT_SIZE = config.TOURNAMENT_SIZE();
  
  MAX_NETWORK_SIZE = config.MAX_NETWORK_SIZE();
  MIN_NETWORK_SIZE = config.MIN_NETWORK_SIZE();

  PER_INDEX_SUB = config.PER_INDEX_SUB();
  PER_PAIR_DUP = config.PER_PAIR_DUP();
  PER_PAIR_INS = config.PER_PAIR_INS();
  PER_PAIR_DEL = config.PER_PAIR_DEL();
  PER_PAIR_SWAP = config.PER_PAIR_SWAP();

  SORT_SIZE = config.SORT_SIZE();
  SORTS_PER_TEST = config.SORTS_PER_TEST();

  PER_SITE_SUB = config.PER_SITE_SUB();
  PER_SEQ_INVERSION = config.PER_SEQ_INVERSION();

  DATA_DIRECTORY = config.DATA_DIRECTORY();
  SNAPSHOT_INTERVAL = config.SNAPSHOT_INTERVAL();
  DOMINANT_STATS_INTERVAL = config.DOMINANT_STATS_INTERVAL();

}

void SortingNetworkExperiment::InitNetworkPop_Random() {
  std::cout << "Randomly initializing sorting network population...";
  // Inject random networks into network world up to population size.
  for (size_t i = 0; i < NETWORK_POP_SIZE; ++i) {
    network_world->Inject(network_genome_t(*random, SORT_SIZE, MIN_NETWORK_SIZE, MAX_NETWORK_SIZE), 1);
  }
  std::cout << " Done." << std::endl;
}

void SortingNetworkExperiment::InitTestPop_Random() {
  std::cout << "Random initializing sorting test population...";
  for (size_t i = 0; i < TEST_POP_SIZE; ++i) {
    test_world->Inject(test_genome_t(*random, SORT_SIZE, SORTS_PER_TEST), 1);
  }
  std::cout << " Done." << std::endl;
}

void SortingNetworkExperiment::SnapshotNetworks() {
  std::string snapshot_dir = DATA_DIRECTORY + "pop_" + emp::to_string(network_world->GetUpdate());
  mkdir(snapshot_dir.c_str(), ACCESSPERMS);
  
  emp::DataFile file(snapshot_dir + "/network_pop_" + emp::to_string((int)network_world->GetUpdate()) + ".csv");

  // Add functions to data file
  // - networkID
  file.AddFun(get_networkID, "network_id", "Network ID");
  // - Fitness
  file.AddFun(get_network_fitness, "fitness", "");
  // - pass total
  file.AddFun(get_network_pass_total, "pass_total", "");
  // - network size
  file.AddFun(get_network_size, "network_size");
  // - num_antagonists
  file.AddFun(get_network_antagonist_cnt, "num_antagonists");
  // - tests per antagonist
  file.AddFun(get_network_sorts_per_antagonist, "sorts_per_antagonist");
  // - antagonist scores]
  file.AddFun(get_network_passes_by_antagonist, "scores_by_antagonist");
  // - network
  file.AddFun(get_network, "network");

  // Output file headers
  file.PrintHeaderKeys();

  // For each network in the population, dump the network and anything we want to know about it.
  for (curIDs.networkID = 0; curIDs.networkID < network_world->GetSize(); ++curIDs.networkID) {
    if (!network_world->IsOccupied(curIDs.networkID)) continue;
    file.Update();
  }
}

void SortingNetworkExperiment::SnapshotTests() {
  std::string snapshot_dir = DATA_DIRECTORY + "pop_" + emp::to_string(test_world->GetUpdate());
  mkdir(snapshot_dir.c_str(), ACCESSPERMS);

  emp::DataFile file(snapshot_dir + "/test_pop_" + emp::to_string((int)test_world->GetUpdate()) + ".csv");  

  file.AddFun(get_testID, "test_id");
  file.AddFun(get_test_fitness, "fitness");
  file.AddFun(get_test_pass_total, "pass_total");
  file.AddFun(get_test_fail_total, "fail_total");
  file.AddFun(get_test_sorts_per_antagonist, "sorts_per_antagonist");
  file.AddFun(get_test_passes_by_antagonist, "passes_by_antagonist");
  file.AddFun(get_test_size, "test_size");
  file.AddFun(get_test, "test");

  // Output file headers
  file.PrintHeaderKeys();

  // For each test in the population, dump the test and anything we want to know about it.
  for (curIDs.testID = 0; curIDs.testID < test_world->GetSize(); ++curIDs.testID) {
    if (!test_world->IsOccupied(curIDs.testID)) continue;
    file.Update();
  }
}

void SortingNetworkExperiment::SetupDominantNetworkFile() {
  
  dom_network_file = emp::NewPtr<emp::DataFile>(DATA_DIRECTORY + "/dominant_network.csv"); 
  dom_network_file->SetTimingRepeat(DOMINANT_STATS_INTERVAL);

  // - Network id
  dom_network_file->AddFun(get_networkID, "network_id", "Network ID");
  // - Fitness
  dom_network_file->AddFun(get_network_fitness, "fitness", "");
  // - pass total
  dom_network_file->AddFun(get_network_pass_total, "pass_total", "");
  // - network size
  dom_network_file->AddFun(get_network_size, "network_size");
  // - num_antagonists
  dom_network_file->AddFun(get_network_antagonist_cnt, "num_antagonists");
  // - tests per antagonist
  dom_network_file->AddFun(get_network_sorts_per_antagonist, "sorts_per_antagonist");
  // - antagonist scores]
  dom_network_file->AddFun(get_network_passes_by_antagonist, "scores_by_antagonist");
  // - network
  dom_network_file->AddFun(get_network, "network");

  dom_network_file->PrintHeaderKeys();
}

void SortingNetworkExperiment::SetupDominantTestFile() {
  dom_test_file = emp::NewPtr<emp::DataFile>(DATA_DIRECTORY + "/dominant_test.csv"); 
  dom_test_file->SetTimingRepeat(DOMINANT_STATS_INTERVAL);

  dom_test_file->AddFun(get_testID, "test_id");
  dom_test_file->AddFun(get_test_fitness, "fitness");
  dom_test_file->AddFun(get_test_pass_total, "pass_total");
  dom_test_file->AddFun(get_test_fail_total, "fail_total");
  dom_test_file->AddFun(get_test_sorts_per_antagonist, "sorts_per_antagonist");
  dom_test_file->AddFun(get_test_passes_by_antagonist, "passes_by_antagonist");
  dom_test_file->AddFun(get_test_size, "test_size");
  dom_test_file->AddFun(get_test, "test");

  dom_test_file->PrintHeaderKeys();
}



size_t SortingNetworkExperiment::EvaluateNetworkOrg(const SortingNetworkOrg & network,
                                                    const SortingTestOrg & test) const {
  size_t passes = 0;
  for (size_t i = 0; i < test.GetNumTests(); ++i) {
    passes += (size_t)(test.GetGenome().test_set[i].Evaluate(network.GetGenome()));
  }
  return passes;                                                    
}


#endif
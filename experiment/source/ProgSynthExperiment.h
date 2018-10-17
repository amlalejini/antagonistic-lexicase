#ifndef SORTING_NETWORK_EXP_H
#define SORTING_NETWORK_EXP_H

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <utility>
#include <unordered_set>
#include <tuple>
#include <variant>
#include <cassert>

#include "base/Ptr.h"
#include "base/vector.h"
#include "control/Signal.h"
#include "Evolve/World.h"
#include "tools/Random.h"
#include "tools/random_utils.h"
#include "tools/math.h"
#include "tools/string_utils.h"
#include "tools/stats.h"
#include "tools/tuple_utils.h"

#include "TagLinearGP.h"
#include "Selection.h"
#include "Mutators.h"

#include "ProgOrg.h"
#include "ProgSynthConfig.h"
#include "ProgSynthBenchmarks_InputReps.h"
#include "TestCaseSet.h"
#include "TagLinearGP.h"
#include "TagLinearGP_InstLib.h"
#include "TagLinearGP_Utilities.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// template <size_t I>
// struct visit_impl
// {
//     template <typename T, typename F>
//     static void visit(T& tup, size_t idx, F fun)
//     {
//         if (idx == I - 1) fun(std::get<I - 1>(tup));
//         else visit_impl<I - 1>::visit(tup, idx, fun);
//     }
// };
  
// template <>
// struct visit_impl<0>
// {
//     template <typename T, typename F>
//     static void visit(T& tup, size_t idx, F fun) { assert(false); }
// };
  
// template <typename F, typename... Ts>
// void visit_at(std::tuple<Ts...> const& tup, size_t idx, F fun)
// {
//     visit_impl<sizeof...(Ts)>::visit(tup, idx, fun);
// }
  
// template <typename F, typename... Ts>
// void visit_at(std::tuple<Ts...>& tup, size_t idx, F fun)
// {
//     visit_impl<sizeof...(Ts)>::visit(tup, idx, fun);
// }
//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////
// --- Notes/Todos ---
// - May need to generate more training examples for problems(?)
// - INSTRUCTION SET
//  - Add LoadAllSetInputs
// - SELECTION
//  - Pools (as in Cliff's implementation of lexicase) (?)
// - Scoring
//  - Assume pass/fail only at first, next add gradient (NOTE - will need to update how we screen/add more things to phenotypes).
//    - Simplest thing to do would be to add a pass_vector + score_vector to test/program phenotypes
//////////////////////////////////////////

constexpr size_t TAG_WIDTH = 32;
constexpr size_t MEM_SIZE = TAG_WIDTH;

// How do training examples change over time?
// - coevolution - training examples co-evolve with programs
// - static - training examples are static
// - random - training examples randomly change over time
enum TRAINING_EXAMPLE_MODE_TYPE { COEVOLUTION=0, STATIC, RANDOM };
enum EVALUATION_TYPE { COHORT=0, FULL=1 };

enum PROBLEM_ID { NumberIO=0,
                  SmallOrLarge,
                  ForLoopIndex,
                  CompareStringLengths,
                  DoubleLetters,
                  CollatzNumbers,
                  ReplaceSpaceWithNewline,
                  StringDifferences,
                  EvenSquares,
                  WallisPi,
                  StringLengthsBackwards,
                  LastIndexOfZero,
                  VectorAverage,
                  CountOdds,
                  MirrorImage,
                  SuperAnagrams,
                  SumOfSquares,
                  VectorsSummed,
                  XWordLines,
                  PigLatin,
                  NegativeToZero,
                  ScrabbleScore,
                  Checksum,
                  Digits,
                  Grade,
                  Median,
                  Smallest,
                  Syllables
};

struct ProblemInfo {
  PROBLEM_ID id; 
  std::string training_fname;
  std::string testing_fname;
  
  ProblemInfo(PROBLEM_ID _id, const std::string & _training_fname, const std::string & _testing_fname) 
    : id(_id), training_fname(_training_fname), testing_fname(_testing_fname)
  { ; }
  
  ProblemInfo(const ProblemInfo &) = default;
  ProblemInfo(ProblemInfo &&) = default;

  ProblemInfo & operator=(const ProblemInfo &) = default;
  ProblemInfo & operator=(ProblemInfo &&) = default;

  const std::string & GetTestingSetFilename() const { return testing_fname; }
  const std::string & GetTrainingSetFilename() const { return training_fname; }

};

std::unordered_map<std::string, ProblemInfo> problems = {
  {"number-io", {PROBLEM_ID::NumberIO, "training-examples-number-io.csv", "testing-examples-number-io.csv"}}
};

class ProgramSynthesisExperiment {
public:
  using hardware_t = typename TagLGP::TagLinearGP_TW<TAG_WIDTH>;
  using inst_lib_t = typename TagLGP::InstLib<hardware_t>;
  using inst_t = typename hardware_t::inst_t;

  using prog_org_t = ProgOrg<TAG_WIDTH>;
  using prog_org_phen_t = typename prog_org_t::Phenotype;

  using prog_world_t = emp::World<prog_org_t>;

  using test_org_phen_t = TestOrg_Base::Phenotype;

  // test world aliases
  using prob_NumberIO_world_t = emp::World<TestOrg_NumberIO>;
  using prob_SmallOrLarge_world_t = emp::World<TestOrg_SmallOrLarge>;
  using prob_ForLoopIndex_world_t = emp::World<TestOrg_ForLoopIndex>;
  using prob_CompareStringLengths_world_t = emp::World<TestOrg_CompareStringLengths>;
  using prob_DoubleLetters_world_t = emp::World<TestOrg_DoubleLetters>;
  using prob_CollatzNumbers_world_t = emp::World<TestOrg_CollatzNumbers>;
  using prob_ReplaceSpaceWithNewline_world_t = emp::World<TestOrg_ReplaceSpaceWithNewline>;
  using prob_StringDifferences_world_t = emp::World<TestOrg_StringDifferences>;
  using prob_EvenSquares_world_t = emp::World<TestOrg_EvenSquares>;
  using prob_WallisPi_world_t = emp::World<TestOrg_WallisPi>;
  using prob_StringLengthsBackwards_world_t = emp::World<TestOrg_StringLengthsBackwards>;
  using prob_LastIndexOfZero_world_t = emp::World<TestOrg_LastIndexOfZero>;
  using prob_VectorAverage_world_t = emp::World<TestOrg_VectorAverage>;
  using prob_CountOdds_world_t = emp::World<TestOrg_CountOdds>;
  using prob_MirrorImage_world_t = emp::World<TestOrg_MirrorImage>;
  using prob_SuperAnagrams_world_t = emp::World<TestOrg_SuperAnagrams>;
  using prob_SumOfSquares_world_t = emp::World<TestOrg_SumOfSquares>;
  using prob_VectorsSummed_world_t = emp::World<TestOrg_VectorsSummed>;
  using prob_XWordLines_world_t = emp::World<TestOrg_XWordLines>;
  using prob_PigLatin_world_t = emp::World<TestOrg_PigLatin>;
  using prob_NegativeToZero_world_t = emp::World<TestOrg_NegativeToZero>;
  using prob_ScrabbleScore_world_t = emp::World<TestOrg_ScrabbleScore>;
  using prob_Checksum_world_t = emp::World<TestOrg_Checksum>;
  using prob_Digits_world_t = emp::World<TestOrg_Digits>;
  using prob_Grade_world_t = emp::World<TestOrg_Grade>;
  using prob_Median_world_t = emp::World<TestOrg_Median>;
  using prob_Smallest_world_t = emp::World<TestOrg_Smallest>;
  using prob_Syllables_world_t = emp::World<TestOrg_Syllables>;

protected:

  // Useful experiment structs
  struct Cohorts {
    protected:
      emp::vector<size_t> population_ids;
      emp::vector<emp::vector<size_t>> cohorts;

      size_t cohort_size;

      bool init;

      /// Used internally to access population_ids
      size_t GetOrgPopID(size_t cohortID, size_t memberID) const {
        emp_assert(init);
        emp_assert(cohortID < cohorts.size());
        emp_assert(memberID < cohorts[cohortID].size());
        return (cohortID * cohort_size) + memberID;
      }

    public:
      Cohorts() : population_ids(), cohorts(), cohort_size(0), init(false) { ; }

      /// Setup cohorts
      void Setup(size_t _pop_size, size_t _cohort_size) {
        emp_assert(_cohort_size > 0);
        population_ids.clear();
        cohorts.clear();
        cohort_size = _cohort_size;
        const size_t num_cohorts = _pop_size / _cohort_size;
        // Initialize population IDS
        for (size_t i = 0; i < _pop_size; ++i) population_ids.emplace_back(i);
        init = true;
        // Initialize cohort vectors
        for (size_t cID = 0; cID < num_cohorts; ++cID) {
          cohorts.emplace_back(cohort_size);
          for (size_t i = 0; i < cohort_size; ++i) cohorts[cID][i] = population_ids[GetOrgPopID(cID, i)];
        }
      }

      size_t GetCohortCnt() const { return cohorts.size(); }
      size_t GetCohortSize() const { return cohort_size; }

      const emp::vector<size_t> & GetCohort(size_t cohortID) const {
        emp_assert(cohortID < cohorts.size());
        return cohorts[cohortID];
      }
      
      /// Used to get world ID of an organism given by its cohort ID and member ID.
      size_t GetWorldID(size_t cohortID, size_t memberID) const {
        emp_assert(init);
        emp_assert(cohortID < cohorts.size());
        emp_assert(memberID < cohorts[cohortID].size());
        return cohorts[cohortID][memberID];
      }

      /// Randomize cohort assignment.
      void Randomize(emp::Random & rnd) {
        // Shuffle population ids
        emp::Shuffle(rnd, population_ids);
        // Reassign cohorts.
        for (size_t cID = 0; cID < cohorts.size(); ++cID) {
          for (size_t i = 0; i < cohort_size; ++i) cohorts[cID][i] = population_ids[GetOrgPopID(cID, i)];
        }
      }

      /// Print cohorts.
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

  // Localized experiment parameters.
  int SEED;
  size_t GENERATIONS;
  size_t PROG_POP_SIZE;
  size_t TEST_POP_SIZE;
  size_t EVALUATION_MODE;
  size_t PROG_COHORT_SIZE;
  size_t TEST_COHORT_SIZE;
  size_t TRAINING_EXAMPLE_MODE;
  std::string PROBLEM;
  std::string BENCHMARK_DATA_DIR;

  size_t MIN_PROG_SIZE;
  size_t MAX_PROG_SIZE;
  size_t PROG_EVAL_TIME;

  double MIN_TAG_SPECIFICITY;
  size_t MAX_CALL_DEPTH;

  double PROB_NUMBER_IO__DOUBLE_MIN;
  double PROB_NUMBER_IO__DOUBLE_MAX;
  int PROB_NUMBER_IO__INT_MIN;
  int PROB_NUMBER_IO__INT_MAX;

  // - Data collection group -
  size_t SNAPSHOT_INTERVAL;
  size_t SOLUTION_SCREEN_INTERVAL;

  // Experiment variables
  bool setup;
  size_t update;

  size_t dominant_prog_id;
  size_t dominant_test_id;

  size_t PROGRAM_MAX_PASSES;
  size_t PROGRAM_EVALUATION_TESTCASE_CNT; ///< How many test cases are organisms evaluated on during evaluation?
  size_t NUM_COHORTS;

  emp::Ptr<emp::Random> random;

  emp::Ptr<inst_lib_t> inst_lib;
  emp::Ptr<hardware_t> eval_hardware;
  size_t eval_time;

  emp::BitSet<TAG_WIDTH> call_tag;
  
  emp::Ptr<prog_world_t> prog_world;

  // Test worlds
  emp::Ptr<prob_NumberIO_world_t> prob_NumberIO_world;
  emp::Ptr<prob_SmallOrLarge_world_t> prob_SmallOrLarge_world;
  emp::Ptr<prob_ForLoopIndex_world_t> prob_ForLoopIndex_world;
  emp::Ptr<prob_CompareStringLengths_world_t> prob_CompareStringLengths_world;
  emp::Ptr<prob_DoubleLetters_world_t> prob_DoubleLetters_world;
  emp::Ptr<prob_CollatzNumbers_world_t> prob_CollatzNumbers_world;
  emp::Ptr<prob_ReplaceSpaceWithNewline_world_t> prob_ReplaceSpaceWithNewline_world;
  emp::Ptr<prob_StringDifferences_world_t> prob_StringDifferences_world;
  emp::Ptr<prob_EvenSquares_world_t> prob_EvenSquares_world;
  emp::Ptr<prob_WallisPi_world_t> prob_WallisPi_world;
  emp::Ptr<prob_StringLengthsBackwards_world_t> prob_StringLengthsBackwards_world;
  emp::Ptr<prob_LastIndexOfZero_world_t> prob_LastIndexOfZero_world;
  emp::Ptr<prob_VectorAverage_world_t> prob_VectorAverage_world;
  emp::Ptr<prob_CountOdds_world_t> prob_CountOdds_world;
  emp::Ptr<prob_MirrorImage_world_t> prob_MirrorImage_world;
  emp::Ptr<prob_SuperAnagrams_world_t> prob_SuperAnagrams_world;
  emp::Ptr<prob_SumOfSquares_world_t> prob_SumOfSquares_world;
  emp::Ptr<prob_VectorsSummed_world_t> prob_VectorsSummed_world;
  emp::Ptr<prob_XWordLines_world_t> prob_XWordLines_world;
  emp::Ptr<prob_PigLatin_world_t> prob_PigLatin_world;
  emp::Ptr<prob_NegativeToZero_world_t> prob_NegativeToZero_world;
  emp::Ptr<prob_ScrabbleScore_world_t> prob_ScrabbleScore_world;
  emp::Ptr<prob_Checksum_world_t> prob_Checksum_world;
  emp::Ptr<prob_Digits_world_t> prob_Digits_world;
  emp::Ptr<prob_Grade_world_t> prob_Grade_world;
  emp::Ptr<prob_Median_world_t> prob_Median_world;
  emp::Ptr<prob_Smallest_world_t> prob_Smallest_world;
  emp::Ptr<prob_Syllables_world_t> prob_Syllables_world;

  // Tuple
  std::tuple<
    emp::Ptr<prob_NumberIO_world_t>,
    emp::Ptr<prob_SmallOrLarge_world_t>,
    emp::Ptr<prob_ForLoopIndex_world_t>,
    emp::Ptr<prob_CompareStringLengths_world_t>,
    emp::Ptr<prob_DoubleLetters_world_t>,
    emp::Ptr<prob_CollatzNumbers_world_t>,
    emp::Ptr<prob_ReplaceSpaceWithNewline_world_t>,
    emp::Ptr<prob_StringDifferences_world_t>,
    emp::Ptr<prob_EvenSquares_world_t>,
    emp::Ptr<prob_WallisPi_world_t>,
    emp::Ptr<prob_StringLengthsBackwards_world_t>,
    emp::Ptr<prob_LastIndexOfZero_world_t>,
    emp::Ptr<prob_VectorAverage_world_t>,
    emp::Ptr<prob_CountOdds_world_t>,
    emp::Ptr<prob_MirrorImage_world_t>,
    emp::Ptr<prob_SuperAnagrams_world_t>,
    emp::Ptr<prob_SumOfSquares_world_t>,
    emp::Ptr<prob_VectorsSummed_world_t>,
    emp::Ptr<prob_XWordLines_world_t>,
    emp::Ptr<prob_PigLatin_world_t>,
    emp::Ptr<prob_NegativeToZero_world_t>,
    emp::Ptr<prob_ScrabbleScore_world_t>,
    emp::Ptr<prob_Checksum_world_t>,
    emp::Ptr<prob_Digits_world_t>,
    emp::Ptr<prob_Grade_world_t>,
    emp::Ptr<prob_Median_world_t>,
    emp::Ptr<prob_Smallest_world_t>,
    emp::Ptr<prob_Syllables_world_t>
  > test_world_tuple;

  // using test_world_var_t = std::variant<emp::Ptr<prob_NumberIO_world_t>,emp::Ptr<prob_SmallOrLarge_world_t>,emp::Ptr<prob_ForLoopIndex_world_t>,emp::Ptr<prob_CompareStringLengths_world_t>,emp::Ptr<prob_DoubleLetters_world_t>,emp::Ptr<prob_CollatzNumbers_world_t>,emp::Ptr<prob_ReplaceSpaceWithNewline_world_t>,emp::Ptr<prob_StringDifferences_world_t>,emp::Ptr<prob_EvenSquares_world_t>,emp::Ptr<prob_WallisPi_world_t>,emp::Ptr<prob_StringLengthsBackwards_world_t>,emp::Ptr<prob_LastIndexOfZero_world_t>,emp::Ptr<prob_VectorAverage_world_t>,emp::Ptr<prob_CountOdds_world_t>,emp::Ptr<prob_MirrorImage_world_t>,emp::Ptr<prob_SuperAnagrams_world_t>,emp::Ptr<prob_SumOfSquares_world_t>,emp::Ptr<prob_VectorsSummed_world_t>,emp::Ptr<prob_XWordLines_world_t>,emp::Ptr<prob_PigLatin_world_t>,emp::Ptr<prob_NegativeToZero_world_t>,emp::Ptr<prob_ScrabbleScore_world_t>,emp::Ptr<prob_Checksum_world_t>,emp::Ptr<prob_Digits_world_t>,emp::Ptr<prob_Grade_world_t>,emp::Ptr<prob_Median_world_t>,emp::Ptr<prob_Smallest_world_t>,emp::Ptr<prob_Syllables_world_t>>;
  // emp::vector<test_world_var_t> test_world_var_vec;
  

  // Problem utilities
  ProblemUtilities_NumberIO prob_utils_NumberIO;

  Cohorts prog_cohorts;
  Cohorts test_cohorts;

  // TODO - TestCaseSet for every input-output pairing type

  /// StatsUtil is useful for managing target program/test during snapshots (gets
  /// captured in lambda).
  struct StatsUtil {
    size_t cur_progID;
    size_t cur_testID;
    StatsUtil(size_t pID=0, size_t tID=0) : cur_progID(pID), cur_testID(tID) { ; }
  } stats_util;

  // Experiment signals
  emp::Signal<void(void)> do_evaluation_sig;
  emp::Signal<void(void)> do_selection_sig;
  emp::Signal<void(void)> do_update_sig;

  emp::Signal<void(void)> do_pop_snapshot_sig;
  
  emp::Signal<void(void)> end_setup_sig;
  emp::Signal<void(void)> on_destruction; ///< Triggered on experiment destruction

  // Program evaluation signals.
  emp::Signal<void(prog_org_t &)> begin_program_eval;
  emp::Signal<void(prog_org_t &)> end_program_eval;

  emp::Signal<void(prog_org_t &, size_t)> begin_program_test;
  emp::Signal<void(prog_org_t &, size_t)> do_program_test;
  emp::Signal<void(prog_org_t &, size_t)> end_program_test;

  emp::Signal<void(prog_org_t &)> do_program_advance;

  // Functions to be setup depending on experiment configuration (e.g., what problem we're solving)
  // std::function<void(void)> InitTestCasePop_TrainingSet;
  std::function<void(void)> UpdateTestCaseWorld;
  std::function<double(prog_org_t &, size_t)> CalcProgramScoreOnTest;
  std::function<test_org_phen_t&(size_t)> GetTestPhenotype;

  // Internal function signatures.
  void InitConfigs(const ProgramSynthesisConfig & config);

  void InitProgPop_Random();    ///< Randomly initialize the program population.
  
  void SetupHardware();         ///< Setup virtual hardware.
  
  void SetupEvaluation();       ///< Setup evaluation

  void SetupDataCollection();   ///< Setup data collection

  void SetupProgramSelection(); ///< Setup program selection scheme
  void SetupProgramMutation();  ///< Setup program mutations

  void SetupProblem();
  void SetupProblem_NumberIO();
  void SetupProblem_SmallOrLarge();
  void SetupProblem_ForLoopIndex();
  void SetupProblem_CompareStringLengths();
  void SetupProblem_DoubleLetters();
  void SetupProblem_CollatzNumbers();
  void SetupProblem_ReplaceSpaceWithNewline();
  void SetupProblem_StringDifferences();
  void SetupProblem_EvenSquares();
  void SetupProblem_WallisPi();
  void SetupProblem_StringLengthsBackwards();
  void SetupProblem_LastIndexOfZero();
  void SetupProblem_VectorAverage();
  void SetupProblem_CountOdds();
  void SetupProblem_MirrorImage();
  void SetupProblem_SuperAnagrams();
  void SetupProblem_SumOfSquares();
  void SetupProblem_VectorsSummed();
  void SetupProblem_XWordLines();
  void SetupProblem_PigLatin();
  void SetupProblem_NegativeToZero();
  void SetupProblem_ScrabbleScore();
  void SetupProblem_Checksum();
  void SetupProblem_Digits();
  void SetupProblem_Grade();
  void SetupProblem_Median();
  void SetupProblem_Smallest();
  void SetupProblem_Syllables();

  // ---- Some useful world-type generic setup functions ----

  // Create a new world and do initial configuration.
  template<typename WORLD_ORG_TYPE>
  void NewTestCaseWorld(emp::Ptr<emp::World<WORLD_ORG_TYPE>> & w, emp::Random & rnd, const std::string & wname) {
    if (w != nullptr) { w.Delete(); }
    w.New(rnd, wname);
    w->SetPopStruct_Mixed(true);
  }

  // template<typename WORLD_TYPE>
  // struct OnPlacement_TestCaseWorld {
  //   static void Run(emp::Ptr<WORLD_TYPE> w) {
  //     if (w == nullptr) {
  //       std::cout << ">> Nullptr!" << std::endl;
  //     } else {
  //       std::cout << ">> Found a world!" << std::endl;
  //       // w->OnPlacement(...)
  //     }
  //   }
  // };

  // // Set on org placement function.
  // void OnPlacement_ActiveTestCaseWorld(const std::function<void(size_t)> & fun) {
  //   // Loop through all test case worlds
  //   // for (size_t i = 0; i < 10; ++i) {
  //   //   visit_at(test_world_tuple, i, [](auto & arg) {
  //   //     if (arg == nullptr) { std::cout << "Nullptr!" << std::endl; }
  //   //     else std::cout << "Found a world!" << std::endl;
  //   //   });
  //   // }
  //   emp::TupleIterateTemplate<decltype(test_world_tuple), OnPlacement_TestCaseWorld>(test_world_tuple);
  // }

  void OnPlacement_ActiveTestCaseWorld(const std::function<void(size_t)> & fun) {
      if (prob_NumberIO_world != nullptr) prob_NumberIO_world->OnPlacement(fun);
      else if (prob_SmallOrLarge_world != nullptr) prob_SmallOrLarge_world->OnPlacement(fun);
      else if (prob_ForLoopIndex_world != nullptr) prob_ForLoopIndex_world->OnPlacement(fun);
      else if (prob_CompareStringLengths_world != nullptr) prob_CompareStringLengths_world->OnPlacement(fun);
      else if (prob_DoubleLetters_world != nullptr) prob_DoubleLetters_world->OnPlacement(fun);
      else if (prob_CollatzNumbers_world != nullptr) prob_CollatzNumbers_world->OnPlacement(fun);
      else if (prob_ReplaceSpaceWithNewline_world != nullptr) prob_ReplaceSpaceWithNewline_world->OnPlacement(fun);
      else if (prob_StringDifferences_world != nullptr) prob_StringDifferences_world->OnPlacement(fun);
      else if (prob_EvenSquares_world != nullptr) prob_EvenSquares_world->OnPlacement(fun);
      else if (prob_WallisPi_world != nullptr) prob_WallisPi_world->OnPlacement(fun);
      else if (prob_StringLengthsBackwards_world != nullptr) prob_StringLengthsBackwards_world->OnPlacement(fun);
      else if (prob_LastIndexOfZero_world != nullptr) prob_LastIndexOfZero_world->OnPlacement(fun);
      else if (prob_VectorAverage_world != nullptr) prob_VectorAverage_world->OnPlacement(fun);
      else if (prob_CountOdds_world != nullptr) prob_CountOdds_world->OnPlacement(fun);
      else if (prob_MirrorImage_world != nullptr) prob_MirrorImage_world->OnPlacement(fun);
      else if (prob_SuperAnagrams_world != nullptr) prob_SuperAnagrams_world->OnPlacement(fun);
      else if (prob_SumOfSquares_world != nullptr) prob_SumOfSquares_world->OnPlacement(fun);
      else if (prob_VectorsSummed_world != nullptr) prob_VectorsSummed_world->OnPlacement(fun);
      else if (prob_XWordLines_world != nullptr) prob_XWordLines_world->OnPlacement(fun);
      else if (prob_PigLatin_world != nullptr) prob_PigLatin_world->OnPlacement(fun);
      else if (prob_NegativeToZero_world != nullptr) prob_NegativeToZero_world->OnPlacement(fun);
      else if (prob_ScrabbleScore_world != nullptr) prob_ScrabbleScore_world->OnPlacement(fun);
      else if (prob_Checksum_world != nullptr) prob_Checksum_world->OnPlacement(fun);
      else if (prob_Digits_world != nullptr) prob_Digits_world->OnPlacement(fun);
      else if (prob_Grade_world != nullptr) prob_Grade_world->OnPlacement(fun);
      else if (prob_Median_world != nullptr) prob_Median_world->OnPlacement(fun);
      else if (prob_Smallest_world != nullptr) prob_Smallest_world->OnPlacement(fun);
      else if (prob_Syllables_world != nullptr) prob_Syllables_world->OnPlacement(fun);
      else { std::cout << "AHH! More than one test case world has been created. Exiting." << std::endl; exit(-1); }
  }
  
  /// Test set is test set to use if doing static initialization
  /// rand gen fun is used to generate random genome
  template<typename WORLD_ORG_TYPE, typename TEST_IN_TYPE, typename TEST_OUT_TYPE>
  void SetupTestCasePop_Init(emp::Ptr<emp::World<WORLD_ORG_TYPE>> w, TestCaseSet<TEST_IN_TYPE, TEST_OUT_TYPE> & test_set,
                             const std::function<typename emp::World<WORLD_ORG_TYPE>::genome_t(void)> & gen_rand_test) {
    // Configure how population should be initialized -- TODO - maybe move this into functor(?)
    if (TRAINING_EXAMPLE_MODE == (size_t)TRAINING_EXAMPLE_MODE_TYPE::STATIC) {
      TEST_POP_SIZE = test_set.GetSize();
      std::cout << ">> In static training example mode, adjusting TEST_POP_SIZE to: " << TEST_POP_SIZE << std::endl;
      end_setup_sig.AddAction([this, w, test_set]() {                     // TODO - test that this is actually working!
        InitTestCasePop_TrainingSet(w, test_set);
      });
    } else {
      end_setup_sig.AddAction([this, w, test_set, gen_rand_test]() {      // TODO - test that this is actually working!
        InitTestCasePop_Random(w, gen_rand_test);
      });
    }
  }

  // Initialize given world's population with training examples in given test case set.
  template<typename WORLD_ORG_TYPE, typename TEST_IN_TYPE, typename TEST_OUT_TYPE>
  void InitTestCasePop_TrainingSet(emp::Ptr<emp::World<WORLD_ORG_TYPE>> w, const TestCaseSet<TEST_IN_TYPE, TEST_OUT_TYPE> & test_set) {
    std::cout << "Initializing test case population from a training set." << std::endl;
    for (size_t i = 0; i < test_set.GetSize(); ++i) {
      w->Inject(test_set.GetInput(i), 1);
    }
  }

  // Initialize given world's population randomly.
  template<typename WORLD_ORG_TYPE>
  void InitTestCasePop_Random(emp::Ptr<emp::World<WORLD_ORG_TYPE>> w, const std::function<typename emp::World<WORLD_ORG_TYPE>::genome_t(void)> & fun) {
    std::cout << "Initializing test case population randomly." << std::endl;
    for (size_t i = 0; i < TEST_POP_SIZE; ++i) {
      w->Inject(fun(), 1);
    }
  }

  // ---- Problem-specific instruction signatures ----
  void Inst_LoadInt_NumberIO(hardware_t & hw, const inst_t & inst);
  void Inst_LoadDouble_NumberIO(hardware_t & hw, const inst_t & inst);
  void Inst_SubmitNum_NumberIO(hardware_t & hw, const inst_t & inst); 

  // ---- Some useful experiment-running functions ----
  double EvaluateTest(prog_org_t & prog_org, size_t testID) {
    begin_program_test.Trigger(prog_org, testID);
    do_program_test.Trigger(prog_org, testID);
    end_program_test.Trigger(prog_org, testID);
    return CalcProgramScoreOnTest(prog_org, testID);
  }

public:
  ProgramSynthesisExperiment() 
    : setup(false), update(0),
      test_world_tuple(prob_NumberIO_world,
                       prob_SmallOrLarge_world,
                       prob_ForLoopIndex_world,
                       prob_CompareStringLengths_world,
                       prob_DoubleLetters_world,
                       prob_CollatzNumbers_world,
                       prob_ReplaceSpaceWithNewline_world,
                       prob_StringDifferences_world,
                       prob_EvenSquares_world,
                       prob_WallisPi_world,
                       prob_StringLengthsBackwards_world,
                       prob_LastIndexOfZero_world,
                       prob_VectorAverage_world,
                       prob_CountOdds_world,
                       prob_MirrorImage_world,
                       prob_SuperAnagrams_world,
                       prob_SumOfSquares_world,
                       prob_VectorsSummed_world,
                       prob_XWordLines_world,
                       prob_PigLatin_world,
                       prob_NegativeToZero_world,
                       prob_ScrabbleScore_world,
                       prob_Checksum_world,
                       prob_Digits_world,
                       prob_Grade_world,
                       prob_Median_world,
                       prob_Smallest_world,
                       prob_Syllables_world
      )

  {
    std::cout << "Problem info:" << std::endl;
    for (const auto & info : problems) {
      std::cout << "  - Problem name: " << info.first << std::endl;
      std::cout << "    - Training examples file: " << info.second.GetTrainingSetFilename() << std::endl;
      std::cout << "    - Testing examples file: " << info.second.GetTestingSetFilename() << std::endl;
    }
  }

  ~ProgramSynthesisExperiment() {
    if (setup) {
      eval_hardware.Delete();
      inst_lib.Delete();
      prog_world.Delete();

      if (prob_NumberIO_world != nullptr) prob_NumberIO_world.Delete();
      if (prob_SmallOrLarge_world != nullptr) prob_SmallOrLarge_world.Delete();
      if (prob_ForLoopIndex_world != nullptr) prob_ForLoopIndex_world.Delete();
      if (prob_CompareStringLengths_world != nullptr) prob_CompareStringLengths_world.Delete();
      if (prob_DoubleLetters_world != nullptr) prob_DoubleLetters_world.Delete();
      if (prob_CollatzNumbers_world != nullptr) prob_CollatzNumbers_world.Delete();
      if (prob_ReplaceSpaceWithNewline_world != nullptr) prob_ReplaceSpaceWithNewline_world.Delete();
      if (prob_StringDifferences_world != nullptr) prob_StringDifferences_world.Delete();
      if (prob_EvenSquares_world != nullptr) prob_EvenSquares_world.Delete();
      if (prob_WallisPi_world != nullptr) prob_WallisPi_world.Delete();
      if (prob_StringLengthsBackwards_world != nullptr) prob_StringLengthsBackwards_world.Delete();
      if (prob_LastIndexOfZero_world != nullptr) prob_LastIndexOfZero_world.Delete();
      if (prob_VectorAverage_world != nullptr) prob_VectorAverage_world.Delete();
      if (prob_CountOdds_world != nullptr) prob_CountOdds_world.Delete();
      if (prob_MirrorImage_world != nullptr) prob_MirrorImage_world.Delete();
      if (prob_SuperAnagrams_world != nullptr) prob_SuperAnagrams_world.Delete();
      if (prob_SumOfSquares_world != nullptr) prob_SumOfSquares_world.Delete();
      if (prob_VectorsSummed_world != nullptr) prob_VectorsSummed_world.Delete();
      if (prob_XWordLines_world != nullptr) prob_XWordLines_world.Delete();
      if (prob_PigLatin_world != nullptr) prob_PigLatin_world.Delete();
      if (prob_NegativeToZero_world != nullptr) prob_NegativeToZero_world.Delete();
      if (prob_ScrabbleScore_world != nullptr) prob_ScrabbleScore_world.Delete();
      if (prob_Checksum_world != nullptr) prob_Checksum_world.Delete();
      if (prob_Digits_world != nullptr) prob_Digits_world.Delete();
      if (prob_Grade_world != nullptr) prob_Grade_world.Delete();
      if (prob_Median_world != nullptr) prob_Median_world.Delete();
      if (prob_Smallest_world != nullptr) prob_Smallest_world.Delete();
      if (prob_Syllables_world != nullptr) prob_Syllables_world.Delete();

      random.Delete();
    }
  }

  /// Configure the experiment.
  void Setup(const ProgramSynthesisConfig & config);

  /// Run the experiment start->finish.
  void Run();

  /// Progress experiment by a single time step (generation):
  /// - (1) evaluate population(s)
  /// - (2) select individuals for reproduction
  /// - (3) update world(s)
  void RunStep();

  /// -- Accessors --
  emp::Random & GetRandom() { return *random; }
  prog_world_t & GetProgramWorld() { return *prog_world; }

};

/// ================ Public facing implementations ================

/// Configure the experiment.
void ProgramSynthesisExperiment::Setup(const ProgramSynthesisConfig & config) {
  std::cout << "Running ProgramSynthesisExperiment setup." << std::endl;
  emp_assert(setup == false, "Can only run setup once because I'm lazy.");
  // Localize experiment configuration.
  InitConfigs(config);
  // Initialization/cleanup depending on whether or not this is the first call to setup.
  if (!setup) { // First call to Setup.
    // Create a random number generator.
    random = emp::NewPtr<emp::Random>(SEED);
    // Create the program world.
    prog_world = emp::NewPtr<prog_world_t>(*random, "program world");
  } else {
    // Fail!
    std::cout << "Setup only allowed once per experiment! (because I'm too lazy to handle multiple setups) Exiting." << std::endl;
    exit(-1);
  }

  // Configure the program world.
  prog_world->SetPopStruct_Mixed(true);
  // Configure how programs should be initialized.
  end_setup_sig.AddAction([this]() {
    // Initialize program population.
    // InitProgPop_Random(); TODO - Add this back in when I've constructed instruction set.
    std::cout << ">> Program population size=" << prog_world->GetSize() << std::endl;
  });
  // Configure On Update signal.
  do_update_sig.AddAction([this]() {
    std::cout << "Update: " << update << ", ";
    std::cout << "best-program score=" << prog_world->CalcFitnessID(dominant_prog_id) << std::endl;

    if (update % SNAPSHOT_INTERVAL == 0) do_pop_snapshot_sig.Trigger();

    prog_world->Update();
    prog_world->ClearCache();

    UpdateTestCaseWorld();
  });

  std::cout << "EXPERIMENT SETUP => Setting up evaluation hardware." << std::endl;
  SetupHardware();
  
  std::cout << "EXPERIMENT SETUP => Setting up problem." << std::endl;
  SetupProblem();  //...many many todos embedded in this one...

  std::cout << "EXPERIMENT SETUP => Setting up evaluation." << std::endl;
  SetupEvaluation();
  
  std::cout << "EXPERIMENT SETUP => Setting up program selection." << std::endl;
  // todo

  #ifndef EMSCRIPTEN
  std::cout << "EXPERIMENT SETUP => Setting up data collection." << std::endl;
  // todo
  #endif

  // Configure fitness function(s)


  end_setup_sig.Trigger();

  setup = true;

  // TODO - assert that only one test world ptr is not nullptr

  std::cout << "EXPERIMENT SETUP => DONE!" << std::endl;

  // std::cout << "--- Instruction set: ---" << std::endl;
  // inst_lib->Print();
}

/// ================ Internal function implementations ================
void ProgramSynthesisExperiment::InitConfigs(const ProgramSynthesisConfig & config) {
  SEED = config.SEED();
  GENERATIONS = config.GENERATIONS();
  PROG_POP_SIZE = config.PROG_POP_SIZE();
  TEST_POP_SIZE = config.TEST_POP_SIZE();
  EVALUATION_MODE = config.EVALUATION_MODE();
  PROG_COHORT_SIZE = config.PROG_COHORT_SIZE();
  TEST_COHORT_SIZE = config.TEST_COHORT_SIZE();
  TRAINING_EXAMPLE_MODE = config.TRAINING_EXAMPLE_MODE();
  PROBLEM = config.PROBLEM();
  BENCHMARK_DATA_DIR = config.BENCHMARK_DATA_DIR();

  // -- Hardware settings --
  MIN_TAG_SPECIFICITY = config.MIN_TAG_SPECIFICITY();
  MAX_CALL_DEPTH = config.MAX_CALL_DEPTH();

  // -- Program settings --
  MIN_PROG_SIZE = config.MIN_PROG_SIZE();
  MAX_PROG_SIZE = config.MAX_PROG_SIZE();
  PROG_EVAL_TIME  = config.PROG_EVAL_TIME();

  // -- Number IO settings --
  PROB_NUMBER_IO__DOUBLE_MIN = config.PROB_NUMBER_IO__DOUBLE_MIN();
  PROB_NUMBER_IO__DOUBLE_MAX = config.PROB_NUMBER_IO__DOUBLE_MAX();
  PROB_NUMBER_IO__INT_MIN = config.PROB_NUMBER_IO__INT_MIN();
  PROB_NUMBER_IO__INT_MAX = config.PROB_NUMBER_IO__INT_MAX();

  SNAPSHOT_INTERVAL = config.SNAPSHOT_INTERVAL();
  SOLUTION_SCREEN_INTERVAL = config.SOLUTION_SCREEN_INTERVAL();

}

void ProgramSynthesisExperiment::SetupProblem() {
  emp_assert(emp::Has(problems, PROBLEM), "Unknown problem!", PROBLEM);

  switch (problems.at(PROBLEM).id) {
    case PROBLEM_ID::NumberIO: { SetupProblem_NumberIO(); break; }
    case PROBLEM_ID::SmallOrLarge: { SetupProblem_SmallOrLarge(); break; }
    case PROBLEM_ID::ForLoopIndex: { SetupProblem_ForLoopIndex(); break; }
    case PROBLEM_ID::CompareStringLengths: { SetupProblem_CompareStringLengths(); break; }
    case PROBLEM_ID::DoubleLetters: { SetupProblem_DoubleLetters(); break; }
    case PROBLEM_ID::CollatzNumbers: { SetupProblem_CollatzNumbers(); break; }
    case PROBLEM_ID::ReplaceSpaceWithNewline: { SetupProblem_ReplaceSpaceWithNewline(); break; }
    case PROBLEM_ID::StringDifferences: { SetupProblem_StringDifferences(); break; }
    case PROBLEM_ID::EvenSquares: { SetupProblem_EvenSquares(); break; }
    case PROBLEM_ID::WallisPi: { SetupProblem_WallisPi(); break; }
    case PROBLEM_ID::StringLengthsBackwards: { SetupProblem_StringLengthsBackwards(); break; }
    case PROBLEM_ID::LastIndexOfZero: { SetupProblem_LastIndexOfZero(); break; }
    case PROBLEM_ID::VectorAverage: { SetupProblem_VectorAverage(); break; }
    case PROBLEM_ID::CountOdds: { SetupProblem_CountOdds(); break; }
    case PROBLEM_ID::MirrorImage: { SetupProblem_MirrorImage(); break; }
    case PROBLEM_ID::SuperAnagrams: { SetupProblem_SuperAnagrams(); break; }
    case PROBLEM_ID::SumOfSquares: { SetupProblem_SumOfSquares(); break; }
    case PROBLEM_ID::VectorsSummed: { SetupProblem_VectorsSummed(); break; }
    case PROBLEM_ID::XWordLines: { SetupProblem_XWordLines(); break; }
    case PROBLEM_ID::PigLatin: { SetupProblem_PigLatin(); break; }
    case PROBLEM_ID::NegativeToZero: { SetupProblem_NegativeToZero(); break; }
    case PROBLEM_ID::ScrabbleScore: { SetupProblem_ScrabbleScore(); break; }
    case PROBLEM_ID::Checksum: { SetupProblem_Checksum(); break; }
    case PROBLEM_ID::Digits: { SetupProblem_Digits(); break; }
    case PROBLEM_ID::Grade: { SetupProblem_Grade(); break; }
    case PROBLEM_ID::Median: { SetupProblem_Median(); break; }
    case PROBLEM_ID::Smallest: { SetupProblem_Smallest(); break; }
    case PROBLEM_ID::Syllables: { SetupProblem_Syllables(); break; }
    default: {
      std::cout << "Unknown problem (" << PROBLEM << "). Exiting." << std::endl;
      exit(-1);
    }
  }
}

void ProgramSynthesisExperiment::SetupHardware() {
  // Create new instruction library.
  inst_lib = emp::NewPtr<inst_lib_t>();
  // Create evaluation hardware.
  eval_hardware = emp::NewPtr<hardware_t>(inst_lib, random);
  // Configure the CPU.
  // - MemSize
  eval_hardware->SetMemSize(MEM_SIZE);
  // - MinBindThresh
  eval_hardware->SetMinTagSpecificity(MIN_TAG_SPECIFICITY);
  // - MaxCallDepth
  eval_hardware->SetMaxCallDepth(MAX_CALL_DEPTH);
  // - MemTags
  //  - HadamardMatrix
  eval_hardware->SetMemTags(GenHadamardMatrix<TAG_WIDTH>());

  // Configure call tag
  call_tag.Clear(); // Set initial call tag to all 0s.

  // Configure default instructions
  // Math
  inst_lib->AddInst("Add", hardware_t::Inst_Add, 3, "wmemANY[C] = wmemNUM[A] + wmemNUM[B]");
  inst_lib->AddInst("Sub", hardware_t::Inst_Sub, 3, "wmemANY[C] = wmemNUM[A] - wmemNUM[B]");
  inst_lib->AddInst("Mult", hardware_t::Inst_Mult, 3, "wmemANY[C] = wmemNUM[A] * wmemNUM[B]");
  inst_lib->AddInst("Div", hardware_t::Inst_Div, 3, "if (wmemNUM[B] != 0) wmemANY[C] = wmemNUM[A] / wmemNUM[B]; else NOP");
  inst_lib->AddInst("Mod", hardware_t::Inst_Mod, 3, "if (wmemNUM[B] != 0) wmemANY[C] = int(wmemNUM[A]) % int(wmemNUM[B]); else NOP");
  inst_lib->AddInst("TestNumEqu", hardware_t::Inst_TestNumEqu, 3, "wmemANY[C] = wmemNUM[A] == wmemNUM[B]");
  inst_lib->AddInst("TestNumNEqu", hardware_t::Inst_TestNumNEqu, 3, "wmemANY[C] = wmemNUM[A] != wmemNUM[B]");
  inst_lib->AddInst("TestNumLess", hardware_t::Inst_TestNumLess, 3, "wmemANY[C] = wmemNUM[A] < wmemNUM[B]");
  inst_lib->AddInst("Floor", hardware_t::Inst_Floor, 1, "wmemNUM[A] = floor(wmemNUM[A])");
  inst_lib->AddInst("Not", hardware_t::Inst_Not, 1, "wmemNUM[A] = !wmemNUM[A]"); 
  inst_lib->AddInst("Inc", hardware_t::Inst_Inc, 1, "wmemNUM[A] = wmemNUM[A] + 1");
  inst_lib->AddInst("Dec", hardware_t::Inst_Dec, 1, "wmemNUM[A] = wmemNUM[A] - 1");
  
  // Memory manipulation
  inst_lib->AddInst("CopyMem", hardware_t::Inst_CopyMem, 2, "wmemANY[B] = wmemANY[A] // Copy mem[A] to mem[B]");
  inst_lib->AddInst("SwapMem", hardware_t::Inst_SwapMem, 2, "swap(wmemANY[A], wmemANY[B])");
  inst_lib->AddInst("Input", hardware_t::Inst_Input, 2, "wmemANY[B] = imemANY[A]");
  inst_lib->AddInst("Output", hardware_t::Inst_Output, 2, "omemANY[B] = wmemANY[A]");
  inst_lib->AddInst("CommitGlobal", hardware_t::Inst_CommitGlobal, 2, "gmemANY[B] = wmemANY[A]");
  inst_lib->AddInst("PullGlobal", hardware_t::Inst_PullGlobal, 2, "wmemANY[B] = gmemANY[A]");
  inst_lib->AddInst("TestMemEqu", hardware_t::Inst_TestMemEqu, 3, "wmemANY[C] = wmemANY[A] == wmemANY[B]");
  inst_lib->AddInst("TestMemNEqu", hardware_t::Inst_TestMemNEqu, 3, "wmemANY[C] = wmemANY[A] != wmemANY[B]");

  // Vector-related instructions
  inst_lib->AddInst("MakeVector", hardware_t::Inst_MakeVector, 3, "");  // TODO - more descriptions
  inst_lib->AddInst("VecGet", hardware_t::Inst_VecGet, 3, "");
  inst_lib->AddInst("VecSet", hardware_t::Inst_VecSet, 3, "");
  inst_lib->AddInst("VecLen", hardware_t::Inst_VecLen, 3, "");
  inst_lib->AddInst("VecAppend", hardware_t::Inst_VecAppend, 3, "");
  inst_lib->AddInst("VecPop", hardware_t::Inst_VecPop, 3, "");
  inst_lib->AddInst("VecRemove", hardware_t::Inst_VecRemove, 3, "");
  inst_lib->AddInst("VecReplaceAll", hardware_t::Inst_VecReplaceAll, 3, "");
  inst_lib->AddInst("VecIndexOf", hardware_t::Inst_VecIndexOf, 3, "");
  inst_lib->AddInst("VecOccurrencesOf", hardware_t::Inst_VecOccurrencesOf, 3, "");
  inst_lib->AddInst("VecReverse", hardware_t::Inst_VecReverse, 3, "");
  inst_lib->AddInst("VecSwapIfLess", hardware_t::Inst_VecSwapIfLess, 3, "");
  inst_lib->AddInst("VecGetFront", hardware_t::Inst_VecGetFront, 3, "");
  inst_lib->AddInst("VecGetBack", hardware_t::Inst_VecGetBack, 3, "");

  // Memory-type
  inst_lib->AddInst("IsStr", hardware_t::Inst_IsStr, 3, "");
  inst_lib->AddInst("IsNum", hardware_t::Inst_IsNum, 3, "");
  inst_lib->AddInst("IsVec", hardware_t::Inst_IsVec, 3, "");

  // Flow control
  inst_lib->AddInst("If", hardware_t::Inst_If, 3, "", {inst_lib_t::InstProperty::BEGIN_FLOW});
  inst_lib->AddInst("IfNot", hardware_t::Inst_IfNot, 3, "", {inst_lib_t::InstProperty::BEGIN_FLOW});
  inst_lib->AddInst("While", hardware_t::Inst_While, 3, "", {inst_lib_t::InstProperty::BEGIN_FLOW});
  inst_lib->AddInst("Countdown", hardware_t::Inst_Countdown, 3, "", {inst_lib_t::InstProperty::BEGIN_FLOW});
  inst_lib->AddInst("Foreach", hardware_t::Inst_Foreach, 3, "", {inst_lib_t::InstProperty::BEGIN_FLOW});
  inst_lib->AddInst("Close", hardware_t::Inst_Close, 3, "", {inst_lib_t::InstProperty::END_FLOW});
  inst_lib->AddInst("Break", hardware_t::Inst_Break, 3, "");
  inst_lib->AddInst("Call", hardware_t::Inst_Call, 3, "");
  inst_lib->AddInst("Routine", hardware_t::Inst_Routine, 3, "");
  inst_lib->AddInst("Return", hardware_t::Inst_Return, 3, "");

  // Module
  inst_lib->AddInst("ModuleDef", hardware_t::Inst_Nop, 3, "", {inst_lib_t::InstProperty::MODULE});

  // Misc
  inst_lib->AddInst("Nop", hardware_t::Inst_Nop, 3, "");

  // Add Terminals [0:16] -- TODO - may want these to be slightly more configurable.
  for (size_t i = 0; i <= 16; ++i) {
    inst_lib->AddInst("Set-" + emp::to_string(i),
      [i](hardware_t & hw, const inst_t & inst) {
        hardware_t::CallState & state = hw.GetCurCallState();
        hardware_t::Memory & wmem = state.GetWorkingMem();
        size_t posA = hw.FindBestMemoryMatch(wmem, inst.arg_tags[0], hw.GetMinTagSpecificity());
        if (!hw.IsValidMemPos(posA)) return; // Do nothing
        wmem.Set(posA, (double)i);
      });
  }

  // What do at beginning of program evaluation (about to be run on potentially many tests)?
  begin_program_eval.AddAction([this](prog_org_t & prog_org) {
    eval_hardware->Reset();
    eval_hardware->SetProgram(prog_org.GetGenome());
  });

  // What do at end of program evaluation (after being run on some number of tests)?
  // end_program_eval - do nothing (?)
  
  // What to do before running program on a single test?
  begin_program_test.AddAction([this](prog_org_t & prog_org, size_t testID) {
    eval_hardware->ResetHardware();
    eval_hardware->CallModule(call_tag, MIN_TAG_SPECIFICITY, true, false);
  });

  // do_program_test
  do_program_test.AddAction([this](prog_org_t & prog_org, size_t testID) {
    for (eval_time = 0; eval_time < PROG_EVAL_TIME; ++eval_time) {
      do_program_advance.Trigger(prog_org);
      if (eval_hardware->GetCallStackSize() == 0) break; // If call stack is ever completely empty, program is done early.
    }
  });

  // end_program_test
  
  // do_program_advance
  do_program_advance.AddAction([this](prog_org_t &) {
    eval_hardware->SingleProcess();
  });

}

void ProgramSynthesisExperiment::SetupEvaluation() {
  switch (EVALUATION_MODE) {
    case (size_t)EVALUATION_TYPE::COHORT: {
      emp_assert(PROG_POP_SIZE % PROG_COHORT_SIZE == 0, "Program population size must be evenly divisible by program cohort size.");
      emp_assert(TEST_POP_SIZE % TEST_COHORT_SIZE == 0, "Test population size must be evenly divisible by test cohort size.");
      std::cout << ">> Setting up cohorts." << std::endl;
      std::cout << "   - Test cohorts" << std::endl;
      test_cohorts.Setup(TEST_POP_SIZE, TEST_COHORT_SIZE);
      std::cout << "   - Program cohorts" << std::endl;
      prog_cohorts.Setup(PROG_POP_SIZE, PROG_COHORT_SIZE);
      std::cout << "     # test cohorts = " << test_cohorts.GetCohortCnt() << std::endl;
      std::cout << "     # program cohorts = " << prog_cohorts.GetCohortCnt() << std::endl;
      if (test_cohorts.GetCohortCnt() != prog_cohorts.GetCohortCnt()) {
        std::cout << "ERROR: Test cohort count must the same as program cohort count in COHORT mode. Exiting." << std::endl;
        exit(-1);
      }
      NUM_COHORTS = prog_cohorts.GetCohortCnt();
      PROGRAM_MAX_PASSES = TEST_COHORT_SIZE;

      // Setup program world on placement response.
      prog_world->OnPlacement([this](size_t pos) {
        prog_world->GetOrg(pos).GetPhenotype().Reset(TEST_COHORT_SIZE);
      });
      
      // Setup test case world on placement response.
      OnPlacement_ActiveTestCaseWorld([this](size_t pos) { GetTestPhenotype(pos).Reset(PROG_COHORT_SIZE); }); //<--back

      // What should happen on evaluation?
      do_evaluation_sig.AddAction([this]() {
        // Randomize the cohorts.
        prog_cohorts.Randomize(*random);
        test_cohorts.Randomize(*random);
        // For each cohort, evaluate all programs against all tests in corresponding cohort.
        for (size_t cID = 0; cID < prog_cohorts.GetCohortCnt(); ++cID) {
          for (size_t pID = 0; pID < PROG_COHORT_SIZE; ++pID) {
            prog_org_t & prog_org = prog_world->GetOrg(prog_cohorts.GetWorldID(cID, pID));
            begin_program_eval.Trigger(prog_org);
            for (size_t tID = 0; tID < TEST_COHORT_SIZE; ++tID) {
              const size_t test_world_id = test_cohorts.GetWorldID(cID, tID);
              double score = EvaluateTest(prog_org, test_world_id);
              test_org_phen_t & test_phen = GetTestPhenotype(test_world_id);
              prog_org_phen_t & prog_phen = prog_org.GetPhenotype();
              // Update test phenotype
              test_phen.test_results[pID] = score;
              prog_phen.test_results[tID] = score;
            }
            end_program_eval.Trigger(prog_org);
          }
        }
      });
      break;
    }
    case (size_t)EVALUATION_TYPE::FULL: {
      // Evaluate all programs on all tests.
      // - Configure world to reset phenotypes on organism placement.
      prog_world->OnPlacement([this](size_t pos) {
        prog_world->GetOrg(pos).GetPhenotype().Reset(TEST_POP_SIZE); // Phenotype should have space for results against all tests
      });
      OnPlacement_ActiveTestCaseWorld([this](size_t pos) { GetTestPhenotype(pos).Reset(PROG_POP_SIZE); });
      PROGRAM_MAX_PASSES = TEST_POP_SIZE;
      // What should happen on evaluation?
      do_evaluation_sig.AddAction([this]() {
        for (size_t pID = 0; pID < PROG_POP_SIZE; ++pID) {
          emp_assert(prog_world->IsOccupied(pID));
          prog_org_t & prog_org = prog_world->GetOrg(pID);
          begin_program_eval.Trigger(prog_org);
          for (size_t tID = 0; tID < TEST_POP_SIZE; ++tID) {
            double score = EvaluateTest(prog_org, tID);
            test_org_phen_t & test_phen = GetTestPhenotype(tID);
            prog_org_phen_t & prog_phen = prog_org.GetPhenotype();
            // Update phenotypes.
            test_phen.test_results[pID] = score;
            prog_phen.test_results[tID] = score;
          }
          end_program_eval.Trigger(prog_org);
        }
      });
      break;
    }
    // case (size_t)EVALUATION_TYPE::POOLS { // TODO - implement pools version
    //   break;
    // }
    default: {
      std::cout << "Unknown EVALUATION_MODE (" << EVALUATION_MODE << "). Exiting." << std::endl;
      exit(-1);
    } 
  }

  // Add generic evaluation action - calculate num_passes/fails for tests and programs.
  do_evaluation_sig.AddAction([this]() {
    // Sum pass totals for programs, find 'dominant' program.
    double cur_best_score = 0;
    // TODO - add correctness test
    for (size_t pID = 0; pID < prog_world->GetSize(); ++pID) {
      emp_assert(prog_world->IsOccupied(pID));
      prog_org_t & prog_org = prog_world->GetOrg(pID);
      const size_t pass_total = emp::Sum(prog_org.GetPhenotype().test_results);
      prog_org.GetPhenotype().num_passes = pass_total;
      // Is this the highest pass total program this generation?
      if (pass_total > cur_best_score || pID == 0) {
        dominant_prog_id = pID;
        cur_best_score = pass_total;
      }
      // At this point, program has been evaluated against all tests. 
      // - TODO - screen for possible solution program.
      // if (pass_total == PROGRAM_MAX_PASSES) 
    }

    // Sum pass/fail totals for tests.
    for (size_t tID = 0; tID < TEST_POP_SIZE; ++tID) {
      test_org_phen_t & test_phen = GetTestPhenotype(tID);
      const size_t pass_total = emp::Sum(test_phen.test_results);
      const size_t fail_total = PROGRAM_MAX_PASSES - pass_total;
      test_phen.num_passes = pass_total;
      test_phen.num_fails = fail_total;
      if (fail_total > cur_best_score || tID == 0) { // NOTE - will need to be updated if switch to gradient fitness functions (will be problem specific whether or not use a gradient).
        dominant_test_id = tID;
        cur_best_score = fail_total;
      }
    }
  });
}

// ================= PROGRAM-RELATED FUNCTIONS ===========
void ProgramSynthesisExperiment::InitProgPop_Random() {
  std::cout << "Randomly initializing program population." << std::endl;
  for (size_t i = 0; i < PROG_POP_SIZE; ++i) {
    prog_world->Inject(TagLGP::GenRandTagGPProgram(*random, inst_lib, MIN_PROG_SIZE, MAX_PROG_SIZE), 1);
  }
}

// ================= PROBLEM SETUPS ======================

void ProgramSynthesisExperiment::SetupProblem_NumberIO() { 
  std::cout << "Setting up problem - NumberIO" << std::endl; 

  // using test_org_t = TestOrg_NumberIO;
  
  // Load testing examples from file (used to evaluate 'true' performance of programs).
  if (BENCHMARK_DATA_DIR.back() != '/') BENCHMARK_DATA_DIR += '/';  
  std::string training_examples_fpath = BENCHMARK_DATA_DIR + problems.at(PROBLEM).GetTrainingSetFilename();  
  std::string testing_examples_fpath = BENCHMARK_DATA_DIR + problems.at(PROBLEM).GetTestingSetFilename();  
  prob_utils_NumberIO.GetTrainingSet().LoadTestCases(training_examples_fpath);
  prob_utils_NumberIO.GetTestingSet().LoadTestCases(testing_examples_fpath);

  std::cout << ">> Training example set size = " << prob_utils_NumberIO.GetTrainingSet().GetSize() << std::endl;
  std::cout << ">> Testing example set size = " << prob_utils_NumberIO.GetTestingSet().GetSize() << std::endl;

  // Setup world.
  NewTestCaseWorld(prob_NumberIO_world, *random, "NumberIO Test Case World");
  
  // Configure how population should be initialized -- TODO - maybe move this into functor(?)
 
  SetupTestCasePop_Init(prob_NumberIO_world, 
                        prob_utils_NumberIO.training_set,
                        [this]() { return GenRandomTestInput_NumberIO(*random, {PROB_NUMBER_IO__INT_MIN, PROB_NUMBER_IO__INT_MAX}, {PROB_NUMBER_IO__DOUBLE_MIN, PROB_NUMBER_IO__DOUBLE_MAX}); } );

  end_setup_sig.AddAction([this]() { std::cout << ">> TestCase world size = " << prob_NumberIO_world->GetSize() << std::endl; });
  
  // Tell world to calculate correct test output (given input) on placement.
  prob_NumberIO_world->OnPlacement([this](size_t pos) { prob_NumberIO_world->GetOrg(pos).CalcOut(); } );

  // Tell experiment how to get test phenotypes.
  GetTestPhenotype = [this](size_t testID) -> test_org_phen_t & {
    emp_assert(prob_NumberIO_world->IsOccupied(testID));
    return prob_NumberIO_world->GetOrg(testID).GetPhenotype();
  };
  
  // Tell experiment how to update the world (i.e., which world to update).
  UpdateTestCaseWorld = [this]() {
    prob_NumberIO_world->Update();
    prob_NumberIO_world->ClearCache();
  };

  // Tell experiment how to configure hardware inputs when running program against a test.
  begin_program_test.AddAction([this](prog_org_t & prog_org, size_t testID) {
    // Reset eval stuff
    // Set current test org.
    prob_utils_NumberIO.cur_eval_test_org = prob_NumberIO_world->GetOrgPtr(testID);
    prob_utils_NumberIO.ResetTestEval();
    emp_assert(eval_hardware->GetMemSize() >= 2);
    // Configure inputs.
    if (eval_hardware->GetCallStackSize()) {
      // Grab some useful references.
      Problem_NumberIO_input_t & input = prob_utils_NumberIO.cur_eval_test_org->GetGenome(); // std::pair<int, double>
      hardware_t::CallState & state = eval_hardware->GetCurCallState();
      hardware_t::Memory & wmem = state.GetWorkingMem();
      // Set hardware input.
      wmem.Set(0, input.first);
      wmem.Set(1, input.second);
    } // TODO - confirm that this is working.
  });

  // Tell experiment how to calculate program score.
  CalcProgramScoreOnTest = [this](prog_org_t & prog_org, size_t testID) {
    if (prob_utils_NumberIO.submitted) {
      return 0.0;
    } else {
      return CalcScorePassFail_NumberIO(prob_NumberIO_world->GetOrg(testID).GetCorrectOut(), prob_utils_NumberIO.submitted_val);
    }
  };

  // Add custom instructions
  // - LoadInteger
  inst_lib->AddInst("LoadInt", [this](hardware_t & hw, const inst_t & inst) {
    this->Inst_LoadInt_NumberIO(hw, inst);
  }, 1, "");
  // - LoadDouble
  inst_lib->AddInst("LoadDouble", [this](hardware_t & hw, const inst_t & inst) {
    this->Inst_LoadDouble_NumberIO(hw, inst);
  }, 1, "");
  // - SubmitNum
  inst_lib->AddInst("SubmitNum", [this](hardware_t & hw, const inst_t & inst) {
    this->Inst_SubmitNum_NumberIO(hw, inst);
  }, 1, "");
}

void ProgramSynthesisExperiment::SetupProblem_SmallOrLarge() { 
  std::cout << "Problem setup not yet implemented... Exiting." << std::endl;
  exit(-1); 
}

void ProgramSynthesisExperiment::SetupProblem_ForLoopIndex() { 
  std::cout << "Problem setup not yet implemented... Exiting." << std::endl;
  exit(-1); 
}

void ProgramSynthesisExperiment::SetupProblem_CompareStringLengths() { 
  std::cout << "Problem setup not yet implemented... Exiting." << std::endl;
  exit(-1); 
}

void ProgramSynthesisExperiment::SetupProblem_DoubleLetters() { 
  std::cout << "Problem setup not yet implemented... Exiting." << std::endl;
  exit(-1); 
}

void ProgramSynthesisExperiment::SetupProblem_CollatzNumbers() { 
  std::cout << "Problem setup not yet implemented... Exiting." << std::endl;
  exit(-1); 
}

void ProgramSynthesisExperiment::SetupProblem_ReplaceSpaceWithNewline() { 
  std::cout << "Problem setup not yet implemented... Exiting." << std::endl;
  exit(-1); 
}

void ProgramSynthesisExperiment::SetupProblem_StringDifferences() { 
  std::cout << "Problem setup not yet implemented... Exiting." << std::endl;
  exit(-1); 
}

void ProgramSynthesisExperiment::SetupProblem_EvenSquares() { 
  std::cout << "Problem setup not yet implemented... Exiting." << std::endl;
  exit(-1); 
}

void ProgramSynthesisExperiment::SetupProblem_WallisPi() { 
  std::cout << "Problem setup not yet implemented... Exiting." << std::endl;
  exit(-1); 
}

void ProgramSynthesisExperiment::SetupProblem_StringLengthsBackwards() { 
  std::cout << "Problem setup not yet implemented... Exiting." << std::endl;
  exit(-1); 
}

void ProgramSynthesisExperiment::SetupProblem_LastIndexOfZero() { 
  std::cout << "Problem setup not yet implemented... Exiting." << std::endl;
  exit(-1); 
}

void ProgramSynthesisExperiment::SetupProblem_VectorAverage() { 
  std::cout << "Problem setup not yet implemented... Exiting." << std::endl;
  exit(-1); 
}

void ProgramSynthesisExperiment::SetupProblem_CountOdds() { 
  std::cout << "Problem setup not yet implemented... Exiting." << std::endl;
  exit(-1); 
}

void ProgramSynthesisExperiment::SetupProblem_MirrorImage() { 
  std::cout << "Problem setup not yet implemented... Exiting." << std::endl;
  exit(-1); 
}

void ProgramSynthesisExperiment::SetupProblem_SuperAnagrams() { 
  std::cout << "Problem setup not yet implemented... Exiting." << std::endl;
  exit(-1); 
}

void ProgramSynthesisExperiment::SetupProblem_SumOfSquares() { 
  std::cout << "Problem setup not yet implemented... Exiting." << std::endl;
  exit(-1); 
}

void ProgramSynthesisExperiment::SetupProblem_VectorsSummed() { 
  std::cout << "Problem setup not yet implemented... Exiting." << std::endl;
  exit(-1); 
}

void ProgramSynthesisExperiment::SetupProblem_XWordLines() { 
  std::cout << "Problem setup not yet implemented... Exiting." << std::endl;
  exit(-1); 
}

void ProgramSynthesisExperiment::SetupProblem_PigLatin() { 
  std::cout << "Problem setup not yet implemented... Exiting." << std::endl;
  exit(-1); 
}

void ProgramSynthesisExperiment::SetupProblem_NegativeToZero() { 
  std::cout << "Problem setup not yet implemented... Exiting." << std::endl;
  exit(-1); 
}

void ProgramSynthesisExperiment::SetupProblem_ScrabbleScore() { 
  std::cout << "Problem setup not yet implemented... Exiting." << std::endl;
  exit(-1); 
}

void ProgramSynthesisExperiment::SetupProblem_Checksum() { 
  std::cout << "Problem setup not yet implemented... Exiting." << std::endl;
  exit(-1); 
}

void ProgramSynthesisExperiment::SetupProblem_Digits() { 
  std::cout << "Problem setup not yet implemented... Exiting." << std::endl;
  exit(-1); 
}

void ProgramSynthesisExperiment::SetupProblem_Grade() { 
  std::cout << "Problem setup not yet implemented... Exiting." << std::endl;
  exit(-1); 
}

void ProgramSynthesisExperiment::SetupProblem_Median() { 
  std::cout << "Problem setup not yet implemented... Exiting." << std::endl;
  exit(-1); 
}

void ProgramSynthesisExperiment::SetupProblem_Smallest() { 
  std::cout << "Problem setup not yet implemented... Exiting." << std::endl;
  exit(-1); 
}

void ProgramSynthesisExperiment::SetupProblem_Syllables() { 
  std::cout << "Problem setup not yet implemented... Exiting." << std::endl;
  exit(-1); 
}


// ================= PROBLEM-SPECIFIC INSTRUCTION IMPLEMENTATIONS ======================

void ProgramSynthesisExperiment::Inst_LoadInt_NumberIO(hardware_t & hw, const inst_t & inst) {
  hardware_t::CallState & state = hw.GetCurCallState();
  hardware_t::Memory & wmem = state.GetWorkingMem();

  // Find arguments
  size_t posA = hw.FindBestMemoryMatch(wmem, inst.arg_tags[0], hw.GetMinTagSpecificity());
  if (!hw.IsValidMemPos(posA)) return;

  emp_assert(prob_utils_NumberIO.cur_eval_test_org != nullptr);
  wmem.Set(posA, prob_utils_NumberIO.cur_eval_test_org->GetTestIntegerInput());
}

void ProgramSynthesisExperiment::Inst_LoadDouble_NumberIO(hardware_t & hw, const inst_t & inst) {
  hardware_t::CallState & state = hw.GetCurCallState();
  hardware_t::Memory & wmem = state.GetWorkingMem();

  // Find arguments
  size_t posA = hw.FindBestMemoryMatch(wmem, inst.arg_tags[0], hw.GetMinTagSpecificity());
  if (!hw.IsValidMemPos(posA)) return;

  emp_assert(prob_utils_NumberIO.cur_eval_test_org != nullptr);
  wmem.Set(posA, prob_utils_NumberIO.cur_eval_test_org->GetTestDoubleInput());
}

void ProgramSynthesisExperiment::Inst_SubmitNum_NumberIO(hardware_t & hw, const inst_t & inst) {
  hardware_t::CallState & state = hw.GetCurCallState();
  hardware_t::Memory & wmem = state.GetWorkingMem();

  // Find arguments
  size_t posA = hw.FindBestMemoryMatch(wmem, inst.arg_tags[0], hw.GetMinTagSpecificity(), hardware_t::MemPosType::NUM);
  if (!hw.IsValidMemPos(posA)) return;

  emp_assert(prob_utils_NumberIO.cur_eval_test_org != nullptr);
  prob_utils_NumberIO.Submit(wmem.AccessVal(posA).GetNum());
}


/*
scratch:
 // if (TRAINING_EXAMPLE_MODE == (size_t)TRAINING_EXAMPLE_MODE_TYPE::STATIC) {
  //   TEST_POP_SIZE = prob_utils_NumberIO.GetTrainingSet().GetSize();
  //   std::cout << "In static training example mode, adjusting TEST_POP_SIZE to: " << TEST_POP_SIZE << std::endl;
  //   end_setup_sig.AddAction([this]() {
  //     InitTestCasePop_TrainingSet(prob_NumberIO_world, prob_utils_NumberIO.GetTrainingSet());
  //   });
  // } else {
  //   end_setup_sig.AddAction([this]() {
  //     InitTestCasePop_Random(prob_NumberIO_world, 
  //       [this]() { 
  //         return GenRandomTestInput_NumberIO(*random, {PROB_NUMBER_IO__INT_MIN, PROB_NUMBER_IO__INT_MAX}, {PROB_NUMBER_IO__DOUBLE_MIN, PROB_NUMBER_IO__DOUBLE_MAX});
  //       });
  //   });
  // }

*/

#endif
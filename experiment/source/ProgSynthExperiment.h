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
#include "Evolve/World_select.h"
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

//////////////////////////////////////////
// --- Notes/Todos ---
// - May need to generate more training examples for problems(?)
// - INSTRUCTION SET
//  - [ ] Add LoadAllSetInputs
// - SELECTION
//  - [ ] Pools (as in Cliff's implementation of lexicase) (?)
// - SCORING
//  - [x] Assume pass/fail only at first, next add gradient (NOTE - will need to update how we screen/add more things to phenotypes).
//    - [x] Simplest thing to do would be to add a pass_vector + score_vector to test/program phenotypes
//  - Add submission test case(?) to encourage submitting *something* ==> For now, not doing this.
//  - [x] Break apart score w/passes
//  - [x] Add selection pressure for small sizes
// - DIAGNOSTICS
//  - [ ] Clean up printing format
// - Cleaning
//  - [ ] Flesh out config comments
//  - [ ] Round out instruction descriptions
// - ISSUES
//  - Memory indexing seems to be the biggest impact on evaluation speed. 
//    Every instruction argument requires a linear scan of memory for best match.
// --------------
// Scratch
// -----
// How do we want to evaluate on the testing set?
// - Evaluate
//////////////////////////////////////////

constexpr size_t TAG_WIDTH = 16;
constexpr size_t MEM_SIZE = TAG_WIDTH;

// How do training examples change over time?
// - coevolution - training examples co-evolve with programs
// - static - training examples are static
// - random - training examples randomly change over time
enum TRAINING_EXAMPLE_MODE_TYPE { COEVOLUTION=0, STATIC, RANDOM };
enum EVALUATION_TYPE { COHORT=0, FULL=1 };
enum SELECTION_TYPE { LEXICASE=0, COHORT_LEXICASE, TOURNAMENT, DRIFT };

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

  size_t PROG_SELECTION_MODE;
  size_t TEST_SELECTION_MODE;
  size_t PROG_LEXICASE_MAX_FUNS;
  size_t PROG_COHORTLEXICASE_MAX_FUNS;
  size_t TEST_LEXICASE_MAX_FUNS;
  size_t TEST_COHORTLEXICASE_MAX_FUNS;
  size_t PROG_TOURNAMENT_SIZE;
  size_t TEST_TOURNAMENT_SIZE;

  size_t MIN_PROG_SIZE;
  size_t MAX_PROG_SIZE;
  size_t PROG_EVAL_TIME;
  double PROG_MUT__PER_BIT_FLIP;
  double PROG_MUT__PER_INST_SUB;
  double PROG_MUT__PER_INST_INS;
  double PROG_MUT__PER_INST_DEL;
  double PROG_MUT__PER_PROG_SLIP;
  double PROG_MUT__PER_MOD_DUP;
  double PROG_MUT__PER_MOD_DEL;

  double MIN_TAG_SPECIFICITY;
  size_t MAX_CALL_DEPTH;

  double PROB_NUMBER_IO__DOUBLE_MIN;
  double PROB_NUMBER_IO__DOUBLE_MAX;
  int PROB_NUMBER_IO__INT_MIN;
  int PROB_NUMBER_IO__INT_MAX;
  double PROB_NUMBER_IO__MUTATION__PER_INT_RATE;
  double PROB_NUMBER_IO__MUTATION__PER_DOUBLE_RATE;

  // - Data collection group -
  std::string DATA_DIRECTORY;
  size_t SUMMARY_STATS_INTERVAL;
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

  size_t smallest_prog_sol_size;
  bool solution_found;

  emp::BitSet<TAG_WIDTH> call_tag;
  
  emp::Ptr<prog_world_t> prog_world;
  
  emp::vector<std::function<double(prog_org_t &)>> lexicase_prog_fit_set;

  TagLGPMutator<TAG_WIDTH> prog_mutator;

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
  
  // Problem utilities
  ProblemUtilities_NumberIO prob_utils_NumberIO;
  ProblemUtilities_SmallOrLarge prob_utils_SmallOrLarge;
  ProblemUtilities_ForLoopIndex prob_utils_ForLoopIndex;
  ProblemUtilities_CompareStringLengths prob_utils_CompareStringLengths;
  ProblemUtilities_DoubleLetters prob_utils_DoubleLetters;
  ProblemUtilities_CollatzNumbers prob_utils_CollatzNumbers;
  ProblemUtilities_ReplaceSpaceWithNewline prob_utils_ReplaceSpaceWithNewline;
  ProblemUtilities_StringDifferences prob_utils_StringDifferences;
  ProblemUtilities_EvenSquares prob_utils_EvenSquares;
  ProblemUtilities_WallisPi prob_utils_WallisPi;
  ProblemUtilities_StringLengthsBackwards prob_utils_StringLengthsBackwards;
  ProblemUtilities_LastIndexOfZero prob_utils_LastIndexOfZero;
  ProblemUtilities_VectorAverage prob_utils_VectorAverage;
  ProblemUtilities_CountOdds prob_utils_CountOdds;
  ProblemUtilities_MirrorImage prob_utils_MirrorImage;
  ProblemUtilities_SuperAnagrams prob_utils_SuperAnagrams;
  ProblemUtilities_SumOfSquares prob_utils_SumOfSquares;
  ProblemUtilities_VectorsSummed prob_utils_VectorsSummed;
  ProblemUtilities_XWordLines prob_utils_XWordLines;
  ProblemUtilities_PigLatin prob_utils_PigLatin;
  ProblemUtilities_NegativeToZero prob_utils_NegativeToZero;
  ProblemUtilities_ScrabbleScore prob_utils_ScrabbleScore;
  ProblemUtilities_Checksum prob_utils_Checksum;
  ProblemUtilities_Digits prob_utils_Digits;
  ProblemUtilities_Grade prob_utils_Grade;
  ProblemUtilities_Median prob_utils_Median;
  ProblemUtilities_Smallest prob_utils_Smallest;
  ProblemUtilities_Syllables prob_utils_Syllables;

  Cohorts prog_cohorts;
  Cohorts test_cohorts;

  emp::Ptr<emp::DataFile> solution_file;

  struct TestResult {
    double score;
    bool pass;
    bool sub;
    TestResult(double sc=0, bool p=false, bool sb=false) : score(sc), pass(p), sub(sb) { ; }
  };

  /// StatsUtil is useful for managing target program/test during snapshots (gets
  /// captured in lambda).
  struct StatsUtil {
    size_t cur_progID;
    size_t cur_testID;

    emp::vector<TestResult> current_program__validation__test_results;
    double current_program__validation__total_score;
    size_t current_program__validation__total_passes;
    bool current_program__validation__is_solution;

    StatsUtil(size_t pID=0, size_t tID=0) : cur_progID(pID), cur_testID(tID) { ; }
  } stats_util;

  struct ProgramStats {
    // Generic stuff
    std::function<size_t(void)> get_id;
    std::function<double(void)> get_fitness;

    // Fitness evaluation stats
    std::function<double(void)> get_fitness_eval__total_score;          // - get_fitness_eval__total_score
    std::function<size_t(void)> get_fitness_eval__num_passes;           // - get_fitness_eval__num_passes
    std::function<size_t(void)> get_fitness_eval__num_fails;            // - get_fitness_eval__num_fails
    std::function<size_t(void)> get_fitness_eval__num_tests;            // - get_fitness_eval__num_tests
    std::function<std::string(void)> get_fitness_eval__passes_by_test;  // - get_fitness_eval__passes_by_test

    // program validation stats
    std::function<size_t(void)> get_validation_eval__num_passes;          // - get_validation_eval__num_passes;
    std::function<size_t(void)> get_validation_eval__num_tests;           // - get_validation_eval__num_tests
    std::function<std::string(void)> get_validation_eval__passes_by_test; // - get_validation_eval__passes_by_test
    
    // program 'morphology' stats
    std::function<size_t(void)> get_program_len;
    std::function<std::string(void)> get_program;

  } program_stats;

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

  emp::Signal<void(prog_org_t &, emp::Ptr<TestOrg_Base>)> begin_program_test;
  emp::Signal<void(prog_org_t &, emp::Ptr<TestOrg_Base>)> do_program_test;
  emp::Signal<void(prog_org_t &, emp::Ptr<TestOrg_Base>)> end_program_test;

  emp::Signal<void(prog_org_t &)> do_program_advance;

  // Functions to be setup depending on experiment configuration (e.g., what problem we're solving)
  std::function<void(void)> UpdateTestCaseWorld;
  
  std::function<TestResult(prog_org_t &, size_t)> EvaluateWorldTest;                ///< Evaluate given program org on world test (specified by given ID). Return the test result.
  std::function<TestResult(prog_org_t &, TestOrg_Base &)> CalcProgramResultOnTest;  ///< Calculate the test result for a given program on a given test organism.
  
  std::function<void(prog_org_t &)> DoTestingSetValidation;     ///< Run program on full validation testing set.
  std::function<bool(prog_org_t &)> ScreenForSolution;          ///< Run program on validation testing set. Return true if program is a solution; false otherwise.
  
  std::function<test_org_phen_t&(size_t)> GetTestPhenotype;     ///< Utility function used to get test phenotype of given test (test type agnostic).
  std::function<void(void)> SetupTestMutation;                  ///< Test world configuration utility. To be defined by test setup.
  std::function<void(void)> SetupTestFitFun;                    ///< Test world configuration utility. To be defined by test setup.
  std::function<void(void)> SnapshotTests;                      ///< Snapshot test population.
  

  // Internal function signatures.
  void InitConfigs(const ProgramSynthesisConfig & config);

  void InitProgPop_Random();    ///< Randomly initialize the program population.
  
  void SetupHardware();         ///< Setup virtual hardware.
  void SetupEvaluation();       ///< Setup evaluation
  void SetupSelection();        ///< Setup selection (?)
  void SetupMutation();         ///< Setup mutation (?)
  void SetupFitFuns();
  void SetupDataCollection();   ///< Setup data collection

  void SetupProgramSelection(); ///< Setup program selection scheme
  void SetupProgramMutation();  ///< Setup program mutations
  void SetupProgramFitFun();
  void SetupProgramStats();

  void AddDefaultInstructions(const std::unordered_set<std::string> & includes);

  void SnapshotPrograms();

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

  template<typename WORLD_ORG_TYPE>
  void SetupTestSelection(emp::Ptr<emp::World<WORLD_ORG_TYPE>> w, emp::vector<std::function<double(WORLD_ORG_TYPE &)>> & lexicase_fit_set) {
    std::cout << "Setting up test selection." << std::endl;
    switch (TEST_SELECTION_MODE) {
      case (size_t)SELECTION_TYPE::LEXICASE: {
        emp_assert(EVALUATION_MODE == (size_t)EVALUATION_TYPE::FULL);
        std::cout << "  >> Setting up test LEXICASE selection." << std::endl;
        // Setup lexicase selection.
        // - 1 lexicase function for every program organism.
        for (size_t i = 0; i < PROG_POP_SIZE; ++i) {
          lexicase_fit_set.push_back([i](WORLD_ORG_TYPE & test_org) {
            TestOrg_Base & org = static_cast<TestOrg_Base&>(test_org);
            return (size_t)(!org.GetPhenotype().test_passes[i]);        // NOTE - test case fitness functions do not use gradient; use pass/fail.
          });
        }
        // Add selection action.
        do_selection_sig.AddAction([this, w, lexicase_fit_set]() mutable { // todo - check that capture is working as expected
          emp::LexicaseSelect_NAIVE(*w, lexicase_fit_set, TEST_POP_SIZE, TEST_LEXICASE_MAX_FUNS);
        });
        break;
      }
      case (size_t)SELECTION_TYPE::COHORT_LEXICASE: {
        emp_assert(EVALUATION_MODE == (size_t)EVALUATION_TYPE::COHORT);
        std::cout << "  >> Setting up test COHORT LEXICASE selection." << std::endl;
        // Setup cohort lexicase.
        // - 1 lexicase function for every program cohort member.
        for (size_t i = 0; i < PROG_COHORT_SIZE; ++i) {
          lexicase_fit_set.push_back([i](WORLD_ORG_TYPE & test_org) {
            TestOrg_Base & org = static_cast<TestOrg_Base&>(test_org);
            return (size_t)(!org.GetPhenotype().test_passes[i]);
          });
        }
        // Add selection action.
        emp_assert(TEST_COHORT_SIZE * test_cohorts.GetCohortCnt() == TEST_POP_SIZE);
        do_selection_sig.AddAction([this, w, lexicase_fit_set]() mutable { // todo - check that capture is working as expected
          // For each cohort, run selection.
          for (size_t cID = 0; cID < test_cohorts.GetCohortCnt(); ++cID) {
            emp::CohortLexicaseSelect_NAIVE(*w,
                                            lexicase_fit_set,
                                            test_cohorts.GetCohort(cID),
                                            TEST_COHORT_SIZE,
                                            TEST_COHORTLEXICASE_MAX_FUNS);
          }
        });
        break;
      }
      case (size_t)SELECTION_TYPE::TOURNAMENT: {
        std::cout << "  >> Setting up test TOURNAMENT selection." << std::endl;
        do_selection_sig.AddAction([this, w]() mutable {
          emp::TournamentSelect(*w, TEST_TOURNAMENT_SIZE, TEST_POP_SIZE);
        });
        break;
      }
      case (size_t)SELECTION_TYPE::DRIFT: {
        std::cout << "  >> Setting up test DRIFT selection." << std::endl;
        do_selection_sig.AddAction([this, w]() mutable {
          emp::RandomSelect(*w, TEST_POP_SIZE);
        });
        break;
      }
      default: {
        std::cout << "Unknown TEST_SELECTION_MODE (" << TEST_SELECTION_MODE << "). Exiting." << std::endl;
        exit(-1);
      }
    }
  }

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

public:
  ProgramSynthesisExperiment() 
    : setup(false), update(0), solution_found(false)
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
      solution_file.Delete();
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

  smallest_prog_sol_size = MAX_PROG_SIZE + 1;
  solution_found = false;

  // Configure the program world.
  prog_world->SetPopStruct_Mixed(true);
  
  // Configure how programs should be initialized.
  end_setup_sig.AddAction([this]() {
    // Initialize program population.
    InitProgPop_Random();
    std::cout << ">> Program population size=" << prog_world->GetSize() << std::endl;
  });

  // Configure On Update signal.
  do_update_sig.AddAction([this]() {
    std::cout << "Update: " << update << "; ";
    std::cout << "best program score: " << prog_world->CalcFitnessID(dominant_prog_id) << "; ";
    std::cout << "solution found? " << solution_found << "; ";
    std::cout << "smallest solution? " << smallest_prog_sol_size << std::endl;

    if (update % SNAPSHOT_INTERVAL == 0) do_pop_snapshot_sig.Trigger(); 

    prog_world->Update();
    prog_world->ClearCache();

    UpdateTestCaseWorld();
  });

  // Setup the virtual hardware used to evaluate programs.
  std::cout << "==== EXPERIMENT SETUP => evaluation hardware ====" << std::endl;
  SetupHardware(); 
  
  // Setup problem that we're evolving programs to solve. The particular problem
  // we setup depends on experiment configuration.
  std::cout << "==== EXPERIMENT SETUP => problem ====" << std::endl;
  SetupProblem();  //...many many todos embedded in this one...

  // Setup program (& test) evaluation.
  std::cout << "==== EXPERIMENT SETUP => evaluation ====" << std::endl;
  SetupEvaluation(); 
  
  // Setup program (& test) selection.
  std::cout << "==== EXPERIMENT SETUP => selection ====" << std::endl;
  SetupSelection();

  // Setup program (& test) mutation.
  std::cout << "==== EXPERIMENT SETUP => mutation ====" << std::endl;
  SetupMutation();

  // Setup program (& test) fitness calculations.
  std::cout << "==== EXPERIMENT SETUP => fitness functions ====" << std::endl;
  SetupFitFuns();

  #ifndef EMSCRIPTEN
  // If we're not compiling to javascript, setup data collection for both programs
  // and tests.
  std::cout << "==== EXPERIMENT SETUP => data collection ====" << std::endl;
  SetupDataCollection();
  #endif

  // Signal that setup is done. This will trigger any post-setup things that need
  // to run before doing evolution (e.g., population initialization).
  end_setup_sig.Trigger();

  // Flag setup as done.
  setup = true;

  // TODO - assert that only one test world ptr is not nullptr

  std::cout << "==== EXPERIMENT SETUP => DONE! ====" << std::endl;
}

/// Run the experiment start->finish [update=0 : update=config.GENERATIONS].
void ProgramSynthesisExperiment::Run() {
  // For each generation, advance 'time' by one step.
  for (update = 0; update <= GENERATIONS; ++update) {
    RunStep();
  }
}

/// Run a single step of the experiment
void ProgramSynthesisExperiment::RunStep() {
  do_evaluation_sig.Trigger();  // (1) Evaluate all members of program (& test) population(s).
  do_selection_sig.Trigger();   // (2) Select who gets to reproduce!
  do_update_sig.Trigger();      // (3) Run update on relevant worlds (population turnover, etc).
}

// ================ Internal function implementations ================
/// Localize configs.
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

  // -- Selection settings --
  PROG_SELECTION_MODE = config.PROG_SELECTION_MODE();
  TEST_SELECTION_MODE = config.TEST_SELECTION_MODE();
  PROG_LEXICASE_MAX_FUNS = config.PROG_LEXICASE_MAX_FUNS();
  PROG_COHORTLEXICASE_MAX_FUNS = config.PROG_COHORTLEXICASE_MAX_FUNS();
  TEST_LEXICASE_MAX_FUNS = config.TEST_LEXICASE_MAX_FUNS();
  TEST_COHORTLEXICASE_MAX_FUNS = config.TEST_COHORTLEXICASE_MAX_FUNS();
  PROG_TOURNAMENT_SIZE = config.PROG_TOURNAMENT_SIZE();
  TEST_TOURNAMENT_SIZE = config.TEST_TOURNAMENT_SIZE();

  // -- Hardware settings --
  MIN_TAG_SPECIFICITY = config.MIN_TAG_SPECIFICITY();
  MAX_CALL_DEPTH = config.MAX_CALL_DEPTH();

  // -- Program settings --
  MIN_PROG_SIZE = config.MIN_PROG_SIZE();
  MAX_PROG_SIZE = config.MAX_PROG_SIZE();
  PROG_EVAL_TIME  = config.PROG_EVAL_TIME();
  PROG_MUT__PER_BIT_FLIP = config.PROG_MUT__PER_BIT_FLIP();
  PROG_MUT__PER_INST_SUB = config.PROG_MUT__PER_INST_SUB();
  PROG_MUT__PER_INST_INS = config.PROG_MUT__PER_INST_INS();
  PROG_MUT__PER_INST_DEL = config.PROG_MUT__PER_INST_DEL();
  PROG_MUT__PER_PROG_SLIP = config.PROG_MUT__PER_PROG_SLIP();
  PROG_MUT__PER_MOD_DUP = config.PROG_MUT__PER_MOD_DUP();
  PROG_MUT__PER_MOD_DEL = config.PROG_MUT__PER_MOD_DEL();

  // -- Number IO settings --
  PROB_NUMBER_IO__DOUBLE_MIN = config.PROB_NUMBER_IO__DOUBLE_MIN();
  PROB_NUMBER_IO__DOUBLE_MAX = config.PROB_NUMBER_IO__DOUBLE_MAX();
  PROB_NUMBER_IO__INT_MIN = config.PROB_NUMBER_IO__INT_MIN();
  PROB_NUMBER_IO__INT_MAX = config.PROB_NUMBER_IO__INT_MAX();
  PROB_NUMBER_IO__MUTATION__PER_INT_RATE = config.PROB_NUMBER_IO__MUTATION__PER_INT_RATE();
  PROB_NUMBER_IO__MUTATION__PER_DOUBLE_RATE = config.PROB_NUMBER_IO__MUTATION__PER_DOUBLE_RATE();

  DATA_DIRECTORY = config.DATA_DIRECTORY();
  SUMMARY_STATS_INTERVAL = config.SUMMARY_STATS_INTERVAL();
  SNAPSHOT_INTERVAL = config.SNAPSHOT_INTERVAL();
  SOLUTION_SCREEN_INTERVAL = config.SOLUTION_SCREEN_INTERVAL();

}

/// Setup the appropriate problem by calling its problem-specific setup function.
/// Which problem is setup will depend on configuration.
void ProgramSynthesisExperiment::SetupProblem() {
  emp_assert(emp::Has(problems, PROBLEM), "Unknown problem!", PROBLEM);
  // Big ol' switch statement to select appropriate problem to setup.
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

/// Setup hardware used to evaluate evolving programs.
void ProgramSynthesisExperiment::SetupHardware() {
  // Create new instruction library.
  inst_lib = emp::NewPtr<inst_lib_t>();
  // Create evaluation hardware.
  eval_hardware = emp::NewPtr<hardware_t>(inst_lib, random);
  // Configure the CPU.
  eval_hardware->SetMemSize(MEM_SIZE);                       // Configure size of memory.
  eval_hardware->SetMinTagSpecificity(MIN_TAG_SPECIFICITY);  // Configure minimum tag specificity required for tag-based referencing.
  eval_hardware->SetMaxCallDepth(MAX_CALL_DEPTH);            // Configure maximum depth of call stack (recursion limit).
  eval_hardware->SetMemTags(GenHadamardMatrix<TAG_WIDTH>()); // Configure memory location tags. Use Hadamard matrix for given TAG_WIDTH.

  // Configure call tag (tag used to call initial module during test evaluation).
  call_tag.Clear(); // Set initial call tag to all 0s.

  // What do at beginning of program evaluation (about to be run on potentially many tests)?
  // - Reset virtual hardware (reset hardware, clear module definitions, clear program).
  // - Set program to given organism's 'genome'.
  begin_program_eval.AddAction([this](prog_org_t & prog_org) {
    eval_hardware->Reset();
    eval_hardware->SetProgram(prog_org.GetGenome());
  });

  // What do at end of program evaluation (after being run on some number of tests)?
  // - Currently, nothing.
  
  // What to do before running program on a single test?
  // - Reset virtual hardware (reset global memory, reset call stack).
  begin_program_test.AddAction([this](prog_org_t & prog_org, emp::Ptr<TestOrg_Base> test_org_ptr) {
    eval_hardware->ResetHardware();
    eval_hardware->CallModule(call_tag, MIN_TAG_SPECIFICITY, true, false);
  });

  // Specify how we 'do' a program test.
  // - For specified evaluation time, advance evaluation hardware. If at any point
  //   the program's call stack is empty, automatically finish the evaluation.
  do_program_test.AddAction([this](prog_org_t & prog_org, emp::Ptr<TestOrg_Base> test_org_ptr) {
    for (eval_time = 0; eval_time < PROG_EVAL_TIME; ++eval_time) {
      do_program_advance.Trigger(prog_org);
      if (eval_hardware->GetCallStackSize() == 0) break; // If call stack is ever completely empty, program is done early.
    }
  });
  
  // How do we advance the evaluation hardware?
  do_program_advance.AddAction([this](prog_org_t &) {
    eval_hardware->SingleProcess();
  });
}

// Setup evaluation.
void ProgramSynthesisExperiment::SetupEvaluation() {
  switch (EVALUATION_MODE) {
    // In cohort evaluation, programs and tests are evaluated in 'cohorts'. Populations
    // are divided into cohorts (the number of cohorts for tests and programs must
    // be equal -- size of cohorts may be different).
    case (size_t)EVALUATION_TYPE::COHORT: {
      emp_assert(PROG_POP_SIZE % PROG_COHORT_SIZE == 0, "Program population size must be evenly divisible by program cohort size.");
      emp_assert(TEST_POP_SIZE % TEST_COHORT_SIZE == 0, "Test population size must be evenly divisible by test cohort size.");
      std::cout << ">> Setting up cohorts." << std::endl;
      test_cohorts.Setup(TEST_POP_SIZE, TEST_COHORT_SIZE);
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
        // On placement, reset organism phenotype.
        prog_world->GetOrg(pos).GetPhenotype().Reset(TEST_COHORT_SIZE);
      });
      
      // Setup test case world on placement response.
      OnPlacement_ActiveTestCaseWorld([this](size_t pos) { 
        // On placement, reset organism phenotype.
        GetTestPhenotype(pos).Reset(PROG_COHORT_SIZE); 
      });

      // What should happen on evaluation?
      do_evaluation_sig.AddAction([this]() {
        // Randomize the cohorts.
        prog_cohorts.Randomize(*random);
        test_cohorts.Randomize(*random);
        // For each cohort, evaluate all programs against all tests in corresponding cohort.
        for (size_t cID = 0; cID < prog_cohorts.GetCohortCnt(); ++cID) {
          for (size_t pID = 0; pID < PROG_COHORT_SIZE; ++pID) {
            // Get program organism specified by pID.
            prog_org_t & prog_org = prog_world->GetOrg(prog_cohorts.GetWorldID(cID, pID));
            begin_program_eval.Trigger(prog_org);
            for (size_t tID = 0; tID < TEST_COHORT_SIZE; ++tID) {
              const size_t test_world_id = test_cohorts.GetWorldID(cID, tID);
              
              // Evaluate program on this test.
              TestResult result = EvaluateWorldTest(prog_org, test_world_id);
              
              // Grab references to relevant phenotypes.
              test_org_phen_t & test_phen = GetTestPhenotype(test_world_id);
              prog_org_phen_t & prog_phen = prog_org.GetPhenotype();
              
              // Test result contents:
              // - double score;
              // - bool pass;
              // - bool sub;

              // Update program phenotype.
              prog_phen.RecordScore(tID, result.score);
              prog_phen.RecordPass(tID, result.pass);
              prog_phen.RecordSubmission(result.sub);
              
              // Update test phenotype
              test_phen.RecordScore(pID, result.score);
              test_phen.RecordPass(pID, result.pass);
            }
            end_program_eval.Trigger(prog_org);
          }
        }
      });
      break;
    }
    // In full evaluation mode, evaluate all programs on all tests.
    case (size_t)EVALUATION_TYPE::FULL: {
      std::cout << ">> Setting up full evaluation. No cohorts here." << std::endl;
      
      // Setup program world on placement signal response.
      prog_world->OnPlacement([this](size_t pos) {
        // Reset program phenotype on placement.
        prog_world->GetOrg(pos).GetPhenotype().Reset(TEST_POP_SIZE); 
      });

      // Setup test world on placement signal response.
      OnPlacement_ActiveTestCaseWorld([this](size_t pos) {
        // Reset test phenotype on placement.
        GetTestPhenotype(pos).Reset(PROG_POP_SIZE);   
      });
      
      PROGRAM_MAX_PASSES = TEST_POP_SIZE;
      NUM_COHORTS = 0;

      // What should happen on evaluation?
      do_evaluation_sig.AddAction([this]() {
        for (size_t pID = 0; pID < PROG_POP_SIZE; ++pID) {
          emp_assert(prog_world->IsOccupied(pID));
          prog_org_t & prog_org = prog_world->GetOrg(pID);
          begin_program_eval.Trigger(prog_org);
          for (size_t tID = 0; tID < TEST_POP_SIZE; ++tID) {

            TestResult result = EvaluateWorldTest(prog_org, tID);
            
            // Grab references to test and program phenotypes.
            test_org_phen_t & test_phen = GetTestPhenotype(tID);
            prog_org_phen_t & prog_phen = prog_org.GetPhenotype();

            // Test result contents:
            // - double score;
            // - bool pass;
            // - bool sub;

            // Update program phenotype.
            prog_phen.RecordScore(tID, result.score);
            prog_phen.RecordPass(tID, result.pass);
            prog_phen.RecordSubmission(result.sub);
            
            // Update phenotypes.
            test_phen.RecordScore(pID, result.score);
            test_phen.RecordPass(pID, result.pass);
          }
          end_program_eval.Trigger(prog_org);
        }
      });

      break;
    }
    default: {
      std::cout << "Unknown EVALUATION_MODE (" << EVALUATION_MODE << "). Exiting." << std::endl;
      exit(-1);
    } 
  }

  // Add generic evaluation action - calculate num_passes/fails for tests and programs.
  do_evaluation_sig.AddAction([this]() {
    // Sum pass totals for programs, find 'dominant' program.
    double cur_best_score = 0;
    for (size_t pID = 0; pID < prog_world->GetSize(); ++pID) {
      emp_assert(prog_world->IsOccupied(pID));

      prog_org_t & prog_org = prog_world->GetOrg(pID);
      const size_t pass_total = prog_org.GetPhenotype().num_passes;
      const double total_score = prog_org.GetPhenotype().total_score;

      // Is this the highest pass total program this generation?
      if (total_score > cur_best_score || pID == 0) {
        dominant_prog_id = pID;
        cur_best_score = pass_total;
      }
      // At this point, program has been evaluated against all tests. .
      if (pass_total == PROGRAM_MAX_PASSES && prog_org.GetGenome().GetSize() < smallest_prog_sol_size) {
        stats_util.cur_progID = pID;
        if (ScreenForSolution(prog_org)) {
          solution_found = true;
          smallest_prog_sol_size = prog_org.GetGenome().GetSize();
          // Add to solutions file.
          solution_file->Update();
        }
      }
    }

    // Sum pass/fail totals for tests.
    for (size_t tID = 0; tID < TEST_POP_SIZE; ++tID) {
      test_org_phen_t & test_phen = GetTestPhenotype(tID);
      if (test_phen.num_fails > cur_best_score || tID == 0) { // NOTE - will need to be updated if switch to gradient fitness functions (will be problem specific whether or not use a gradient).
        dominant_test_id = tID;
        cur_best_score = test_phen.num_fails;
      }
    }
  });
}

/// Setup selection for programs and tests.
void ProgramSynthesisExperiment::SetupSelection() {
  // (1) Setup program selection.
  SetupProgramSelection();
  // (2) Setup test selection.
  if (prob_NumberIO_world != nullptr) { SetupTestSelection(prob_NumberIO_world, prob_utils_NumberIO.lexicase_fit_set); }
  else if (prob_SmallOrLarge_world != nullptr) { SetupTestSelection(prob_SmallOrLarge_world, prob_utils_SmallOrLarge.lexicase_fit_set); }
  else if (prob_ForLoopIndex_world != nullptr) { SetupTestSelection(prob_ForLoopIndex_world, prob_utils_ForLoopIndex.lexicase_fit_set); }
  else if (prob_CompareStringLengths_world != nullptr) { SetupTestSelection(prob_CompareStringLengths_world, prob_utils_CompareStringLengths.lexicase_fit_set); }
  else if (prob_DoubleLetters_world != nullptr) { SetupTestSelection(prob_DoubleLetters_world, prob_utils_DoubleLetters.lexicase_fit_set); }
  else if (prob_CollatzNumbers_world != nullptr) { SetupTestSelection(prob_CollatzNumbers_world, prob_utils_CollatzNumbers.lexicase_fit_set); }
  else if (prob_ReplaceSpaceWithNewline_world != nullptr) { SetupTestSelection(prob_ReplaceSpaceWithNewline_world, prob_utils_ReplaceSpaceWithNewline.lexicase_fit_set); }
  else if (prob_StringDifferences_world != nullptr) { SetupTestSelection(prob_StringDifferences_world, prob_utils_StringDifferences.lexicase_fit_set); }
  else if (prob_EvenSquares_world != nullptr) { SetupTestSelection(prob_EvenSquares_world, prob_utils_EvenSquares.lexicase_fit_set); }
  else if (prob_WallisPi_world != nullptr) { SetupTestSelection(prob_WallisPi_world, prob_utils_WallisPi.lexicase_fit_set); }
  else if (prob_StringLengthsBackwards_world != nullptr) { SetupTestSelection(prob_StringLengthsBackwards_world, prob_utils_StringLengthsBackwards.lexicase_fit_set); }
  else if (prob_LastIndexOfZero_world != nullptr) { SetupTestSelection(prob_LastIndexOfZero_world, prob_utils_LastIndexOfZero.lexicase_fit_set); }
  else if (prob_VectorAverage_world != nullptr) { SetupTestSelection(prob_VectorAverage_world, prob_utils_VectorAverage.lexicase_fit_set); }
  else if (prob_CountOdds_world != nullptr) { SetupTestSelection(prob_CountOdds_world, prob_utils_CountOdds.lexicase_fit_set); }
  else if (prob_MirrorImage_world != nullptr) { SetupTestSelection(prob_MirrorImage_world, prob_utils_MirrorImage.lexicase_fit_set); }
  else if (prob_SuperAnagrams_world != nullptr) { SetupTestSelection(prob_SuperAnagrams_world, prob_utils_SuperAnagrams.lexicase_fit_set); }
  else if (prob_SumOfSquares_world != nullptr) { SetupTestSelection(prob_SumOfSquares_world, prob_utils_SumOfSquares.lexicase_fit_set); }
  else if (prob_VectorsSummed_world != nullptr) { SetupTestSelection(prob_VectorsSummed_world, prob_utils_VectorsSummed.lexicase_fit_set); }
  else if (prob_XWordLines_world != nullptr) { SetupTestSelection(prob_XWordLines_world, prob_utils_XWordLines.lexicase_fit_set); }
  else if (prob_PigLatin_world != nullptr) { SetupTestSelection(prob_PigLatin_world, prob_utils_PigLatin.lexicase_fit_set); }
  else if (prob_NegativeToZero_world != nullptr) { SetupTestSelection(prob_NegativeToZero_world, prob_utils_NegativeToZero.lexicase_fit_set); }
  else if (prob_ScrabbleScore_world != nullptr) { SetupTestSelection(prob_ScrabbleScore_world, prob_utils_ScrabbleScore.lexicase_fit_set); }
  else if (prob_Checksum_world != nullptr) { SetupTestSelection(prob_Checksum_world, prob_utils_Checksum.lexicase_fit_set); }
  else if (prob_Digits_world != nullptr) { SetupTestSelection(prob_Digits_world, prob_utils_Digits.lexicase_fit_set); }
  else if (prob_Grade_world != nullptr) { SetupTestSelection(prob_Grade_world, prob_utils_Grade.lexicase_fit_set); }
  else if (prob_Median_world != nullptr) { SetupTestSelection(prob_Median_world, prob_utils_Median.lexicase_fit_set); }
  else if (prob_Smallest_world != nullptr) { SetupTestSelection(prob_Smallest_world, prob_utils_Smallest.lexicase_fit_set); }
  else if (prob_Syllables_world != nullptr) { SetupTestSelection(prob_Syllables_world, prob_utils_Syllables.lexicase_fit_set); }
  else { std::cout << "AHH! More than one test case world has been created. Exiting." << std::endl; exit(-1); }
}

/// Setup program selection.
void ProgramSynthesisExperiment::SetupProgramSelection() {
  switch (PROG_SELECTION_MODE) {
    case (size_t)SELECTION_TYPE::LEXICASE: {
      std::cout << "  >> Setting up program LEXICASE selection." << std::endl;
      // Lexicase selection requires full evaluation mode?
      emp_assert(EVALUATION_MODE == (size_t)EVALUATION_TYPE::FULL, "Lexicase selection requires FULL evaluation mode.");
      // Setup program fitness functions.
      // - 1 function for every test score.
      for (size_t i = 0; i < TEST_POP_SIZE; ++i) {
        lexicase_prog_fit_set.push_back([i](prog_org_t & prog_org) {
          emp_assert(i < prog_org.GetPhenotype().test_scores.size(), i, prog_org.GetPhenotype().test_scores.size());
          double score = prog_org.GetPhenotype().test_scores[i];
          return score;
        });
      }
      // Add pressure for small size
      lexicase_prog_fit_set.push_back([this](prog_org_t & prog_org) {
        if (prog_org.GetPhenotype().num_passes == PROGRAM_MAX_PASSES) {
          return (double)(MAX_PROG_SIZE - prog_org.GetGenome().GetSize());
        } 
        return 0.0;
      });
      // Add selection action
      do_selection_sig.AddAction([this]() {
        emp::LexicaseSelect_NAIVE(*prog_world,
                                  lexicase_prog_fit_set,
                                  PROG_POP_SIZE,
                                  PROG_LEXICASE_MAX_FUNS); // TODO - track lexicase fit fun stats
      });
      break;
    }
    case (size_t)SELECTION_TYPE::COHORT_LEXICASE: {
      std::cout << "  >> Setting up program COHORT LEXICASE selection." << std::endl;
      emp_assert(EVALUATION_MODE == (size_t)EVALUATION_TYPE::COHORT, "Cohort lexicase selection requires COHORT evaluation mode.");
      emp_assert(PROG_COHORT_SIZE * prog_cohorts.GetCohortCnt() == PROG_POP_SIZE);

      // Setup program fitness functions.
      // - 1 function for every test cohort member.
      for (size_t i = 0; i < TEST_COHORT_SIZE; ++i) {
        lexicase_prog_fit_set.push_back([i](prog_org_t & prog_org) {
          emp_assert(i < prog_org.GetPhenotype().test_scores.size(), i, prog_org.GetPhenotype().test_scores.size());
          double score = prog_org.GetPhenotype().test_scores[i];
          return score;
        });
      }
      // Selection pressure for small program size.
      lexicase_prog_fit_set.push_back([this](prog_org_t & prog_org) {
        if (prog_org.GetPhenotype().num_passes == PROGRAM_MAX_PASSES) {
          return (double)(MAX_PROG_SIZE - prog_org.GetGenome().GetSize());
        } 
        return 0.0;
      });
      // Add selection action
      do_selection_sig.AddAction([this]() {
        for (size_t cID = 0; cID < prog_cohorts.GetCohortCnt(); ++cID) {
          emp::CohortLexicaseSelect_NAIVE(*prog_world,
                                          lexicase_prog_fit_set,
                                          prog_cohorts.GetCohort(cID),
                                          PROG_COHORT_SIZE,
                                          PROG_COHORTLEXICASE_MAX_FUNS);
        }
      });
      break;
    }
    case (size_t)SELECTION_TYPE::TOURNAMENT: {
      std::cout << "  >> Setting up program TOURNAMENT selection." << std::endl;
      do_selection_sig.AddAction([this]() {
        emp::TournamentSelect(*prog_world, PROG_TOURNAMENT_SIZE, PROG_POP_SIZE);
      });
      break;
    }
    case (size_t)SELECTION_TYPE::DRIFT: {
      std::cout << "  >> Setting up program DRIFT selection." << std::endl;
      do_selection_sig.AddAction([this]() {
        emp::RandomSelect(*prog_world, PROG_POP_SIZE);
      });
      break;
    }
    default: {
      std::cout << "Unknown PROG_SELECTION_MODE (" << PROG_SELECTION_MODE << "). Exiting." << std::endl;
      exit(-1);
    }
  }
}

/// Setup program/test mutation.
void ProgramSynthesisExperiment::SetupMutation() {
  // (1) Setup program mutations
  SetupProgramMutation();
  // (2) Setup test mutations
  SetupTestMutation();
  // (3) Setup world(s) to auto mutate.
  end_setup_sig.AddAction([this]() {
    prog_world->SetAutoMutate();      // After we've initialized populations, turn auto mutate on.
  });
  if (prob_NumberIO_world != nullptr) { end_setup_sig.AddAction([this]() { prob_NumberIO_world->SetAutoMutate(); }); }
  else if (prob_SmallOrLarge_world != nullptr) { end_setup_sig.AddAction([this]() { prob_SmallOrLarge_world->SetAutoMutate(); }); }
  else if (prob_ForLoopIndex_world != nullptr) { end_setup_sig.AddAction([this]() { prob_ForLoopIndex_world->SetAutoMutate(); }); }
  else if (prob_CompareStringLengths_world != nullptr) { end_setup_sig.AddAction([this]() { prob_CompareStringLengths_world->SetAutoMutate(); }); }
  else if (prob_DoubleLetters_world != nullptr) { end_setup_sig.AddAction([this]() { prob_DoubleLetters_world->SetAutoMutate(); }); }
  else if (prob_CollatzNumbers_world != nullptr) { end_setup_sig.AddAction([this]() { prob_CollatzNumbers_world->SetAutoMutate(); }); }
  else if (prob_ReplaceSpaceWithNewline_world != nullptr) { end_setup_sig.AddAction([this]() { prob_ReplaceSpaceWithNewline_world->SetAutoMutate(); }); }
  else if (prob_StringDifferences_world != nullptr) { end_setup_sig.AddAction([this]() { prob_StringDifferences_world->SetAutoMutate(); }); }
  else if (prob_EvenSquares_world != nullptr) { end_setup_sig.AddAction([this]() { prob_EvenSquares_world->SetAutoMutate(); }); }
  else if (prob_WallisPi_world != nullptr) { end_setup_sig.AddAction([this]() { prob_WallisPi_world->SetAutoMutate(); }); }
  else if (prob_StringLengthsBackwards_world != nullptr) { end_setup_sig.AddAction([this]() { prob_StringLengthsBackwards_world->SetAutoMutate(); }); }
  else if (prob_LastIndexOfZero_world != nullptr) { end_setup_sig.AddAction([this]() { prob_LastIndexOfZero_world->SetAutoMutate(); }); }
  else if (prob_VectorAverage_world != nullptr) { end_setup_sig.AddAction([this]() { prob_VectorAverage_world->SetAutoMutate(); }); }
  else if (prob_CountOdds_world != nullptr) { end_setup_sig.AddAction([this]() { prob_CountOdds_world->SetAutoMutate(); }); }
  else if (prob_MirrorImage_world != nullptr) { end_setup_sig.AddAction([this]() { prob_MirrorImage_world->SetAutoMutate(); }); }
  else if (prob_SuperAnagrams_world != nullptr) { end_setup_sig.AddAction([this]() { prob_SuperAnagrams_world->SetAutoMutate(); }); }
  else if (prob_SumOfSquares_world != nullptr) { end_setup_sig.AddAction([this]() { prob_SumOfSquares_world->SetAutoMutate(); }); }
  else if (prob_VectorsSummed_world != nullptr) { end_setup_sig.AddAction([this]() { prob_VectorsSummed_world->SetAutoMutate(); }); }
  else if (prob_XWordLines_world != nullptr) { end_setup_sig.AddAction([this]() { prob_XWordLines_world->SetAutoMutate(); }); }
  else if (prob_PigLatin_world != nullptr) { end_setup_sig.AddAction([this]() { prob_PigLatin_world->SetAutoMutate(); }); }
  else if (prob_NegativeToZero_world != nullptr) { end_setup_sig.AddAction([this]() { prob_NegativeToZero_world->SetAutoMutate(); }); }
  else if (prob_ScrabbleScore_world != nullptr) { end_setup_sig.AddAction([this]() { prob_ScrabbleScore_world->SetAutoMutate(); }); }
  else if (prob_Checksum_world != nullptr) { end_setup_sig.AddAction([this]() { prob_Checksum_world->SetAutoMutate(); }); }
  else if (prob_Digits_world != nullptr) { end_setup_sig.AddAction([this]() { prob_Digits_world->SetAutoMutate(); }); }
  else if (prob_Grade_world != nullptr) { end_setup_sig.AddAction([this]() { prob_Grade_world->SetAutoMutate(); }); }
  else if (prob_Median_world != nullptr) { end_setup_sig.AddAction([this]() { prob_Median_world->SetAutoMutate(); }); }
  else if (prob_Smallest_world != nullptr) { end_setup_sig.AddAction([this]() { prob_Smallest_world->SetAutoMutate(); }); }
  else if (prob_Syllables_world != nullptr) { end_setup_sig.AddAction([this]() { prob_Syllables_world->SetAutoMutate(); }); }
  else { std::cout << "AHH! None of the worlds have been initialized. Exiting." << std::endl; exit(-1); }
}

/// Setup program mutation
void ProgramSynthesisExperiment::SetupProgramMutation() {
  // Configure program mutator.
  prog_mutator.MAX_PROGRAM_LEN = MAX_PROG_SIZE;
  prog_mutator.MIN_PROGRAM_LEN = MIN_PROG_SIZE;
  prog_mutator.PER_BIT_FLIP = PROG_MUT__PER_BIT_FLIP;
  prog_mutator.PER_INST_SUB = PROG_MUT__PER_INST_SUB;
  prog_mutator.PER_INST_INS = PROG_MUT__PER_INST_INS;
  prog_mutator.PER_INST_DEL = PROG_MUT__PER_INST_DEL;
  prog_mutator.PER_PROG_SLIP = PROG_MUT__PER_PROG_SLIP;
  prog_mutator.PER_MOD_DUP = PROG_MUT__PER_MOD_DUP;
  prog_mutator.PER_MOD_DEL = PROG_MUT__PER_MOD_DEL;

  // Configure world mutation function.
  prog_world->SetMutFun([this](prog_org_t & prog_org, emp::Random & rnd) {
    return prog_mutator.Mutate(rnd, prog_org.GetGenome());
  });
}

/// Setup fitness functions for tests/programs.
void ProgramSynthesisExperiment::SetupFitFuns() {
  // (1) Setup program mutations
  SetupProgramFitFun();
  // (2) Setup test mutations
  SetupTestFitFun();
}

/// Setup program fitness function.
void ProgramSynthesisExperiment::SetupProgramFitFun() {
  prog_world->SetFitFun([this](prog_org_t & prog_org) {
    double fitness = prog_org.GetPhenotype().total_score;
    if (prog_org.GetPhenotype().num_passes == PROGRAM_MAX_PASSES) { // Add 'smallness' bonus.
      fitness += ((double)(MAX_PROG_SIZE - prog_org.GetGenome().GetSize()))/(double)MAX_PROG_SIZE;
    }
    return fitness;
  });
}

/// Setup data collection.
void ProgramSynthesisExperiment::SetupDataCollection() {
  std::cout << "Setting up data collection." << std::endl;
  // Make a data directory
  mkdir(DATA_DIRECTORY.c_str(), ACCESSPERMS);
  if (DATA_DIRECTORY.back() != '/') DATA_DIRECTORY += '/';

  // Setup snapshot program stats.
  SetupProgramStats();

  // Setup solution file.
  solution_file = emp::NewPtr<emp::DataFile>(DATA_DIRECTORY + "/solutions.csv");
  std::function<size_t(void)> get_update = [this]() { return prog_world->GetUpdate(); };
  solution_file->AddFun(get_update, "update");
  solution_file->AddFun(program_stats.get_id, "program_id");
  solution_file->AddFun(program_stats.get_program_len, "program_len");
  solution_file->AddFun(program_stats.get_program, "program");
  solution_file->PrintHeaderKeys();

  // Setup program/problem[X] fitness file.
  prog_world->SetupFitnessFile(DATA_DIRECTORY + "program_fitness.csv").SetTimingRepeat(SUMMARY_STATS_INTERVAL);
  // Setup test world fitness file (just don't look...)
  if (prob_NumberIO_world != nullptr) { prob_NumberIO_world->SetupFitnessFile(DATA_DIRECTORY + "test_fitness.csv").SetTimingRepeat(SUMMARY_STATS_INTERVAL); }
  else if (prob_SmallOrLarge_world != nullptr) { prob_SmallOrLarge_world->SetupFitnessFile(DATA_DIRECTORY + "test_fitness.csv").SetTimingRepeat(SUMMARY_STATS_INTERVAL); }
  else if (prob_ForLoopIndex_world != nullptr) { prob_ForLoopIndex_world->SetupFitnessFile(DATA_DIRECTORY + "test_fitness.csv").SetTimingRepeat(SUMMARY_STATS_INTERVAL); }
  else if (prob_CompareStringLengths_world != nullptr) { prob_CompareStringLengths_world->SetupFitnessFile(DATA_DIRECTORY + "test_fitness.csv").SetTimingRepeat(SUMMARY_STATS_INTERVAL); }
  else if (prob_DoubleLetters_world != nullptr) { prob_DoubleLetters_world->SetupFitnessFile(DATA_DIRECTORY + "test_fitness.csv").SetTimingRepeat(SUMMARY_STATS_INTERVAL); }
  else if (prob_CollatzNumbers_world != nullptr) { prob_CollatzNumbers_world->SetupFitnessFile(DATA_DIRECTORY + "test_fitness.csv").SetTimingRepeat(SUMMARY_STATS_INTERVAL); }
  else if (prob_ReplaceSpaceWithNewline_world != nullptr) { prob_ReplaceSpaceWithNewline_world->SetupFitnessFile(DATA_DIRECTORY + "test_fitness.csv").SetTimingRepeat(SUMMARY_STATS_INTERVAL); }
  else if (prob_StringDifferences_world != nullptr) { prob_StringDifferences_world->SetupFitnessFile(DATA_DIRECTORY + "test_fitness.csv").SetTimingRepeat(SUMMARY_STATS_INTERVAL); }
  else if (prob_EvenSquares_world != nullptr) { prob_EvenSquares_world->SetupFitnessFile(DATA_DIRECTORY + "test_fitness.csv").SetTimingRepeat(SUMMARY_STATS_INTERVAL); }
  else if (prob_WallisPi_world != nullptr) { prob_WallisPi_world->SetupFitnessFile(DATA_DIRECTORY + "test_fitness.csv").SetTimingRepeat(SUMMARY_STATS_INTERVAL); }
  else if (prob_StringLengthsBackwards_world != nullptr) { prob_StringLengthsBackwards_world->SetupFitnessFile(DATA_DIRECTORY + "test_fitness.csv").SetTimingRepeat(SUMMARY_STATS_INTERVAL); }
  else if (prob_LastIndexOfZero_world != nullptr) { prob_LastIndexOfZero_world->SetupFitnessFile(DATA_DIRECTORY + "test_fitness.csv").SetTimingRepeat(SUMMARY_STATS_INTERVAL); }
  else if (prob_VectorAverage_world != nullptr) { prob_VectorAverage_world->SetupFitnessFile(DATA_DIRECTORY + "test_fitness.csv").SetTimingRepeat(SUMMARY_STATS_INTERVAL); }
  else if (prob_CountOdds_world != nullptr) { prob_CountOdds_world->SetupFitnessFile(DATA_DIRECTORY + "test_fitness.csv").SetTimingRepeat(SUMMARY_STATS_INTERVAL); }
  else if (prob_MirrorImage_world != nullptr) { prob_MirrorImage_world->SetupFitnessFile(DATA_DIRECTORY + "test_fitness.csv").SetTimingRepeat(SUMMARY_STATS_INTERVAL); }
  else if (prob_SuperAnagrams_world != nullptr) { prob_SuperAnagrams_world->SetupFitnessFile(DATA_DIRECTORY + "test_fitness.csv").SetTimingRepeat(SUMMARY_STATS_INTERVAL); }
  else if (prob_SumOfSquares_world != nullptr) { prob_SumOfSquares_world->SetupFitnessFile(DATA_DIRECTORY + "test_fitness.csv").SetTimingRepeat(SUMMARY_STATS_INTERVAL); }
  else if (prob_VectorsSummed_world != nullptr) { prob_VectorsSummed_world->SetupFitnessFile(DATA_DIRECTORY + "test_fitness.csv").SetTimingRepeat(SUMMARY_STATS_INTERVAL); }
  else if (prob_XWordLines_world != nullptr) { prob_XWordLines_world->SetupFitnessFile(DATA_DIRECTORY + "test_fitness.csv").SetTimingRepeat(SUMMARY_STATS_INTERVAL); }
  else if (prob_PigLatin_world != nullptr) { prob_PigLatin_world->SetupFitnessFile(DATA_DIRECTORY + "test_fitness.csv").SetTimingRepeat(SUMMARY_STATS_INTERVAL); }
  else if (prob_NegativeToZero_world != nullptr) { prob_NegativeToZero_world->SetupFitnessFile(DATA_DIRECTORY + "test_fitness.csv").SetTimingRepeat(SUMMARY_STATS_INTERVAL); }
  else if (prob_ScrabbleScore_world != nullptr) { prob_ScrabbleScore_world->SetupFitnessFile(DATA_DIRECTORY + "test_fitness.csv").SetTimingRepeat(SUMMARY_STATS_INTERVAL); }
  else if (prob_Checksum_world != nullptr) { prob_Checksum_world->SetupFitnessFile(DATA_DIRECTORY + "test_fitness.csv").SetTimingRepeat(SUMMARY_STATS_INTERVAL); }
  else if (prob_Digits_world != nullptr) { prob_Digits_world->SetupFitnessFile(DATA_DIRECTORY + "test_fitness.csv").SetTimingRepeat(SUMMARY_STATS_INTERVAL); }
  else if (prob_Grade_world != nullptr) { prob_Grade_world->SetupFitnessFile(DATA_DIRECTORY + "test_fitness.csv").SetTimingRepeat(SUMMARY_STATS_INTERVAL); }
  else if (prob_Median_world != nullptr) { prob_Median_world->SetupFitnessFile(DATA_DIRECTORY + "test_fitness.csv").SetTimingRepeat(SUMMARY_STATS_INTERVAL); }
  else if (prob_Smallest_world != nullptr) { prob_Smallest_world->SetupFitnessFile(DATA_DIRECTORY + "test_fitness.csv").SetTimingRepeat(SUMMARY_STATS_INTERVAL); }
  else if (prob_Syllables_world != nullptr) { prob_Syllables_world->SetupFitnessFile(DATA_DIRECTORY + "test_fitness.csv").SetTimingRepeat(SUMMARY_STATS_INTERVAL); }
  else { std::cout << "AHH! None of the worlds have been initialized. Exiting." << std::endl; exit(-1); }

  // todo - how are we snapshotting tests?
  do_pop_snapshot_sig.AddAction([this]() {
    SnapshotPrograms();
    SnapshotTests();
  });

}

/// Setup program stats functions (used by data collection stuff).
void ProgramSynthesisExperiment::SetupProgramStats() {
  // Setup program stats functions.

  // program_stats.get_id
  program_stats.get_id = [this]() { return stats_util.cur_progID; };

  // program_stats.get_fitness
  program_stats.get_fitness = [this]() { return prog_world->CalcFitnessID(stats_util.cur_progID); };

  // program_stats.get_fitness_eval__total_score
  program_stats.get_fitness_eval__total_score = [this]() { return prog_world->GetOrg(stats_util.cur_progID).GetPhenotype().total_score; };
  
  // program_stats.get_fitness_eval__num_passes
  program_stats.get_fitness_eval__num_passes = [this]() { return prog_world->GetOrg(stats_util.cur_progID).GetPhenotype().num_passes; };

  // program_stats.get_fitness_eval__num_fails
  program_stats.get_fitness_eval__num_fails = [this]() { return prog_world->GetOrg(stats_util.cur_progID).GetPhenotype().num_fails; };

  // program_stats.get_fitness_eval__num_tests
  program_stats.get_fitness_eval__num_tests = [this]() { return prog_world->GetOrg(stats_util.cur_progID).GetPhenotype().test_passes.size(); };

  // program_stats.get_fitness_eval__passes_by_test
  program_stats.get_fitness_eval__passes_by_test = [this]() { 
    prog_org_t & prog = prog_world->GetOrg(stats_util.cur_progID);
    std::string scores = "\"[";
    for (size_t i = 0; i < prog.GetPhenotype().test_passes.size(); ++i) {
      if (i) scores += ",";
      scores += emp::to_string((size_t)prog.GetPhenotype().test_passes[i]);
    }
    scores += "]\"";
    return scores;
  };

  // program_stats.get_validation_eval__num_passes
  program_stats.get_validation_eval__num_passes = [this]() {
    return stats_util.current_program__validation__total_passes;
  };

  // program_stats.get_validation_eval__num_tests
  program_stats.get_validation_eval__num_tests = [this]() {
    return stats_util.current_program__validation__test_results.size();
  };

  // program_stats.get_validation_eval__passes_by_test
  program_stats.get_validation_eval__passes_by_test = [this]() {
    std::string scores = "\"[";
    for (size_t i = 0; i < stats_util.current_program__validation__test_results.size(); ++i) {
      if (i) scores += ",";
      scores += emp::to_string((size_t)stats_util.current_program__validation__test_results[i].pass);
    }
    scores += "]\"";
    return scores; 
  };

  // program_stats.get_program_len
  program_stats.get_program_len = [this]() {
    return prog_world->GetOrg(stats_util.cur_progID).GetGenome().GetSize();
  };

  // program_stats.get_program
  program_stats.get_program = [this]() {
    std::ostringstream stream;
    prog_world->GetOrg(stats_util.cur_progID).GetGenome().PrintCSVEntry(stream);
    return stream.str();
  };
}

/// Take a snapshot of the program population.
void ProgramSynthesisExperiment::SnapshotPrograms() {
  std::string snapshot_dir = DATA_DIRECTORY + "pop_" + emp::to_string(prog_world->GetUpdate());
  mkdir(snapshot_dir.c_str(), ACCESSPERMS);

  emp::DataFile file(snapshot_dir + "/program_pop_" + emp::to_string((int)prog_world->GetUpdate()) + ".csv");

  // Add functions to data file.
  file.AddFun(program_stats.get_id, "program_id", "Program ID");
  
  file.AddFun(program_stats.get_fitness, "fitness");
  file.AddFun(program_stats.get_fitness_eval__total_score, "total_score__fitness_eval");
  file.AddFun(program_stats.get_fitness_eval__num_passes, "num_passes__fitness_eval");
  file.AddFun(program_stats.get_fitness_eval__num_fails, "num_fails__fitness_eval");
  file.AddFun(program_stats.get_fitness_eval__num_tests, "num_tests__fitness_eval");
  file.AddFun(program_stats.get_fitness_eval__passes_by_test, "passes_by_test__fitness_eval");

  file.AddFun(program_stats.get_validation_eval__num_passes, "num_passes__validation_eval");
  file.AddFun(program_stats.get_validation_eval__num_tests, "num_tests__validation_eval");
  file.AddFun(program_stats.get_validation_eval__passes_by_test, "passes_by_test__validation_eval");

  file.AddFun(program_stats.get_program_len, "program_len");
  file.AddFun(program_stats.get_program, "program");

  file.PrintHeaderKeys();

  // For each program in the population, dump the program and anything we want to know about it.
  for (stats_util.cur_progID = 0; stats_util.cur_progID < prog_world->GetSize(); ++stats_util.cur_progID) {
    if (!prog_world->IsOccupied(stats_util.cur_progID)) continue;
    DoTestingSetValidation(prog_world->GetOrg(stats_util.cur_progID)); // Do validation for program.
    file.Update();
  }
}

// ================= PROGRAM-RELATED FUNCTIONS ===========
void ProgramSynthesisExperiment::InitProgPop_Random() {
  std::cout << "Randomly initializing program population." << std::endl;
  for (size_t i = 0; i < PROG_POP_SIZE; ++i) {
    prog_world->Inject(TagLGP::GenRandTagGPProgram(*random, inst_lib, MIN_PROG_SIZE, MAX_PROG_SIZE), 1);
  }
  // New:
  // emp::vector<emp::BitSet<TAG_WIDTH>> matrix = GenHadamardMatrix<TAG_WIDTH>();
  // hardware_t::Program sol(inst_lib);
  // sol.PushInst("LoadInt", {matrix[0], matrix[0], matrix[0]});
  // sol.PushInst("LoadDouble", {matrix[1], matrix[1], matrix[1]});
  // sol.PushInst("Add", {matrix[0], matrix[1], matrix[2]});
  // sol.PushInst("SubmitNum", {matrix[2], matrix[2], matrix[2]});
  // sol.PushInst("Return", {matrix[0], matrix[0], matrix[0]});
  // prog_world->Inject(sol, PROG_POP_SIZE);
}

void ProgramSynthesisExperiment::AddDefaultInstructions(const std::unordered_set<std::string> & includes={"Add","Sub","Mult","Div","Mod","TestNumEqu","TestNumNEqu","TestNumLess","Floor","Not","Inc","Dec",
                                                                                                  "CopyMem","SwapMem","Input","Output","CommitGlobal","PullGlobal","TestMemEqu","TestMemNEqu",
                                                                                                  "MakeVector","VecGet","VecSet","VecLen","VecAppend","VecPop","VecRemove","VecReplaceAll","VecIndexOf","VecOccurrencesOf","VecReverse","VecSwapIfLess","VecGetFront","VecGetBack",
                                                                                                  "IsNum","IsStr","IsVec",
                                                                                                  "If","IfNot","While","Countdown","Foreach","Close","Break","Call","Routine","Return",
                                                                                                  "ModuleDef"}) 
{
  // Configure instructions
  // Math
  if (emp::Has(includes, "Add")) inst_lib->AddInst("Add", hardware_t::Inst_Add, 3, "wmemANY[C] = wmemNUM[A] + wmemNUM[B]");
  if (emp::Has(includes, "Sub")) inst_lib->AddInst("Sub", hardware_t::Inst_Sub, 3, "wmemANY[C] = wmemNUM[A] - wmemNUM[B]");
  if (emp::Has(includes, "Mult")) inst_lib->AddInst("Mult", hardware_t::Inst_Mult, 3, "wmemANY[C] = wmemNUM[A] * wmemNUM[B]");
  if (emp::Has(includes, "Div")) inst_lib->AddInst("Div", hardware_t::Inst_Div, 3, "if (wmemNUM[B] != 0) wmemANY[C] = wmemNUM[A] / wmemNUM[B]; else NOP");
  if (emp::Has(includes, "Mod")) inst_lib->AddInst("Mod", hardware_t::Inst_Mod, 3, "if (wmemNUM[B] != 0) wmemANY[C] = int(wmemNUM[A]) % int(wmemNUM[B]); else NOP");
  if (emp::Has(includes, "TestNumEqu")) inst_lib->AddInst("TestNumEqu", hardware_t::Inst_TestNumEqu, 3, "wmemANY[C] = wmemNUM[A] == wmemNUM[B]");
  if (emp::Has(includes, "TestNumNEqu")) inst_lib->AddInst("TestNumNEqu", hardware_t::Inst_TestNumNEqu, 3, "wmemANY[C] = wmemNUM[A] != wmemNUM[B]");
  if (emp::Has(includes, "TestNumLess")) inst_lib->AddInst("TestNumLess", hardware_t::Inst_TestNumLess, 3, "wmemANY[C] = wmemNUM[A] < wmemNUM[B]");
  if (emp::Has(includes, "Floor")) inst_lib->AddInst("Floor", hardware_t::Inst_Floor, 1, "wmemNUM[A] = floor(wmemNUM[A])");
  if (emp::Has(includes, "Not")) inst_lib->AddInst("Not", hardware_t::Inst_Not, 1, "wmemNUM[A] = !wmemNUM[A]"); 
  if (emp::Has(includes, "Inc")) inst_lib->AddInst("Inc", hardware_t::Inst_Inc, 1, "wmemNUM[A] = wmemNUM[A] + 1");
  if (emp::Has(includes, "Dec")) inst_lib->AddInst("Dec", hardware_t::Inst_Dec, 1, "wmemNUM[A] = wmemNUM[A] - 1");
  
  // Memory manipulation
  if (emp::Has(includes, "CopyMem")) inst_lib->AddInst("CopyMem", hardware_t::Inst_CopyMem, 2, "wmemANY[B] = wmemANY[A] // Copy mem[A] to mem[B]");
  if (emp::Has(includes, "SwapMem")) inst_lib->AddInst("SwapMem", hardware_t::Inst_SwapMem, 2, "swap(wmemANY[A], wmemANY[B])");
  if (emp::Has(includes, "Input")) inst_lib->AddInst("Input", hardware_t::Inst_Input, 2, "wmemANY[B] = imemANY[A]");
  if (emp::Has(includes, "Output")) inst_lib->AddInst("Output", hardware_t::Inst_Output, 2, "omemANY[B] = wmemANY[A]");
  if (emp::Has(includes, "CommitGlobal")) inst_lib->AddInst("CommitGlobal", hardware_t::Inst_CommitGlobal, 2, "gmemANY[B] = wmemANY[A]");
  if (emp::Has(includes, "PullGlobal")) inst_lib->AddInst("PullGlobal", hardware_t::Inst_PullGlobal, 2, "wmemANY[B] = gmemANY[A]");
  if (emp::Has(includes, "TestMemEqu")) inst_lib->AddInst("TestMemEqu", hardware_t::Inst_TestMemEqu, 3, "wmemANY[C] = wmemANY[A] == wmemANY[B]");
  if (emp::Has(includes, "TestMemNEqu")) inst_lib->AddInst("TestMemNEqu", hardware_t::Inst_TestMemNEqu, 3, "wmemANY[C] = wmemANY[A] != wmemANY[B]");

  // Vector-related instructions
  if (emp::Has(includes, "MakeVector")) inst_lib->AddInst("MakeVector", hardware_t::Inst_MakeVector, 3, "wmemANY[C]=Vector([wmemANY[min(A,B),max(A,B)])");  // TODO - more descriptions
  if (emp::Has(includes, "VecGet")) inst_lib->AddInst("VecGet", hardware_t::Inst_VecGet, 3, "wmemANY[C]=wmemVEC[A][wmemNUM[B]]");
  if (emp::Has(includes, "VecSet")) inst_lib->AddInst("VecSet", hardware_t::Inst_VecSet, 3, "wmemVEC[A][wmemNUM[B]]=wmemNUM/STR[C]");
  if (emp::Has(includes, "VecLen")) inst_lib->AddInst("VecLen", hardware_t::Inst_VecLen, 2, "wmemANY[B]=wmemVEC[A]");
  if (emp::Has(includes, "VecAppend")) inst_lib->AddInst("VecAppend", hardware_t::Inst_VecAppend, 2, "wmemVEC[A].Append(wmemNUM/STR[B])");
  if (emp::Has(includes, "VecPop")) inst_lib->AddInst("VecPop", hardware_t::Inst_VecPop, 2, "wmemANY[B]=wmemVEC[A].pop()");
  if (emp::Has(includes, "VecRemove")) inst_lib->AddInst("VecRemove", hardware_t::Inst_VecRemove, 2, "wmemVEC[A].Remove(wmemNUM[B])");
  if (emp::Has(includes, "VecReplaceAll")) inst_lib->AddInst("VecReplaceAll", hardware_t::Inst_VecReplaceAll, 3, "Replace all values (wmemNUM/STR[B]) in wmemVEC[A] with wmemNUM/STR[C]");
  if (emp::Has(includes, "VecIndexOf")) inst_lib->AddInst("VecIndexOf", hardware_t::Inst_VecIndexOf, 3, "wmemANY[C] = index of wmemNUM/STR[B] in wmemVEC[A]");
  if (emp::Has(includes, "VecOccurrencesOf")) inst_lib->AddInst("VecOccurrencesOf", hardware_t::Inst_VecOccurrencesOf, 3, "wmemANY[C]= occurrances of wmemNUM/STR[B] in wmemVEC[A]");
  if (emp::Has(includes, "VecReverse")) inst_lib->AddInst("VecReverse", hardware_t::Inst_VecReverse, 1, "wmemVEC[A] = Reverse(wmemVEC[A])");
  if (emp::Has(includes, "VecSwapIfLess")) inst_lib->AddInst("VecSwapIfLess", hardware_t::Inst_VecSwapIfLess, 3, "Swap two indices in wmemVEC[A] if vec[wmemNUM[A]] < vec[wmemNUM[B]].");
  if (emp::Has(includes, "VecGetFront")) inst_lib->AddInst("VecGetFront", hardware_t::Inst_VecGetFront, 2, "wmemANY[B] = front of wmemVEC[A]");
  if (emp::Has(includes, "VecGetBack")) inst_lib->AddInst("VecGetBack", hardware_t::Inst_VecGetBack, 2, "wmemANY[B] = back of wmemVEC[A]");
  
  // Memory-type
  if (emp::Has(includes, "IsNum")) inst_lib->AddInst("IsNum", hardware_t::Inst_IsNum, 2, "wmemANY[B] = IsNum(wmemANY[A])");
  if (emp::Has(includes, "IsStr")) inst_lib->AddInst("IsStr", hardware_t::Inst_IsStr, 2, "wmemANY[B] = IsStr(wmemANY[A])");
  if (emp::Has(includes, "IsVec")) inst_lib->AddInst("IsVec", hardware_t::Inst_IsVec, 2, "wmemANY[B] = IsVec(wmemANY[A])");

  // Flow control
  if (emp::Has(includes, "If")) inst_lib->AddInst("If", hardware_t::Inst_If, 1, "Execute next flow if(wmemANY[A]) // To be true, mem loc must be non-zero number", {inst_lib_t::InstProperty::BEGIN_FLOW});
  if (emp::Has(includes, "IfNot")) inst_lib->AddInst("IfNot", hardware_t::Inst_IfNot, 1, "Execute next flow if(!wmemANY[A])", {inst_lib_t::InstProperty::BEGIN_FLOW});
  if (emp::Has(includes, "While")) inst_lib->AddInst("While", hardware_t::Inst_While, 1, "While loop over wmemANY[A]", {inst_lib_t::InstProperty::BEGIN_FLOW});
  if (emp::Has(includes, "Countdown")) inst_lib->AddInst("Countdown", hardware_t::Inst_Countdown, 1, "Countdown loop with wmemANY as index.", {inst_lib_t::InstProperty::BEGIN_FLOW});
  if (emp::Has(includes, "Foreach")) inst_lib->AddInst("Foreach", hardware_t::Inst_Foreach, 2, "For each thing in wmemVEC[B]", {inst_lib_t::InstProperty::BEGIN_FLOW});
  if (emp::Has(includes, "Close")) inst_lib->AddInst("Close", hardware_t::Inst_Close, 0, "Close flow", {inst_lib_t::InstProperty::END_FLOW});
  if (emp::Has(includes, "Break")) inst_lib->AddInst("Break", hardware_t::Inst_Break, 0, "Break current flow");
  if (emp::Has(includes, "Call")) inst_lib->AddInst("Call", hardware_t::Inst_Call, 1, "Call module using A for tag-based reference");
  if (emp::Has(includes, "Routine")) inst_lib->AddInst("Routine", hardware_t::Inst_Routine, 1, "Call module as a routine (don't use call stack)");
  if (emp::Has(includes, "Return")) inst_lib->AddInst("Return", hardware_t::Inst_Return, 0, "Return from current routine/call");

  // Module
  if (emp::Has(includes, "ModuleDef")) inst_lib->AddInst("ModuleDef", hardware_t::Inst_Nop, 1, "Define module with tag A", {inst_lib_t::InstProperty::MODULE});

  // Misc
  // inst_lib->AddInst("Nop", hardware_t::Inst_Nop, 3, "Do nothing");
}

// ================= PROBLEM SETUPS ======================

void ProgramSynthesisExperiment::SetupProblem_NumberIO() { 
  std::cout << "NumberIO problem setup" << std::endl; 

  using test_org_t = TestOrg_NumberIO;
  
  // Load testing examples from file (used to evaluate 'true' performance of programs).
  if (BENCHMARK_DATA_DIR.back() != '/') BENCHMARK_DATA_DIR += '/';
  std::string training_examples_fpath = BENCHMARK_DATA_DIR + problems.at(PROBLEM).GetTrainingSetFilename();  
  std::string testing_examples_fpath = BENCHMARK_DATA_DIR + problems.at(PROBLEM).GetTestingSetFilename();  
  prob_utils_NumberIO.GetTrainingSet().LoadTestCases(training_examples_fpath);
  prob_utils_NumberIO.GetTestingSet().LoadTestCases(testing_examples_fpath);
  prob_utils_NumberIO.GenerateTestingSetPop();
  std::cout << ">> Loaded training example set size = " << prob_utils_NumberIO.GetTrainingSet().GetSize() << std::endl;
  std::cout << ">> Loaded testing example set size = " << prob_utils_NumberIO.GetTestingSet().GetSize() << std::endl;
  std::cout << ">> Testing set (non-training examples used to evaluate program accuracy) size = " << prob_utils_NumberIO.testingset_pop.size() << std::endl;

  // Setup world.
  NewTestCaseWorld(prob_NumberIO_world, *random, "NumberIO Test Case World");
  
  // Configure how population should be initialized
  SetupTestCasePop_Init(prob_NumberIO_world, 
                        prob_utils_NumberIO.training_set,
                        [this]() { return GenRandomTestInput_NumberIO(*random, {PROB_NUMBER_IO__INT_MIN, PROB_NUMBER_IO__INT_MAX}, {PROB_NUMBER_IO__DOUBLE_MIN, PROB_NUMBER_IO__DOUBLE_MAX}); } );

  end_setup_sig.AddAction([this]() { std::cout << "TestCase world size = " << prob_NumberIO_world->GetSize() << std::endl; });
  
  // Tell world to calculate correct test output (given input) on placement.
  prob_NumberIO_world->OnPlacement([this](size_t pos) { prob_NumberIO_world->GetOrg(pos).CalcOut(); } );

  EvaluateWorldTest = [this](prog_org_t & prog_org, size_t testID) {
    emp::Ptr<test_org_t> test_org_ptr = prob_NumberIO_world->GetOrgPtr(testID);
    begin_program_test.Trigger(prog_org, test_org_ptr);
    do_program_test.Trigger(prog_org, test_org_ptr);
    end_program_test.Trigger(prog_org, test_org_ptr);
    return CalcProgramResultOnTest(prog_org, *test_org_ptr);
  };

  DoTestingSetValidation = [this](prog_org_t & prog_org) {
    // evaluate program on full testing set; update stats utils with results
    begin_program_eval.Trigger(prog_org);
    stats_util.current_program__validation__test_results.resize(prob_utils_NumberIO.testingset_pop.size());
    stats_util.current_program__validation__total_score = 0;
    stats_util.current_program__validation__total_passes = 0;
    stats_util.current_program__validation__is_solution = false;
    // For each test in validation set, evaluate program.
    for (size_t testID = 0; testID < prob_utils_NumberIO.testingset_pop.size(); ++testID) {
      stats_util.cur_testID = testID;
      emp::Ptr<test_org_t> test_org_ptr = prob_utils_NumberIO.testingset_pop[testID];
      begin_program_test.Trigger(prog_org, test_org_ptr);
      do_program_test.Trigger(prog_org, test_org_ptr);
      end_program_test.Trigger(prog_org, test_org_ptr);
      stats_util.current_program__validation__test_results[testID] = CalcProgramResultOnTest(prog_org, *test_org_ptr);
      stats_util.current_program__validation__total_score += stats_util.current_program__validation__test_results[testID].score;
      stats_util.current_program__validation__total_passes += (size_t)stats_util.current_program__validation__test_results[testID].pass;
    }
    stats_util.current_program__validation__is_solution = stats_util.current_program__validation__total_passes == prob_utils_NumberIO.testingset_pop.size();
    end_program_eval.Trigger(prog_org);
  }; // todo - test this out

  ScreenForSolution = [this](prog_org_t & prog_org) {
    begin_program_eval.Trigger(prog_org);
    for (size_t testID = 0; testID < prob_utils_NumberIO.testingset_pop.size(); ++testID) {
      stats_util.cur_testID = testID;
      emp::Ptr<test_org_t> test_org_ptr = prob_utils_NumberIO.testingset_pop[testID];

      begin_program_test.Trigger(prog_org, test_org_ptr);
      do_program_test.Trigger(prog_org, test_org_ptr);
      end_program_test.Trigger(prog_org, test_org_ptr);
      
      TestResult result = CalcProgramResultOnTest(prog_org, *test_org_ptr);
      if (!result.pass) {
        end_program_eval.Trigger(prog_org);
        return false;
      }
    }
    end_program_eval.Trigger(prog_org);
    return true;
  };

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

  SetupTestMutation = [this]() {
    // (1) Configure mutator.
    prob_utils_NumberIO.mutator.MIN_INT = PROB_NUMBER_IO__INT_MIN;
    prob_utils_NumberIO.mutator.MAX_INT = PROB_NUMBER_IO__INT_MAX;
    prob_utils_NumberIO.mutator.MIN_DOUBLE = PROB_NUMBER_IO__DOUBLE_MIN;
    prob_utils_NumberIO.mutator.MAX_DOUBLE = PROB_NUMBER_IO__DOUBLE_MAX;
    prob_utils_NumberIO.mutator.PER_INT_RATE = PROB_NUMBER_IO__MUTATION__PER_INT_RATE;
    prob_utils_NumberIO.mutator.PER_DOUBLE_RATE = PROB_NUMBER_IO__MUTATION__PER_DOUBLE_RATE;
    // (2) Hook mutator up to world.
    prob_NumberIO_world->SetMutFun([this](test_org_t & test_org, emp::Random & rnd) {
      return prob_utils_NumberIO.mutator.Mutate(rnd, test_org.GetGenome());
    });
  };

  SetupTestFitFun = [this]() {
    prob_NumberIO_world->SetFitFun([](test_org_t & test_org) {
      return (double)test_org.GetPhenotype().num_fails;
    });
  };
  
  // Tell experiment how to configure hardware inputs when running program against a test.
  begin_program_test.AddAction([this](prog_org_t & prog_org, emp::Ptr<TestOrg_Base> test_org_base_ptr) {
    // Reset eval stuff
    // Set current test org.
    // prob_utils_NumberIO.cur_eval_test_org = prob_NumberIO_world->GetOrgPtr(testID); // currently only place need testID for this?
    prob_utils_NumberIO.cur_eval_test_org = test_org_base_ptr.Cast<test_org_t>(); // currently only place need testID for this?
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
  CalcProgramResultOnTest = [this](prog_org_t & prog_org, TestOrg_Base & test_org_base) {
    // std::cout << "Calc score on test!" << std::endl;
    test_org_t & test_org = static_cast<test_org_t&>(test_org_base);
    TestResult result;
    if (!prob_utils_NumberIO.submitted) {
      result.score = 0;
      result.pass = false;
      result.sub = false;
    } else {
      std::pair<double, bool> r(CalcScorePassFail_NumberIO(test_org.GetCorrectOut(), prob_utils_NumberIO.submitted_val));
      result.score = r.first;
      result.pass = r.second;
      result.sub = true;
    }
    return result;
  };

  SnapshotTests = [this]() {
    std::string snapshot_dir = DATA_DIRECTORY + "pop_" + emp::to_string(prog_world->GetUpdate());
    mkdir(snapshot_dir.c_str(), ACCESSPERMS);
    
    emp::DataFile file(snapshot_dir + "/test_pop_" + emp::to_string((int)prog_world->GetUpdate()) + ".csv");
    // Test file contents:
    // - test id
    std::function<size_t(void)> get_test_id = [this]() { return stats_util.cur_testID; };
    file.AddFun(get_test_id, "test_id");

    // - test fitness
    std::function<double(void)> get_test_fitness = [this]() { return prob_NumberIO_world->CalcFitnessID(stats_util.cur_testID); };
    file.AddFun(get_test_fitness, "fitness");

    // - num passes
    std::function<size_t(void)> get_test_num_passes = [this]() { return GetTestPhenotype(stats_util.cur_testID).num_passes; };
    file.AddFun(get_test_num_passes, "num_passes");

    // - num fails
    std::function<size_t(void)> get_test_num_fails = [this]() { return GetTestPhenotype(stats_util.cur_testID).num_fails; };
    file.AddFun(get_test_num_fails, "num_fails");

    std::function<size_t(void)> get_num_tested = [this]() { return GetTestPhenotype(stats_util.cur_testID).test_passes.size(); };
    file.AddFun(get_num_tested, "num_programs_tested_against");

    // - test scores by program
    std::function<std::string(void)> get_passes_by_program = [this]() {
      std::string scores = "\"[";
      test_org_phen_t & phen = GetTestPhenotype(stats_util.cur_testID);
      for (size_t i = 0; i < phen.test_passes.size(); ++i) {
        if (i) scores += ",";
        scores += emp::to_string(phen.test_passes[i]);
      }
      scores += "]\"";
      return scores;
    };
    file.AddFun(get_passes_by_program, "passes_by_program");

    // - test
    std::function<std::string(void)> get_test = [this]() {
      std::ostringstream stream;
      stream << "\"";
      prob_NumberIO_world->GetOrg(stats_util.cur_testID).Print(stream);
      stream << "\"";
      return stream.str();
    };
    file.AddFun(get_test, "test", "");

    file.PrintHeaderKeys();

    // Loop over tests, snapshotting each.
    for (stats_util.cur_testID = 0; stats_util.cur_testID < prob_NumberIO_world->GetSize(); ++stats_util.cur_testID) {
      if (!prob_NumberIO_world->IsOccupied(stats_util.cur_testID)) continue;
      file.Update();
    }

  };

  AddDefaultInstructions({"Add",
                          "Sub",
                          "Mult",
                          "Div",
                          "Mod",
                          "TestNumEqu",
                          "TestNumNEqu",
                          "TestNumLess",
                          "Floor",
                          "Not",
                          "Inc",
                          "Dec",
                          "CopyMem",
                          "SwapMem",
                          "Input",
                          "Output",
                          "CommitGlobal",
                          "PullGlobal",
                          "TestMemEqu",
                          "TestMemNEqu",
                          "If",
                          "IfNot",
                          "While",
                          "Countdown",
                          "Foreach",
                          "Close",
                          "Break",
                          "Call",
                          "Routine",
                          "Return",
                          "ModuleDef"
  });

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
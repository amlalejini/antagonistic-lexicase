#ifndef PROG_SYNTH_BENCHMARK_INPUT_REPRESENTATIONS_H
#define PROG_SYNTH_BENCHMARK_INPUT_REPRESENTATIONS_H

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <utility>
#include <unordered_set>

#include "base/Ptr.h"
#include "base/vector.h"
#include "tools/Random.h"
#include "tools/random_utils.h"
#include "tools/math.h"
#include "tools/sequence_utils.h"
#include "tools/string_utils.h"
#include "tools/stats.h"

#include "TestCaseSet.h"

/*

Genetic representations for different input types used across the programming
synthesis benchmarks.

Ideas:
- Validate take restriction lambdas(?) 

Input/Output types:
- NumberIO: Pair<integer, float>
- SmallOrLarge: Integer
- ForLoopIndex: Array<Integer, 3>
- Compare String Lengths: Array<String, 3>
- Double Letters: String
- Collatz Numbers: Integer
- Replace Space with Newline: String
- String Differences: Array<String, 2>
- Even Squares: Integer
- Wallis Pi: Integer
- String Lengths Backwards: Vector<String>
- Last Index of Zero: Vector<Integer>
- Vector Average: Vector<Float>
- Count Odds: Vector<Integer>
- Mirror Image: Array<Vector<Integer>, 2>
- Super Anagrams: Array<String, 2>
- Sum of Squares: Integer
- Vectors Summed: Array<Vector<Integer>, 2>
- X-Word Lines: Pair<Integer, String>
- Pig Latin: String
- Negative To Zero: Vector<Integer>
- Scrabble Score: String
- Word Stats: File
- Checksum: String
- Digits: Integer
- Grade: Array<Integer, 5>
- Median: Array<Integer, 3>
- Smallest: Array<Integer, 4>
- Syllables: String

Aggregated types:
- Pair<TYPE1, TYPE2>
- Integer
- String
- Array<TYPE, SIZE>
- Vector<TYPE>
*/

// Problem-specific, static variables

// -- PROBLEM: Small or Large --
constexpr int SmallOrLarge__SMALL_THRESH = 1000;
constexpr int SmallOrLarge__LARGE_THRESH = 2000;
const std::string SmallOrLarge__SMALL_STR = "small";
const std::string SmallOrLarge__LARGE_STR = "large";
const std::string SmallOrLarge__NONE_STR = "";

// ================== Problem I/O Type Aliases ==================
using Problem_NumberIO_input_t = std::pair<int, double>;
using Problem_NumberIO_output_t = double;

using Problem_SmallOrLarge_input_t = int;
using Problem_SmallOrLarge_output_t = std::string;

using Problem_ForLoopIndex_input_t = std::array<int, 3>;
using Problem_ForLoopIndex_output_t = emp::vector<int>;

using Problem_CompareStringLengths_input_t = std::array<std::string, 3>;
using Problem_CompareStringLengths_output_t = bool;

using Problem_DoubleLetters_input_t = std::string;
using Problem_DoubleLetters_output_t = std::string;

using Problem_CollatzNumbers_input_t = int;
using Problem_CollatzNumbers_output_t = int;

using Problem_ReplaceSpaceWithNewline_input_t = std::string;
using Problem_ReplaceSpaceWithNewline_output_t = std::pair<std::string, int>;

using Problem_StringDifferences_input_t = std::array<std::string, 2>;
using Problem_StringDifferences_output_t = std::string;

using Problem_EvenSquares_input_t = int;
using Problem_EvenSquares_output_t = int;

using Problem_WallisPi_input_t = int;
using Problem_WallisPi_output_t = double;

using Problem_StringLengthsBackwards_input_t = emp::vector<std::string>;
using Problem_StringLengthsBackwards_output_t = std::string;

using Problem_LastIndexOfZero_input_t = emp::vector<int>;
using Problem_LastIndexOfZero_output_t = int;

using Problem_VectorAverage_input_t = emp::vector<double>;
using Problem_VectorAverage_output_t = double;

using Problem_CountOdds_input_t = emp::vector<int>;
using Problem_CountOdds_output_t = int;

using Problem_MirrorImage_input_t = std::array<emp::vector<int>, 2>;
using Problem_MirrorImage_output_t = bool;

using Problem_SuperAnagrams_input_t = std::array<std::string, 2>;
using Problem_SuperAnagrams_output_t = bool;

using Problem_SumOfSquares_input_t = int;
using Problem_SumOfSquares_output_t = int;

using Problem_VectorsSummed_input_t = std::array<emp::vector<int>, 2>;
using Problem_VectorsSummed_output_t = emp::vector<int>;

using Problem_XWordLines_input_t = std::pair<int, std::string>;
using Problem_XWordLines_output_t = std::string;

using Problem_PigLatin_input_t = std::string;
using Problem_PigLatin_output_t = std::string;

using Problem_NegativeToZero_input_t = emp::vector<int>;
using Problem_NegativeToZero_output_t = emp::vector<int>;

using Problem_ScrabbleScore_input_t = std::string;
using Problem_ScrabbleScore_output_t = int;

using Problem_Checksum_input_t = std::string;
using Problem_Checksum_output_t = std::string;

using Problem_Digits_input_t = int;
using Problem_Digits_output_t = emp::vector<int>;

using Problem_Grade_input_t = std::array<int, 5>;
using Problem_Grade_output_t = std::string;

using Problem_Median_input_t = std::array<int, 3>;
using Problem_Median_output_t = int;

using Problem_Smallest_input_t = std::array<int, 4>;
using Problem_Smallest_output_t = int;

using Problem_Syllables_input_t = std::string;
using Problem_Syllables_output_t = std::string;


// ================ Problem mutators ================

// Problem_NumberIO_input_t = std::pair<int, double>;
struct Pair_IntDouble_Mutator {
  int MIN_INT;
  int MAX_INT;

  double MIN_DOUBLE;
  double MAX_DOUBLE;

  double PER_INT_RATE;
  double PER_DOUBLE_RATE;

  size_t Mutate(emp::Random & rnd, std::pair<int, double> & mut_pair) {
    size_t muts = 0;
    if (rnd.P(PER_INT_RATE)) {
      mut_pair.first = rnd.GetInt(MIN_INT, MAX_INT+1);
      ++muts;
    }
    if (rnd.P(PER_DOUBLE_RATE)) {
      mut_pair.second = rnd.GetDouble(MIN_DOUBLE, MAX_DOUBLE);
      ++muts;
    }
    return muts;
  }
};

struct Int_Mutator {
  int MIN_INT;
  int MAX_INT;

  double PER_INT_RATE;

  size_t Mutate(emp::Random & rnd, int & mut_int) {
    if (rnd.P(PER_INT_RATE)) {
      mut_int = rnd.GetInt(MIN_INT, MAX_INT+1);
      return 1;
    }
    return 0;
  }
};

// ================ Problem Organism Classes ================

// Test org base class
class TestOrg_Base {
  public:

    struct Phenotype {
      emp::vector<double> test_scores;
      double total_score;

      emp::vector<bool> test_passes;
      size_t num_passes;
      size_t num_fails;

      void Reset(size_t s=0) {
        test_scores.clear();
        test_scores.resize(s, 0);
        test_passes.clear();
        test_passes.resize(s, false);
        num_passes = 0;
        num_fails = 0;
      }

      void RecordScore(size_t id, double val) {
        emp_assert(id < test_scores.size());
        total_score += val;
        test_scores[id] = val;
      }

      void RecordPass(size_t id, bool pass) {
        emp_assert(id < test_passes.size());
        if (pass) ++num_passes;
        else ++num_fails;
        test_passes[id] = pass;
      }
    };

    using phenotype_t = Phenotype;

  protected:
    phenotype_t phenotype;

  public:

    phenotype_t & GetPhenotype() { return phenotype; }
    const phenotype_t & GetPhenotype() const { return phenotype; }

    virtual ~TestOrg_Base() = default;

    virtual void CalcOut() = 0;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Problem: NumberIO
// - Input type: [double, integer]
// - Output type: double 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Problem_NumberIO_input_t GenRandomTestInput_NumberIO(emp::Random & rand, const std::pair<int, int> & int_range, const std::pair<double, double> & double_range) {
  emp_assert(double_range.first < double_range.second);
  emp_assert(int_range.first < int_range.second);
  return Problem_NumberIO_input_t{rand.GetInt(int_range.first, int_range.second), rand.GetDouble(double_range.first, double_range.second)};
}
  
Problem_NumberIO_output_t GenCorrectOut_NumberIO(const Problem_NumberIO_input_t & input) {
  return input.first + input.second;
}

void SetCorrectOut_NumberIO(const Problem_NumberIO_input_t & input, Problem_NumberIO_output_t & output) {
  output = input.first + input.second;
}

/// Calculate pass/fail score on NumberIO problem.
std::pair<double, bool> CalcScorePassFail_NumberIO(const Problem_NumberIO_output_t & correct_test_output, double sub) {
  const bool pass = (double)sub == correct_test_output;
  return {(double)pass, pass};
}

/// ProblemOrg: NumberIO
/// NumberIO: Pair<integer, float>
class TestOrg_NumberIO : public TestOrg_Base {
  public:
    using parent_t = TestOrg_Base;
    using parent_t::phenotype;

    using genome_t = Problem_NumberIO_input_t;
    using out_t = Problem_NumberIO_output_t;

  protected:
    genome_t genome;
    out_t out;
  
  public:
    TestOrg_NumberIO(const genome_t & _g) : genome(_g), out() { ; }

    genome_t & GetGenome() { return genome; }
    const genome_t & GetGenome() const { return genome; }

    out_t & GetCorrectOut() { return out; }
    const out_t & GetCorrectOut() const { return out; }

    // std::pair<int, double>;
    int GetTestIntegerInput() const { return genome.first; }
    double GetTestDoubleInput() const { return genome.second; }

    void SetOut(const out_t & _out) { out = _out; }

    void CalcOut() { SetCorrectOut_NumberIO(genome, out); }

    void Print(std::ostream & os=std::cout) const {
      os << genome.first << "," << genome.second;
    }
};

struct ProblemUtilities_NumberIO {
  using input_t = Problem_NumberIO_input_t;
  using output_t = Problem_NumberIO_output_t;
  
  using testcase_set_t = TestCaseSet<input_t,output_t>;
  
  testcase_set_t testing_set;
  testcase_set_t training_set;

  emp::vector<emp::Ptr<TestOrg_NumberIO>> testingset_pop;

  // A few useful things for use within a test evaluation
  emp::Ptr<TestOrg_NumberIO> cur_eval_test_org;
  bool submitted;
  double submitted_val; // if going to do string thing, we can have a submission_str.

  Pair_IntDouble_Mutator mutator;

  emp::vector<std::function<double(TestOrg_NumberIO &)>> lexicase_fit_set;

  ProblemUtilities_NumberIO() 
    : testing_set(ProblemUtilities_NumberIO::LoadTestCaseFromLine),
      training_set(ProblemUtilities_NumberIO::LoadTestCaseFromLine),
      submitted(false), submitted_val(0.0)
  { ; }

  ~ProblemUtilities_NumberIO() {
    for (size_t i = 0; i < testingset_pop.size(); ++i) testingset_pop[i].Delete();
  }

  testcase_set_t & GetTestingSet() { return testing_set; }
  testcase_set_t & GetTrainingSet() { return training_set; }

  void ResetTestEval() {
    submitted = false;
    submitted_val = 0.0;
  }

  void Submit(double val) {
    submitted = true;
    submitted_val = val;
  }

  static std::pair<input_t, output_t> LoadTestCaseFromLine(const std::string & line) {
    emp::vector<std::string> split_line = emp::slice(line, ',');
    input_t input;
    output_t output;
    input.second = std::atof(split_line[0].c_str());
    input.first = std::atof(split_line[1].c_str());
    output = std::atof(split_line[2].c_str());
    return {input, output};
  }

  void GenerateTestingSetPop() {
    for (size_t i = 0; i < testing_set.GetSize(); ++i) {
      testingset_pop.emplace_back(emp::NewPtr<TestOrg_NumberIO>(testing_set.GetInput(i)));
      testingset_pop[i]->CalcOut();
    }
  }

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Problem: Small or large
// - Input: integer
// - Output: string {'small', 'large', ''}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Generate random test input
Problem_SmallOrLarge_input_t GenRandomTestInput_SmallOrLarge(emp::Random & rand, const std::pair<int,int> & int_range) {
  emp_assert(int_range.first < int_range.second);
  return rand.GetInt(int_range.first, int_range.second+1);
}

Problem_SmallOrLarge_output_t GenCorrectOut_SmallOrLarge(const Problem_SmallOrLarge_input_t & input) {
  if (input < SmallOrLarge__SMALL_THRESH) return SmallOrLarge__SMALL_STR;
  else if (input >= SmallOrLarge__LARGE_THRESH) return SmallOrLarge__LARGE_STR;
  else return SmallOrLarge__NONE_STR;
}

/// SmallOrLarge: Integer
class TestOrg_SmallOrLarge : public TestOrg_Base {
  public:
    using parent_t = TestOrg_Base;
    using parent_t::phenotype;

    using genome_t = Problem_SmallOrLarge_input_t;
    using out_t = Problem_SmallOrLarge_output_t;

  protected:
    genome_t genome;
    out_t out;

  public:
    TestOrg_SmallOrLarge(const genome_t & _g) : genome(_g), out() { ; }
    
    genome_t & GetGenome() { return genome; }
    const genome_t & GetGenome() const { return genome; }

    out_t & GetCorrectOut() { return out; }
    const out_t & GetCorrectOut() const { return out; }

    void CalcOut() { out = GenCorrectOut_SmallOrLarge(genome); }

    void Print(std::ostream & os=std::cout) const {
      os << genome;
    }
};

struct ProblemUtilities_SmallOrLarge { 
  using this_t = ProblemUtilities_SmallOrLarge;
  using problem_org_t = TestOrg_SmallOrLarge;
  using input_t = Problem_SmallOrLarge_input_t;
  using output_t = Problem_SmallOrLarge_output_t;
  
  using testcase_set_t = TestCaseSet<input_t, output_t>;
  
  testcase_set_t testing_set;
  testcase_set_t training_set;

  emp::vector<emp::Ptr<problem_org_t>> testingset_pop;

  // --- Useful during a test evaluation ---
  emp::Ptr<problem_org_t> cur_eval_test_org;
  bool submitted;
  std::string submitted_str;

  // Mutation
  Int_Mutator mutator;

  // Selection
  emp::vector<std::function<double(problem_org_t &)>> lexicase_fit_set;

  ProblemUtilities_SmallOrLarge()
    : testing_set(this_t::LoadTestCaseFromLine),
      training_set(this_t::LoadTestCaseFromLine),
      submitted(false), submitted_str("")
  { ; }

  ~ProblemUtilities_SmallOrLarge() {
    for (size_t i = 0; i < testingset_pop.size(); ++i) testingset_pop[i].Delete();
  }

  testcase_set_t & GetTestingSet() { return testing_set; }
  testcase_set_t & GetTrainingSet() { return training_set; }

  void ResetTestEval() {
    submitted = false;
    submitted_str = "";
  }

  void Submit(const std::string & val) {
    submitted = true;
    submitted_str = val;
  }

  static std::pair<input_t, output_t> LoadTestCaseFromLine(const std::string & line) {
    emp::vector<std::string> split_line = emp::slice(line + " ", ',');
    input_t input;    // int
    output_t output;  // std::string
    // std::cout << "LINE=" << line << std::endl;
    input = std::atof(split_line[0].c_str());
    output = split_line[1];
    if (!(output == " " || output == "small " || output == "large ")) {
      std::cout << "ERROR! Bad output ("<<output<<") from line: " << line << std::endl;
      exit(-1);
    }
    if (output == " ") output = "";
    else if (output == "small ") output = "small";
    else if (output == "large ") output = "large";
    return {input, output};
  }

  void GenerateTestingSetPop() {
    for (size_t i = 0; i < testing_set.GetSize(); ++i) {
      testingset_pop.emplace_back(emp::NewPtr<problem_org_t>(testing_set.GetInput(i)));
      testingset_pop[i]->CalcOut();
    }
  }

  std::pair<double, bool> CalcScorePassFail(const output_t & correct_test_output, const output_t & sub) {
    const bool pass = (sub == correct_test_output);
    return {(double)pass, pass};
  }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Problem: ForLoopIndex
// - Input: std::array<int, 3>;
// - Output: emp::vector<int>;
// - Description: Given 3 integer inputs (start, end, step), print the integers in the sequence:
//              - n0 = start
//              - ni = ni-1 + step
//              - for each ni < end
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Generate random test input
Problem_ForLoopIndex_input_t GenRandomTestInput_ForLoopIndex(emp::Random & rand,
                                                         const std::pair<int,int> & start_end_range,
                                                         const std::pair<int,int> & step_range) {
  // Guarantee that start comes before end.
  int start, end, step;
  start = rand.GetInt(start_end_range.first, start_end_range.second+1);
  end = rand.GetInt(start_end_range.first, start_end_range.second+1);
  step = rand.GetInt(step_range.first, step_range.second+1);

  // Need to follow the following rules:
  // (1) start < end               // -> Start should come before end
  // (2) start + (20xstep)+1 > end // -> Enumeration should take no more than 20 steps
  while (true) {
    if (end < start) std::swap(end, start);
    if (start + (20*step) + 1 > end) break;
    start = rand.GetInt(start_end_range.first, start_end_range.second+1);
    end = rand.GetInt(start_end_range.first, start_end_range.second+1);
    step = rand.GetInt(step_range.first, step_range.second+1);
  }
  return {start, end, step};
}

/// Generate correct output
Problem_ForLoopIndex_output_t GenCorrectOut_ForLoopIndex(const Problem_ForLoopIndex_input_t & input) {
  emp::vector<int> out;
  for (int i = input[0]; i < input[1]; i+= input[2]) {
    out.emplace_back(i);
  }
  return out;
}

class TestOrg_ForLoopIndex : public TestOrg_Base {
  public:
    using parent_t = TestOrg_Base;
    using parent_t::phenotype;

    using genome_t = std::array<int, 3>;
    using out_t = Problem_ForLoopIndex_output_t;

  protected:
    genome_t genome;
    out_t out;

  public:
    TestOrg_ForLoopIndex(const genome_t & _g) : genome(_g), out() { ; }
    
    genome_t & GetGenome() { return genome; }
    const genome_t & GetGenome() const { return genome; }

    void CalcOut() { out = GenCorrectOut_ForLoopIndex(genome); }

    out_t & GetCorrectOut() { return out; }
    const out_t & GetCorrectOut() const { return out; }   

    void Print(std::ostream & os=std::cout) {
      os << genome[0] << "," << genome[1] << "," << genome[2];
    }
};

struct ProblemUtilities_ForLoopIndex { 
  using this_t = ProblemUtilities_ForLoopIndex;
  using problem_org_t = TestOrg_ForLoopIndex;
  using input_t = Problem_ForLoopIndex_input_t;
  using output_t = Problem_ForLoopIndex_output_t;
  
  using testcase_set_t = TestCaseSet<input_t, output_t>;
  
  testcase_set_t testing_set;
  testcase_set_t training_set;

  emp::vector<emp::Ptr<problem_org_t>> testingset_pop;

  // --- Useful during a test evaluation ---
  emp::Ptr<problem_org_t> cur_eval_test_org;
  bool submitted;
  emp::vector<int> submitted_vec;

  // Mutation - Handle here...
  int MIN_START_END;
  int MAX_START_END;
  int MIN_STEP;
  int MAX_STEP;
  double MUT_RATE;  

  size_t Mutate(emp::Random & rnd, Problem_ForLoopIndex_input_t & mut_input) {
    size_t muts = 0;

    int start = mut_input[0];
    int end = mut_input[1];
    int step = mut_input[2];

    // Mutate start
    if (rnd.P(MUT_RATE)) { // Mutate start?
      start = rnd.GetInt(MIN_START_END, MAX_START_END+1);
      muts++;
    }
    // Mutate end
    if (rnd.P(MUT_RATE)) {
      end = rnd.GetInt(MIN_START_END, MAX_START_END+1);
      muts++;
    }
    // Mutate step
    if (rnd.P(MUT_RATE)) {
      step = rnd.GetInt(MIN_STEP, MAX_STEP+1);
      muts++;
    }
    
    // Ensure that start still comes before end.  
    if (end == start) end++;
    if (end < start) std::swap(start, end);
    if (!(start + (20*step) + 1 > end)) {
      // Move end closer to start
      end = rnd.GetInt(start, start + (20*step)+1);
    }

    mut_input[0] = start;
    mut_input[1] = end;
    mut_input[2] = step;
    
    return muts;
  }

  // Selection
  emp::vector<std::function<double(problem_org_t &)>> lexicase_fit_set;

  ProblemUtilities_ForLoopIndex()
    : testing_set(this_t::LoadTestCaseFromLine),
      training_set(this_t::LoadTestCaseFromLine),
      submitted(false), submitted_vec()
  { ; }

  ~ProblemUtilities_ForLoopIndex() {
    for (size_t i = 0; i < testingset_pop.size(); ++i) testingset_pop[i].Delete();
  }

  testcase_set_t & GetTestingSet() { return testing_set; }
  testcase_set_t & GetTrainingSet() { return training_set; }

  void ResetTestEval() {
    submitted = false;
    submitted_vec.clear();
  }

  void Submit(int val) {
    submitted = true;
    submitted_vec.emplace_back(val);
  }

  static std::pair<input_t, output_t> LoadTestCaseFromLine(const std::string & line) {
    emp::vector<std::string> split_line = emp::slice(line, ',');
    input_t input;    // int
    output_t output;  // std::string
    // Start = line[0]
    input[0] = std::atof(split_line[0].c_str());
    // End = line[1]
    input[1] = std::atof(split_line[1].c_str());
    // Step = line[2]
    input[2] = std::atof(split_line[2].c_str());
    output = GenCorrectOut_ForLoopIndex(input);  
    return {input, output};
  }

  void GenerateTestingSetPop() {
    for (size_t i = 0; i < testing_set.GetSize(); ++i) {
      testingset_pop.emplace_back(emp::NewPtr<problem_org_t>(testing_set.GetInput(i)));
      testingset_pop[i]->CalcOut();
    }
  }

  std::pair<double, bool> CalcScorePassFail(const output_t & correct_test_output, const output_t & sub) {
    const bool pass = (sub == correct_test_output);
    return {(double)pass, pass};
  }

  std::pair<double, bool> CalcScoreGradient(const output_t & correct_test_output, const output_t & sub) {
    const double max_dist = emp::Max(correct_test_output.size(), sub.size());
    double dist = emp::calc_edit_distance(correct_test_output, sub);
    if (dist == 0) {
      return {1.0, true};
    } else {
      return {(max_dist - dist)/max_dist, false};
    }
  } // todo - test this
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Problem: CompareStringLengths
// - Input: std::array<std::string, 3>;
// - Output: bool;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// /// Generate random test input
Problem_CompareStringLengths_input_t GenRandomTestInput_CompareStringLengths(emp::Random & rand,
                                                                             std::pair<size_t, size_t> str_size_range) {
  // Valid characters: \n, \t, [32, 127)
  emp::vector<char> valid_chars = {'\n', '\t'};
  for (size_t i = 32; i < 127; ++i) valid_chars.emplace_back((char)i);

  // String 1
  size_t str_size = rand.GetUInt(str_size_range.first, str_size_range.second+1);
  std::string str1;
  for (size_t i = 0; i < str_size; ++i) str1.push_back(valid_chars[rand.GetUInt(valid_chars.size())]);

  // String 2
  str_size = rand.GetUInt(str_size_range.first, str_size_range.second+1);
  std::string str2;
  for (size_t i = 0; i < str_size; ++i) str2.push_back(valid_chars[rand.GetUInt(valid_chars.size())]);

  // String 3
  str_size = rand.GetUInt(str_size_range.first, str_size_range.second+1);
  std::string str3;
  for (size_t i = 0; i < str_size; ++i) str3.push_back(valid_chars[rand.GetUInt(valid_chars.size())]);
  // std::cout << "---- RANDOM STRING ----" << std::endl;
  // std::cout << "String 1: " << str1 << std::endl;
  // std::cout << "String 2: " << str2 << std::endl;
  // std::cout << "String 3: " << str3 << std::endl;
  return {str1, str2, str3};
}

/// Generate correct output
Problem_CompareStringLengths_output_t GenCorrectOut_CompareStringLengths(const Problem_CompareStringLengths_input_t & input) {
  if (input[0].size() < input[1].size() && input[1].size() < input[2].size()) return true;
  return false;
}

/// Compare String Lengths: Array<String, 3>
class TestOrg_CompareStringLengths: public TestOrg_Base {
  public:
    using parent_t = TestOrg_Base;
    using parent_t::phenotype;

    using genome_t = std::array<std::string, 3>;
    using out_t = Problem_CompareStringLengths_output_t;

  protected:
    genome_t genome;
    out_t out;

  public:
    TestOrg_CompareStringLengths(const genome_t & _g) : genome(_g), out() { ; }
    
    genome_t & GetGenome() { return genome; }
    const genome_t & GetGenome() const { return genome; }

    void CalcOut() { out = GenCorrectOut_CompareStringLengths(genome); }

    out_t & GetCorrectOut() { return out; }
    const out_t & GetCorrectOut() const { return out; }   

    void Print(std::ostream & os=std::cout) {
      os << "\"" << genome[0] << "\"," << "\"" << genome[1] << "\"," << "\"" << genome[2] << "\"";
    }

};

struct ProblemUtilities_CompareStringLengths { 
  using this_t = ProblemUtilities_CompareStringLengths;
  using problem_org_t = TestOrg_CompareStringLengths;
  using input_t = Problem_CompareStringLengths_input_t;
  using output_t = Problem_CompareStringLengths_output_t;
  
  using testcase_set_t = TestCaseSet<input_t, output_t>;
  
  testcase_set_t testing_set;
  testcase_set_t training_set;

  emp::vector<emp::Ptr<problem_org_t>> testingset_pop;

  // --- Useful during a test evaluation ---
  emp::Ptr<problem_org_t> cur_eval_test_org;
  bool submitted;
  bool submitted_val;

  // Mutation - Handle here...
  int MIN_STR_LEN;
  int MAX_STR_LEN;
  double PER_SITE_INS_RATE;
  double PER_SITE_DEL_RATE;
  double PER_SITE_SUB_RATE; 
  double PER_STR_SWAP_RATE; 
  emp::vector<char> valid_chars;
  
  size_t Mutate(emp::Random & rnd, input_t & mut_input) {
    size_t muts = 0;

    for (size_t i = 0; i < mut_input.size(); ++i) {
      std::string & str = mut_input[i];
      int expected_size = (int)str.size();
      std::string new_string = "";

      for (size_t s = 0; s < str.size(); ++s) {
        
        // Per-site insertions
        if (rnd.P(PER_SITE_INS_RATE) && (expected_size+1 <= MAX_STR_LEN)) {
          // Insert a random character.
          new_string.push_back(valid_chars[rnd.GetUInt(valid_chars.size())]);
          ++expected_size;
          ++muts;
        }
        // Per-site deletions
        if (rnd.P(PER_SITE_DEL_RATE) && (expected_size-1 >= MIN_STR_LEN)) {
          --expected_size;
          ++muts;
          continue;
        }
        size_t whead = new_string.size();
        new_string.push_back(str[s]); 
        // Per-site substitutions
        if (rnd.P(PER_SITE_SUB_RATE)) {
          new_string[whead] = valid_chars[rnd.GetUInt(valid_chars.size())];
          ++muts;
        }
      }
      mut_input[i] = new_string;
    }

    for (size_t i = 0; i < mut_input.size(); ++i) {
      if (rnd.P(PER_STR_SWAP_RATE)) {
        ++muts;
        // Swap position i with a random different position
        size_t other = rnd.GetUInt(mut_input.size());
        while (other == i) other = rnd.GetUInt(mut_input.size());
        std::swap(mut_input[i], mut_input[other]);
      }
    }

    return muts;
  } // todo - test

  // Selection
  emp::vector<std::function<double(problem_org_t &)>> lexicase_fit_set;

  ProblemUtilities_CompareStringLengths()
    : testing_set(this_t::LoadTestCaseFromLine),
      training_set(this_t::LoadTestCaseFromLine),
      submitted(false), submitted_val(false)
  { 
    valid_chars = {'\n', '\t'};
    for (size_t i = 32; i < 127; ++i) valid_chars.emplace_back((char)i); 
  }

  ~ProblemUtilities_CompareStringLengths() {
    for (size_t i = 0; i < testingset_pop.size(); ++i) testingset_pop[i].Delete();
  }

  testcase_set_t & GetTestingSet() { return testing_set; }
  testcase_set_t & GetTrainingSet() { return training_set; }

  void ResetTestEval() {
    submitted = false;
    submitted_val = false;
  }

  void Submit(bool val) {
    submitted = true;
    submitted_val = val;
  }

  static std::pair<input_t, output_t> LoadTestCaseFromLine(const emp::vector<std::string> & line) {
    input_t input;   
    output_t output; 
    // Load input.
    input[0] = line[0];
    input[1] = line[1];
    input[2] = line[2];
    // Load output.
    if (line[3] == "false") output = false;
    else if (line[3] == "true") output = true;
    else {
      std::cout << "ERROR when loading test case output (" << line[3] << "). Exiting." << std::endl;
      exit(-1);
    }
    return {input, output};
  }

  void GenerateTestingSetPop() {
    for (size_t i = 0; i < testing_set.GetSize(); ++i) {
      testingset_pop.emplace_back(emp::NewPtr<problem_org_t>(testing_set.GetInput(i)));
      testingset_pop[i]->CalcOut();
    }
  }

  std::pair<double, bool> CalcScorePassFail(const output_t & correct_test_output, const output_t & sub) {
    const bool pass = (sub == correct_test_output);
    return {(double)pass, pass};
  }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Problem:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Double Letters: String
class TestOrg_DoubleLetters : public TestOrg_Base {
  public:
    using parent_t = TestOrg_Base;
    using parent_t::phenotype;

    using genome_t = std::string;
  protected:
    genome_t genome;

  public:
    TestOrg_DoubleLetters(const genome_t & _g) : genome(_g) { ; }
    
    genome_t & GetGenome() { return genome; }
    const genome_t & GetGenome() const { return genome; }

    void CalcOut() { ; }
};

struct ProblemUtilities_DoubleLetters { emp::vector<std::function<double(TestOrg_DoubleLetters &)>> lexicase_fit_set; };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Problem:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Collatz Numbers: Integer
class TestOrg_CollatzNumbers : public TestOrg_Base {
  public:
    using parent_t = TestOrg_Base;
    using parent_t::phenotype;

    using genome_t = int;
  protected:
    genome_t genome;

  public:
    TestOrg_CollatzNumbers(const genome_t & _g) : genome(_g) { ; }
    
    genome_t & GetGenome() { return genome; }
    const genome_t & GetGenome() const { return genome; }

    void CalcOut() { ; }
};

struct ProblemUtilities_CollatzNumbers { emp::vector<std::function<double(TestOrg_CollatzNumbers &)>> lexicase_fit_set; };



/// Replace Space with Newline: String
class TestOrg_ReplaceSpaceWithNewline : public TestOrg_Base {
  public:
    using parent_t = TestOrg_Base;
    using parent_t::phenotype;

    using genome_t = std::string;
  protected:
    genome_t genome;

  public:
    TestOrg_ReplaceSpaceWithNewline(const genome_t & _g) : genome(_g) { ; }
    
    genome_t & GetGenome() { return genome; }
    const genome_t & GetGenome() const { return genome; }

    void CalcOut() { ; }
};

struct ProblemUtilities_ReplaceSpaceWithNewline { emp::vector<std::function<double(TestOrg_ReplaceSpaceWithNewline &)>> lexicase_fit_set; };



/// String Differences: Array<String, 2>
class TestOrg_StringDifferences : public TestOrg_Base {
  public:
    using parent_t = TestOrg_Base;
    using parent_t::phenotype;

    using genome_t = std::array<std::string, 2>;
  protected:
    genome_t genome;

  public:
    TestOrg_StringDifferences(const genome_t & _g) : genome(_g) { ; }
    
    genome_t & GetGenome() { return genome; }
    const genome_t & GetGenome() const { return genome; }

    void CalcOut() { ; }
};

struct ProblemUtilities_StringDifferences { emp::vector<std::function<double(TestOrg_StringDifferences &)>> lexicase_fit_set; };



/// Even Squares: Integer
class TestOrg_EvenSquares : public TestOrg_Base {
  public:
    using parent_t = TestOrg_Base;
    using parent_t::phenotype;

    using genome_t = int;
  protected:
    genome_t genome;

  public:
    TestOrg_EvenSquares(const genome_t & _g) : genome(_g) { ; }
    
    genome_t & GetGenome() { return genome; }
    const genome_t & GetGenome() const { return genome; }

    void CalcOut() { ; }
};

struct ProblemUtilities_EvenSquares { emp::vector<std::function<double(TestOrg_EvenSquares &)>> lexicase_fit_set; };




/// Wallis Pi: Integer
class TestOrg_WallisPi : public TestOrg_Base {
  public:
    using parent_t = TestOrg_Base;
    using parent_t::phenotype;

    using genome_t = int;
  protected:
    genome_t genome;

  public:
    TestOrg_WallisPi(const genome_t & _g) : genome(_g) { ; }
    
    genome_t & GetGenome() { return genome; }
    const genome_t & GetGenome() const { return genome; }

    void CalcOut() { ; }
};

struct ProblemUtilities_WallisPi { emp::vector<std::function<double(TestOrg_WallisPi &)>> lexicase_fit_set; };




/// String Lengths Backwards: Vector<String>
class TestOrg_StringLengthsBackwards : public TestOrg_Base {
  public:
    using parent_t = TestOrg_Base;
    using parent_t::phenotype;

    using genome_t = emp::vector<std::string>;
  protected:
    genome_t genome;

  public:
    TestOrg_StringLengthsBackwards(const genome_t & _g) : genome(_g) { ; }
    
    genome_t & GetGenome() { return genome; }
    const genome_t & GetGenome() const { return genome; }

    void CalcOut() { ; }
};

struct ProblemUtilities_StringLengthsBackwards { emp::vector<std::function<double(TestOrg_StringLengthsBackwards &)>> lexicase_fit_set; };



/// Last Index of Zero: Vector<Integer>
class TestOrg_LastIndexOfZero : public TestOrg_Base {
  public:
    using parent_t = TestOrg_Base;
    using parent_t::phenotype;

    using genome_t = emp::vector<int>;
  protected:
    genome_t genome;

  public:
    TestOrg_LastIndexOfZero(const genome_t & _g) : genome(_g) { ; }
    
    genome_t & GetGenome() { return genome; }
    const genome_t & GetGenome() const { return genome; }

    void CalcOut() { ; }
};

struct ProblemUtilities_LastIndexOfZero { emp::vector<std::function<double(TestOrg_LastIndexOfZero &)>> lexicase_fit_set; };



/// Vector Average: Vector<Float>
class TestOrg_VectorAverage : public TestOrg_Base {
  public:
    using parent_t = TestOrg_Base;
    using parent_t::phenotype;

    using genome_t = emp::vector<double>;
  protected:
    genome_t genome;

  public:
    TestOrg_VectorAverage(const genome_t & _g) : genome(_g) { ; }
    
    genome_t & GetGenome() { return genome; }
    const genome_t & GetGenome() const { return genome; }

    void CalcOut() { ; }
};

struct ProblemUtilities_VectorAverage { emp::vector<std::function<double(TestOrg_VectorAverage &)>> lexicase_fit_set; };




/// Count Odds: Vector<Integer>
class TestOrg_CountOdds : public TestOrg_Base {
  public:
    using parent_t = TestOrg_Base;
    using parent_t::phenotype;

    using genome_t = emp::vector<int>;
  protected:
    genome_t genome;

  public:
    TestOrg_CountOdds(const genome_t & _g) : genome(_g) { ; }
    
    genome_t & GetGenome() { return genome; }
    const genome_t & GetGenome() const { return genome; }

    void CalcOut() { ; }
};

struct ProblemUtilities_CountOdds { emp::vector<std::function<double(TestOrg_CountOdds &)>> lexicase_fit_set; };




/// Mirror Image: Array<Vector<Integer>, 2>
class TestOrg_MirrorImage : public TestOrg_Base {
  public:
    using parent_t = TestOrg_Base;
    using parent_t::phenotype;

    using genome_t = std::array<emp::vector<int>, 2>;
  protected:
    genome_t genome;

  public:
    TestOrg_MirrorImage(const genome_t & _g) : genome(_g) { ; }
    
    genome_t & GetGenome() { return genome; }
    const genome_t & GetGenome() const { return genome; }

    void CalcOut() { ; }
};

struct ProblemUtilities_MirrorImage { emp::vector<std::function<double(TestOrg_MirrorImage &)>> lexicase_fit_set; };



/// Super Anagrams: Array<String, 2>
class TestOrg_SuperAnagrams : public TestOrg_Base {
  public:
    using parent_t = TestOrg_Base;
    using parent_t::phenotype;

    using genome_t = std::array<std::string, 2>;
  protected:
    genome_t genome;

  public:
    TestOrg_SuperAnagrams(const genome_t & _g) : genome(_g) { ; }
    
    genome_t & GetGenome() { return genome; }
    const genome_t & GetGenome() const { return genome; }

    void CalcOut() { ; }
};

struct ProblemUtilities_SuperAnagrams { emp::vector<std::function<double(TestOrg_SuperAnagrams &)>> lexicase_fit_set; };



/// Sum of Squares: Integer
class TestOrg_SumOfSquares : public TestOrg_Base {
  public:
    using parent_t = TestOrg_Base;
    using parent_t::phenotype;

    using genome_t = int;
  protected:
    genome_t genome;

  public:
    TestOrg_SumOfSquares(const genome_t & _g) : genome(_g) { ; }
    
    genome_t & GetGenome() { return genome; }
    const genome_t & GetGenome() const { return genome; }

    void CalcOut() { ; }
};

struct ProblemUtilities_SumOfSquares { emp::vector<std::function<double(TestOrg_SumOfSquares &)>> lexicase_fit_set; };




/// Vectors Summed: Array<Vector<Integer>, 2>
class TestOrg_VectorsSummed: public TestOrg_Base {
  public:
    using parent_t = TestOrg_Base;
    using parent_t::phenotype;

    using genome_t = std::array<emp::vector<int>, 2>;
  protected:
    genome_t genome;

  public:
    TestOrg_VectorsSummed(const genome_t & _g) : genome(_g) { ; }
    
    genome_t & GetGenome() { return genome; }
    const genome_t & GetGenome() const { return genome; }

    void CalcOut() { ; }
};

struct ProblemUtilities_VectorsSummed { emp::vector<std::function<double(TestOrg_VectorsSummed &)>> lexicase_fit_set; };




/// X-Word Lines: Pair<Integer, String>
class TestOrg_XWordLines : public TestOrg_Base {
  public:
    using parent_t = TestOrg_Base;
    using parent_t::phenotype;

    using genome_t = std::pair<int, std::string>;
  protected:
    genome_t genome;

  public:
    TestOrg_XWordLines(const genome_t & _g) : genome(_g) { ; }
    
    genome_t & GetGenome() { return genome; }
    const genome_t & GetGenome() const { return genome; }

    void CalcOut() { ; }
};

struct ProblemUtilities_XWordLines { emp::vector<std::function<double(TestOrg_XWordLines &)>> lexicase_fit_set; };



/// Pig Latin: String
class TestOrg_PigLatin : public TestOrg_Base {
  public:
    using parent_t = TestOrg_Base;
    using parent_t::phenotype;

    using genome_t = std::string;
  protected:
    genome_t genome;

  public:
    TestOrg_PigLatin(const genome_t & _g) : genome(_g) { ; }
    
    genome_t & GetGenome() { return genome; }
    const genome_t & GetGenome() const { return genome; }

    void CalcOut() { ; }
};

struct ProblemUtilities_PigLatin { emp::vector<std::function<double(TestOrg_PigLatin &)>> lexicase_fit_set; };



/// Negative To Zero: Vector<Integer>
class TestOrg_NegativeToZero : public TestOrg_Base {
  public:
    using parent_t = TestOrg_Base;
    using parent_t::phenotype;

    using genome_t = emp::vector<int>;
  protected:
    genome_t genome;

  public:
    TestOrg_NegativeToZero(const genome_t & _g) : genome(_g) { ; }
    
    genome_t & GetGenome() { return genome; }
    const genome_t & GetGenome() const { return genome; }

    void CalcOut() { ; }
};

struct ProblemUtilities_NegativeToZero { emp::vector<std::function<double(TestOrg_NegativeToZero &)>> lexicase_fit_set; };



/// Scrabble Score: String
class TestOrg_ScrabbleScore : public TestOrg_Base {
  public:
    using parent_t = TestOrg_Base;
    using parent_t::phenotype;

    using genome_t = std::string;
  protected:
    genome_t genome;

  public:
    TestOrg_ScrabbleScore(const genome_t & _g) : genome(_g) { ; }
    
    genome_t & GetGenome() { return genome; }
    const genome_t & GetGenome() const { return genome; }

    void CalcOut() { ; }
};

struct ProblemUtilities_ScrabbleScore { emp::vector<std::function<double(TestOrg_ScrabbleScore &)>> lexicase_fit_set; };



/// Word Stats: File
// class TestOrg_WordStats : public TestOrg_Base {
//   public:
//     using parent_t = TestOrg_Base;
//     using parent_t::phenotype;

//     using genome_t = ;
//   protected:
//     genome_t genome;

//   public:
//     TestOrg_WordStats (const genome_t & _g) : genome(_g) { ; }
    
//     genome_t & GetGenome() { return genome; }
//     const genome_t & GetGenome() const { return genome; }
// };


/// Checksum: String
class TestOrg_Checksum : public TestOrg_Base {
  public:
    using parent_t = TestOrg_Base;
    using parent_t::phenotype;

    using genome_t = std::string;
  protected:
    genome_t genome;

  public:
    TestOrg_Checksum(const genome_t & _g) : genome(_g) { ; }
    
    genome_t & GetGenome() { return genome; }
    const genome_t & GetGenome() const { return genome; }

    void CalcOut() { ; }
};

struct ProblemUtilities_Checksum { emp::vector<std::function<double(TestOrg_Checksum &)>> lexicase_fit_set; };



/// Digits: Integer
class TestOrg_Digits : public TestOrg_Base {
  public:
    using parent_t = TestOrg_Base;
    using parent_t::phenotype;

    using genome_t = int;
  protected:
    genome_t genome;

  public:
    TestOrg_Digits(const genome_t & _g) : genome(_g) { ; }
    
    genome_t & GetGenome() { return genome; }
    const genome_t & GetGenome() const { return genome; }

    void CalcOut() { ; }

};

struct ProblemUtilities_Digits { emp::vector<std::function<double(TestOrg_Digits &)>> lexicase_fit_set; };



/// Grade: Array<Integer, 5>
class TestOrg_Grade : public TestOrg_Base {
  public:
    using parent_t = TestOrg_Base;
    using parent_t::phenotype;

    using genome_t = std::array<int, 5>;
  protected:
    genome_t genome;

  public:
    TestOrg_Grade(const genome_t & _g) : genome(_g) { ; }
    
    genome_t & GetGenome() { return genome; }
    const genome_t & GetGenome() const { return genome; }

    void CalcOut() { ; }
};

struct ProblemUtilities_Grade { emp::vector<std::function<double(TestOrg_Grade &)>> lexicase_fit_set; };



/// Median: Array<Integer, 3>
class TestOrg_Median : public TestOrg_Base {
  public:
    using parent_t = TestOrg_Base;
    using parent_t::phenotype;

    using genome_t = std::array<int, 3>;
  protected:
    genome_t genome;

  public:
    TestOrg_Median(const genome_t & _g) : genome(_g) { ; }
    
    genome_t & GetGenome() { return genome; }
    const genome_t & GetGenome() const { return genome; }

    void CalcOut() { ; }
};

struct ProblemUtilities_Median { emp::vector<std::function<double(TestOrg_Median &)>> lexicase_fit_set; };



/// Smallest: Array<Integer, 4>
class TestOrg_Smallest : public TestOrg_Base {
  public:
    using parent_t = TestOrg_Base;
    using parent_t::phenotype;

    using genome_t = std::array<int, 4>;
  protected:
    genome_t genome;

  public:
    TestOrg_Smallest(const genome_t & _g) : genome(_g) { ; }
    
    genome_t & GetGenome() { return genome; }
    const genome_t & GetGenome() const { return genome; }

    void CalcOut() { ; }
};

struct ProblemUtilities_Smallest { emp::vector<std::function<double(TestOrg_Smallest &)>> lexicase_fit_set; };



/// Syllables: String
class TestOrg_Syllables : public TestOrg_Base {
  public:
    using parent_t = TestOrg_Base;
    using parent_t::phenotype;

    using genome_t = std::string;
  protected:
    genome_t genome;

  public:
    TestOrg_Syllables(const genome_t & _g) : genome(_g) { ; }
    
    genome_t & GetGenome() { return genome; }
    const genome_t & GetGenome() const { return genome; }

    void CalcOut() { ; }
};


struct ProblemUtilities_Syllables { emp::vector<std::function<double(TestOrg_Syllables &)>> lexicase_fit_set; };

#endif
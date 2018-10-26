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

// Problem_SmallOrLarge_input_t = int;
// Problem_ForLoopIndex_input_t = std::array<int, 3>;
// Problem_CompareStringLengths_input_t = std::array<std::string, 3>;
// Problem_DoubleLetters_input_t = std::string;
// Problem_CollatzNumbers_input_t = int;
// Problem_ReplaceSpaceWithNewline_input_t = std::string;
// Problem_StringDifferences_input_t = std::array<std::string, 2>;
// Problem_EvenSquares_input_t = int;
// Problem_WallisPi_input_t = int;
// Problem_StringLengthsBackwards_input_t = emp::vector<std::string>;
// Problem_LastIndexOfZero_input_t = emp::vector<int>;
// Problem_VectorAverage_input_t = emp::vector<double>;
// Problem_CountOdds_input_t = emp::vector<int>;
// Problem_MirrorImage_input_t = std::array<emp::vector<int>, 2>;
// Problem_SuperAnagrams_input_t = std::array<std::string, 2>;
// Problem_SumOfSquares_input_t = int;
// Problem_VectorsSummed_input_t = std::array<emp::vector<int>, 2>;
// Problem_XWordLines_input_t = std::pair<int, std::string>;
// Problem_PigLatin_input_t = std::string;
// Problem_NegativeToZero_input_t = emp::vector<int>;
// Problem_ScrabbleScore_input_t = std::string;
// Problem_Checksum_input_t = std::string;
// Problem_Digits_input_t = int;
// Problem_Grade_input_t = std::array<int, 5>;
// Problem_Median_input_t = std::array<int, 3>;
// Problem_Smallest_input_t = std::array<int, 4>;
// Problem_Syllables_input_t = std::string;

// ================ Problem Organism Classes ================

// Test org base class
class TestOrg_Base {
  public:

    struct Phenotype {
      emp::vector<double> test_results; ///< Results correspond to per-organism test results.
      size_t num_passes;
      size_t num_fails;

      void Reset(size_t s=0) {
        test_results.clear();
        test_results.resize(s, 0);
        num_passes = 0;
        num_fails = 0;
      }
    };

    using phenotype_t = Phenotype;

  protected:
    phenotype_t phenotype;

  public:

    phenotype_t & GetPhenotype() { return phenotype; }
    const phenotype_t & GetPhenotype() const { return phenotype; }

};


////////////////////////////////////////////////////////////////
// Problem: NumberIO
// - Input type: [double, integer]
// - Output type: double 
////////////////////////////////////////////////////////////////

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

double CalcScorePassFail_NumberIO(const Problem_NumberIO_output_t & correct_test_output, double sub) {
  return (double)sub == correct_test_output;
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

////////////////////////////////////////////////////////////////
// Problem: Small or large
// - Input: integer
// - Output: string {'small', 'large', ''}
////////////////////////////////////////////////////////////////

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

    void CalcOut() { out = GenCorrectOut_SmallOrLarge(genome); }
};

struct ProblemUtilities_SmallOrLarge { emp::vector<std::function<double(TestOrg_SmallOrLarge &)>> lexicase_fit_set; };



/// ForLoopIndex: Array<Integer, 3>
class TestOrg_ForLoopIndex : public TestOrg_Base {
  public:
    using parent_t = TestOrg_Base;
    using parent_t::phenotype;

    using genome_t = std::array<int, 3>;

  protected:
    genome_t genome;

  public:
    TestOrg_ForLoopIndex(const genome_t & _g) : genome(_g) { ; }
    
    genome_t & GetGenome() { return genome; }
    const genome_t & GetGenome() const { return genome; }
};

struct ProblemUtilities_ForLoopIndex { emp::vector<std::function<double(TestOrg_ForLoopIndex &)>> lexicase_fit_set; };



/// Compare String Lengths: Array<String, 3>
class TestOrg_CompareStringLengths: public TestOrg_Base {
  public:
    using parent_t = TestOrg_Base;
    using parent_t::phenotype;

    using genome_t = std::array<std::string, 3>;
  protected:
    genome_t genome;

  public:
    TestOrg_CompareStringLengths(const genome_t & _g) : genome(_g) { ; }
    
    genome_t & GetGenome() { return genome; }
    const genome_t & GetGenome() const { return genome; }
};

struct ProblemUtilities_CompareStringLengths { emp::vector<std::function<double(TestOrg_CompareStringLengths &)>> lexicase_fit_set; };



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
};

struct ProblemUtilities_DoubleLetters { emp::vector<std::function<double(TestOrg_DoubleLetters &)>> lexicase_fit_set; };



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
};


struct ProblemUtilities_Syllables { emp::vector<std::function<double(TestOrg_Syllables &)>> lexicase_fit_set; };

#endif
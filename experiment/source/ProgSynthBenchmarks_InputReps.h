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

// ================== Problem Utilities =========================

/*
struct ProgSynthProblemUtility {
  
  /// Input: std::pair<int, double>;
  /// Output: double;




  /// Input: std::array<int, 3>;
  /// Output: emp::vector<int>;
  static Problem_ForLoopIndex_output_t GenCorrectOut_ForLoopIndex(const Problem_ForLoopIndex_input_t & input) {
    emp::vector<int> out;
    SetCorrectOut_ForLoopIndex(input, out);
    return out;
  }
  static void SetCorrectOut_ForLoopIndex(const Problem_ForLoopIndex_input_t & input, Problem_ForLoopIndex_output_t & output) {
    out.clear();
    const size_t start = input[0]; 
    const size_t end = input[1];
    const size_t step = input[2];
    for (size_t i = start; i < end; i+= step) {
      out.emplace_back(i);
    }
  }


  /// std::array<std::string, 3>;
  /// bool;
  Problem_CompareStringLengths_output_t GenCorrectOut_CompareStringLengths(const Problem_CompareStringLengths_input_t & input) {
    // todo
    return false;
  }

  /// std::string;
  /// std::string;
  Problem_DoubleLetters_output_t GenCorrectOut_DoubleLetters(const Problem_DoubleLetters_input_t & input) {
    // todo
    return "";
  }

  /// int;
  /// int;
  Problem_CollatzNumbers_output_t GenCorrectOut_CollatzNumbers(const Problem_CollatzNumbers_input_t & input) {
    correct_out.clear();
    correct_out.emplace_back(test);
    while (correct_out.back() != 1) {
      if (correct_out.back() % 2 == 0) correct_out.emplace_back(correct_out.back() / 2);
      else correct_out.emplace_back((3*correct_out.back()) + 1);
    }
  }


  /// std::string;
  /// std::pair<std::string, int>;
  Problem_ReplaceSpaceWithNewline_output_t GenCorrectOut_ReplaceSpaceWithNewline(const Problem_ReplaceSpaceWithNewline_input_t & input) {


  }

  /// std::array<std::string, 2>;
  /// std::string;
  Problem_StringDifferences_output_t GenCorrectOut_StringDifferences(const Problem_StringDifferences_input_t & input) {



  }

  /// int;
  /// int;
  Problem_EvenSquares_output_t GenCorrectOut_EvenSquares(const Problem_EvenSquares_input_t & input) {


  }


  /// int;
  /// double;
  Problem_WallisPi_output_t GenCorrectOut_WallisPi(const Problem_WallisPi_input_t & input) {


  }

  /// emp::vector<std::string>;
  /// std::string;
  Problem_StringLengthsBackwards_output_t GenCorrectOut_StringLengthsBackwards(const Problem_StringLengthsBackwards_input_t & input) {



  }

  /// emp::vector<int>;
  /// int;
  Problem_LastIndexOfZero_output_t GenCorrectOut_LastIndexOfZero(const Problem_LastIndexOfZero_input_t & input) {


  }


  /// emp::vector<double>;
  /// double;
  Problem_VectorAverage_output_t GenCorrectOut_VectorAverage(const Problem_VectorAverage_input_t & input) {


  }

  /// emp::vector<int>;
  /// int;
  Problem_CountOdds_output_t GenCorrectOut_CountOdds(const Problem_CountOdds_input_t & input) {



  }

  /// std::array<emp::vector<int>, 2>;
  /// bool;
  Problem_MirrorImage_output_t GenCorrectOut_MirrorImage(const Problem_MirrorImage_input_t & input) {


  }


  /// std::array<std::string, 2>;
  /// bool;
  Problem_SuperAnagrams_output_t GenCorrectOut_SuperAnagrams(const Problem_SuperAnagrams_input_t & input) {


  }

  /// int;
  /// int;
  Problem_SumOfSquares_output_t GenCorrectOut_SumOfSquares(const Problem_SumOfSquares_input_t & input) {



  }

  /// std::array<emp::vector<int>, 2>;
  /// emp::vector<int>;
  Problem_VectorsSummed_output_t GenCorrectOut_VectorsSummed(const Problem_VectorsSummed_input_t & input) {


  }


  /// std::pair<int, std::string>;
  /// std::string;
  Problem_XWordLines_output_t GenCorrectOut_XWordLines(const Problem_XWordLines_input_t & input) {


  }

  /// std::string;
  /// std::string;
  Problem_PigLatin_output_t GenCorrectOut_PigLatin(const Problem_PigLatin_input_t & input) {



  }

  /// emp::vector<int>;
  /// emp::vector<int>;
  Problem_NegativeToZero_output_t GenCorrectOut_NegativeToZero(const Problem_NegativeToZero_input_t & input) {


  }
  /// std::string;
  /// int;
  Problem_ScrabbleScore_output_t GenCorrectOut_ScrabbleScore(const Problem_ScrabbleScore_input_t & input) {

  }
  /// std::string;
  /// std::string;
  Problem_Checksum_output_t GenCorrectOut_Checksum(const Problem_Checksum_input_t & input) {

  }
  /// int;
  /// emp::vector<int>;
  Problem_Digits_output_t GenCorrectOut_Digits(const Problem_Digits_input_t & input) {

  }
  /// std::array<int, 5>;
  /// std::string;
  Problem_Grade_output_t GenCorrectOut_Grade(const Problem_Grade_input_t & input) {

  }
  /// std::array<int, 3>;
  /// int;
  Problem_Median_output_t GenCorrectOut_Median(const Problem_Median_input_t & input) {

  }
  /// std::array<int, 4>;
  /// int;
  Problem_Smallest_output_t GenCorrectOut_Smallest(const Problem_Smallest_input_t & input) {

  }
  /// std::string;
  /// std::string;
  Problem_Syllables_output_t GenCorrectOut_Syllables(const Problem_Syllables_input_t & input) {

  }

};
*/

// ================ Problem Organism Classes ====================

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

    void SetOut(const out_t & _out) { out = _out; }

    void CalcOut() { SetCorrectOut_NumberIO(genome, out); }
};

struct ProblemUtilities_NumberIO {
  using input_t = Problem_NumberIO_input_t;
  using output_t = Problem_NumberIO_output_t;
  
  using testcase_set_t = TestCaseSet<input_t,output_t>;
  
  testcase_set_t testing_set;
  testcase_set_t training_set;

  // A few useful things for use within a test evaluation
  emp::Ptr<TestOrg_NumberIO> cur_eval_test_org;
  bool submitted;
  double submitted_val; // if going to do string thing, we can have a submission_str.

  ProblemUtilities_NumberIO() 
    : testing_set(ProblemUtilities_NumberIO::LoadTestCaseFromLine),
      training_set(ProblemUtilities_NumberIO::LoadTestCaseFromLine),
      submitted(false), submitted_val(0.0)
  { ; }

  testcase_set_t & GetTestingSet() { return testing_set; }
  testcase_set_t & GetTrainingSet() { return training_set; }

  void ResetTestEval() {
    submitted = false;
    submitted_val = 0.0;
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




#endif
#ifndef PROG_SYNTH_BENCHMARK_SUITE_H
#define PROG_SYNTH_BENCHMARK_SUITE_H

/**
 * @file ProgSynthBenchmarkSuite.h
 *
 * This file gives programmatic definitions to select benchmarks from the General
 * Program Synthesis Benchmark Suite (Helmuth and Spector, 2015). Each of these
 * benchmarks were initially defined in terms of input-output pairings; thus,
 * programmatic definitions are not strictly necessary. However, in this work,
 * programmatic definitions are convenient to facilitate test case evolution.
 * 
 * For now, I just provide implementations of non-string requiring benchmarks.
 */

 // TODO: 
 // - [ ] where relevant, setup eval cache(?); do this in organism?

#include <algorithm>
#include <unordered_set>
#include "base/array.h"
#include "base/vector.h"
#include "tools/math.h"
#include "tools/Random.h"
#include "tools/set_utils.h"
#include "tools/vector_utils.h"

/// Test case (1): Number IO
/// - Description: Given an integer and a float, print their sum.
/// - Input: int A; double B;
/// - Output: A + B
/// - Notes
///   - Float terminology used to keep consistent with problem description. Under
///     the hood (here), everything is actually represented as a double.
class NumberIOTest {
public:
  using test_t = emp::array<double, 2>;

protected:
  test_t test;

public:
  NumberIOTest()
    : test({0,0}) { ; }
  
  NumberIOTest(emp::Random & rnd, 
               int min_int_a=-100, int max_int_a=100,
               double min_float_b=-100.0, double max_float_b=100.0)
    : test({(double)rnd.GetInt(min_int_a, max_int_a+1), rnd.GetDouble(min_float_b, max_float_b)}) 
  {    
    emp_assert(min_int_a < max_int_a);
    emp_assert(min_float_b < max_float_b); 
  }

  NumberIOTest(NumberIOTest &&) = default;
  NumberIOTest(const NumberIOTest &) = default;

  NumberIOTest & operator=(const NumberIOTest &) = default;
  NumberIOTest & operator=(NumberIOTest &&) = default;

  bool operator==(const NumberIOTest & in) const { return in.test == test; }
  bool operator!=(const NumberIOTest & in) const { return !(in == *this); }
  bool operator<(const NumberIOTest & in) const { return test < in.test; }

  int GetInt() const { return (int)test[0]; }
  double GetFloat() const { return (double)test[1]; }
  void SetInt(int val) { test[0] = (double)val; }
  void SetFloat(double val) { test[1]=val; }

  test_t & GetTest() { return test; }

  void RandomizeTest(emp::Random & rnd,
                     int min_int_a=-100, int max_int_a=100,
                     double min_float_b=-100.0, double max_float_b=100.0) {
   emp_assert(min_int_a < max_int_a);
   emp_assert(min_float_b < max_float_b);
   test[0] = (double)rnd.GetInt(min_int_a, max_int_a+1);
   test[1] = rnd.GetDouble(min_float_b, max_float_b);            
  }
  
  bool Evaluate(double out) {
    return out == test[0] + test[1];
  }
  
  bool Validate(int min_int_a=-100, int max_int_a=100,
                double min_float_b=-100.0, double max_float_b=100.0) {
    emp_assert(min_int_a < max_int_a);
    emp_assert(min_float_b < max_float_b);
    return ((int)test[0] >= min_int_a && (int)test[0] <= max_int_a) && (test[1] >= min_float_b && test[1] < max_float_b);
  }
  
  void Print(std::ostream & out=std::cout) const {
    out << "[" << (int)test[0] << "," << test[1] << "]";
  }

};

/// Test case (2): Small or Large
/// - Description: Given an integer n, print 'small' if n < 1000 and 'large' if
///   n >= 2000 (and nothing if 1000 <= n < 2000).
/// - Input: n
/// - Output: 'small', 'large', ''
class SmallOrLargeTest {
public:
  static constexpr int SMALL_THRESH = 1000;
  static constexpr int LARGE_THRESH = 2000;
  enum INPUT_TYPES { SMALL=0, LARGE=1, NOTHING=2 };

  std::string SMALL_STR;
  std::string LARGE_STR;
  std::string NOTHING_STR;

protected:
  int test;
  
public:

  SmallOrLargeTest()
  : SMALL_STR("small"),
    LARGE_STR("large"),
    NOTHING_STR(""),
    test(0) { ; }
  
  SmallOrLargeTest(emp::Random & rnd, 
                   int min_n=-10000, int max_n=10000)
    : SMALL_STR("small"),
      LARGE_STR("large"),
      NOTHING_STR(""),
      test(rnd.GetInt(min_n, max_n+1)) 
  { emp_assert(min_n < max_n); }

  SmallOrLargeTest(SmallOrLargeTest &&) = default;
  SmallOrLargeTest(const SmallOrLargeTest &) = default;

  SmallOrLargeTest & operator=(const SmallOrLargeTest &) = default;
  SmallOrLargeTest & operator=(SmallOrLargeTest &&) = default;

  bool operator==(const SmallOrLargeTest & in) const { return in.test == test; }
  bool operator!=(const SmallOrLargeTest & in) const { return !(in == *this); }
  bool operator<(const SmallOrLargeTest & in) const { return test < in.test; }

  int GetTest() const { return test; }
  int GetN() const { return test; }
  void SetN(int val) { test = val; }

  void RandomizeTest(emp::Random & rnd, 
                     int min_n=-10000, int max_n=10000) {
    emp_assert(min_n < max_n);
    test = rnd.GetInt(min_n, max_n+1);            
  }
  
  bool Evaluate(INPUT_TYPES out) {
    if (test < SMALL_THRESH && out == INPUT_TYPES::SMALL) return true;
    else if (test >= LARGE_THRESH && out == INPUT_TYPES::LARGE) return true;
    else if (out == INPUT_TYPES::NOTHING) return true;
    else return false;
  }

  bool Evaluate(const std::string & out) {
    if (test < SMALL_THRESH && out == SMALL_STR) return true;
    else if (test >= LARGE_THRESH && out == LARGE_STR) return true;
    else if (out == NOTHING_STR) return true;
    else return false;
  }

  bool Validate(emp::Random & rnd, 
                int min_n=-10000, int max_n=10000) {
    return test >= min_n && test <= max_n;
  }

  void Print(std::ostream & out=std::cout) const {
    out << test;
  }

};

/// Test case (3): For Loop Index
/// - Description: Given 3 integer inputs start, end, and step, print the integers
///   in the sequence: 
///     n_{0}=start, n_{i} = n_{i-1} + step
///   for each n_i < end
class ForLoopIndexTest {
public:
  using test_t = emp::array<int, 3>;
  using out_t = emp::vector<int>;

protected:
  test_t test;
  out_t correct_out;

  void CalcOut() {
    correct_out.clear();
    for (size_t i = GetStart(); i < GetEnd(); i+= GetStep()) {
      correct_out.emplace_back(i);
    }
  }

public:
  ForLoopIndexTest()
    : test({0,0,0}), correct_out() { CalcOut(); }
  
  ForLoopIndexTest(emp::Random & rnd, 
                   int min_start=-500, int max_start=500,
                   int min_end=-500, int max_end=500,
                   int min_step=1, int max_step=10)
    : test({rnd.GetInt(min_start, max_start+1),
            rnd.GetInt(min_end, max_end+1), 
            rnd.GetInt(min_step, max_step+1)}),
      correct_out()
  { 
    emp_assert(min_start < max_start); 
    emp_assert(min_end < max_end);
    emp_assert(min_step < max_step);
    CalcOut();
  }

  ForLoopIndexTest(ForLoopIndexTest &&) = default;
  ForLoopIndexTest(const ForLoopIndexTest &) = default;

  ForLoopIndexTest & operator=(const ForLoopIndexTest &) = default;
  ForLoopIndexTest & operator=(ForLoopIndexTest &&) = default;

  bool operator==(const ForLoopIndexTest & in) const { return in.test == test; }
  bool operator!=(const ForLoopIndexTest & in) const { return !(in == *this); }
  bool operator<(const ForLoopIndexTest & in) const { return test < in.test; }

  int GetStart() const { return test[0]; }
  int GetEnd() const { return test[1]; }
  int GetStep() const { return test[2]; }

  const out_t & GetCorrectOutput() const { return correct_out; }
  
  void SetStart(int val) { test[0] = val; CalcOut(); }
  void SetEnd(int val) { test[1] = val; CalcOut(); }
  void SetStep(int val) { test[2] = val; CalcOut(); }
  void Set(int start, int end, int step) {
    test[0] = start;
    test[1] = end;
    test[2] = step;
    CalcOut();
  }

  void RandomizeTest(emp::Random & rnd,
                     int min_start=-500, int max_start=500,
                     int min_end=-500, int max_end=500,
                     int min_step=1, int max_step=10) {
    emp_assert(min_start < max_start); 
    emp_assert(min_end < max_end);
    emp_assert(min_step < max_step);
    test[0] = rnd.GetInt(min_start, max_start+1);
    test[1] = rnd.GetInt(min_end, max_end+1);
    test[2] = rnd.GetInt(min_step, max_step+1);
    CalcOut();
  }

  bool Evaluate(emp::vector<int> & out) {
    return out == correct_out;
  }

  bool Validate(int min_start=-500, int max_start=500,
                int min_end=-500, int max_end=500,
                int min_step=1, int max_step=10) {
    return (test[0] >= min_start && test[0] <= max_start) &&
           (test[1] >= min_end && test[1] <= max_end) &&
           (test[2] >= min_step && test[2] <= max_step);
  }

  void Print(std::ostream & out=std::cout) const {
    out << "[" << test[0] << "," << test[1] << "," << test[2] << "]";
  }

};

/// Test case (6): Collatz Numbers
/// - Description: Given an integer, find the number of terms in the Collatz (hailstone)
///   sequence starting from that integer.
class CollatzNumbersTest {
public:
  using out_t = emp::vector<size_t>;
  
  static constexpr size_t MIN_TEST_VAL = 1;

protected:
  size_t test;
  out_t correct_out;

  void CalcOut() {
    correct_out.clear();
    correct_out.emplace_back(test);
    while (correct_out.back() != 1) {
      if (correct_out.back() % 2 == 0) correct_out.emplace_back(correct_out.back() / 2);
      else correct_out.emplace_back((3*correct_out.back()) + 1);
    }
  }

public:

  CollatzNumbersTest()
    : test(1), correct_out() { CalcOut(); }
  
  CollatzNumbersTest(emp::Random & rnd, 
                     size_t max_val=10000)
    : test(rnd.GetUInt(MIN_TEST_VAL, max_val+1)),
      correct_out()
  { 
    emp_assert(max_val >= MIN_TEST_VAL); 
    CalcOut();
  }

  CollatzNumbersTest(CollatzNumbersTest &&) = default;
  CollatzNumbersTest(const CollatzNumbersTest &) = default;

  CollatzNumbersTest & operator=(const CollatzNumbersTest &) = default;
  CollatzNumbersTest & operator=(CollatzNumbersTest &&) = default;

  bool operator==(const CollatzNumbersTest & in) const { return in.test == test; }
  bool operator!=(const CollatzNumbersTest & in) const { return !(in == *this); }
  bool operator<(const CollatzNumbersTest & in) const { return test < in.test; }

  const out_t & GetCorrectOutput() const { return correct_out; }
  size_t GetTest() const { return test; }

  void SetTest(size_t val) { test = val; CalcOut(); }

  void RandomizeTest(emp::Random & rnd, size_t max_val=10000) {
    emp_assert(max_val >= MIN_TEST_VAL);
    test = rnd.GetUInt(MIN_TEST_VAL, max_val+1);
    CalcOut();
  }

  bool Evaluate(const out_t & out) {
    return out == correct_out;
  }

  bool Validate(size_t max_val=10000) {
    return test >= MIN_TEST_VAL && test <= max_val;
  }

  void Print(std::ostream & out=std::cout) const {
    out << test;
  }
};


/// Test case (9): Even Squares
/// - Description: Given an integer n, print all positive even perfect squres
///   less than n on separate lines.
/// - Note: squares of even numbers are even, squares of odd numbers are odd
class EvenSquaresTest {
public:
  using out_t = emp::vector<size_t>;

  static constexpr size_t MIN_TEST_VAL = 1;
  static constexpr size_t DEFAULT_MAX_TEST_VAL=9999;

protected:
  size_t test;
  out_t correct_out;

  void CalcOut() {
    correct_out.clear();
    for (size_t i = 2; i*i < test; i+= 2) {
      correct_out.emplace_back(i*i);
    }
  }

public:

  EvenSquaresTest()
    : test(1), correct_out() { CalcOut(); }
  
  EvenSquaresTest(emp::Random & rnd, 
                     size_t max_val=DEFAULT_MAX_TEST_VAL)
    : test(rnd.GetUInt(MIN_TEST_VAL, max_val+1)),
      correct_out()
  { 
    emp_assert(max_val >= MIN_TEST_VAL); 
    CalcOut();
  }

  EvenSquaresTest(EvenSquaresTest &&) = default;
  EvenSquaresTest(const EvenSquaresTest &) = default;

  EvenSquaresTest & operator=(const EvenSquaresTest &) = default;
  EvenSquaresTest & operator=(EvenSquaresTest &&) = default;

  bool operator==(const EvenSquaresTest & in) const { return in.test == test; }
  bool operator!=(const EvenSquaresTest & in) const { return !(in == *this); }
  bool operator<(const EvenSquaresTest & in) const { return test < in.test; }

  const out_t & GetCorrectOutput() const { return correct_out; }
  size_t GetTest() const { return test; }

  void SetTest(size_t val) { test = val; CalcOut(); }

  void RandomizeTest(emp::Random & rnd, size_t max_val=DEFAULT_MAX_TEST_VAL) {
    emp_assert(max_val >= MIN_TEST_VAL);
    test = rnd.GetUInt(MIN_TEST_VAL, max_val+1);
    CalcOut();
  }

  bool Evaluate(const out_t & out) {
    return out == correct_out;
  }

  bool Validate(size_t max_val=DEFAULT_MAX_TEST_VAL) {
    return test >= MIN_TEST_VAL && test <= max_val;
  }

  void Print(std::ostream & out=std::cout) const {
    out << test;
  }  

};

/// Test case (10): Wallis Pi
/// - Description: John Wallis gave the following infinite product that converges
///   to pi/4:
///     2/3 * 4/3 * 4/5 * 6/5 * 6/7 * 8/7 * 8/9 * 10/9 * ...
///   Given an integer input n, compute an approximation of this product out to
///   n terms. Results are rounded to 5 decimal places.
class WallisPiTest {
public:
  static constexpr double EPSILON = 0.000005;
  static constexpr size_t MIN_TEST_VAL = 1;
  static constexpr size_t DEFAULT_MAX_TEST_VAL = 200;

protected:
  int test;

public:

  WallisPiTest() : test(MIN_TEST_VAL) { ; }

  WallisPiTest(emp::Random & rnd, size_t max_test_val=DEFAULT_MAX_TEST_VAL)
    : test()
  { RandomizeTest(rnd, max_test_val); }

  WallisPiTest(WallisPiTest &&) = default;
  WallisPiTest(const WallisPiTest &) = default;

  WallisPiTest & operator=(const WallisPiTest &) = default;
  WallisPiTest & operator=(WallisPiTest &&) = default;

  bool operator==(const WallisPiTest & in) const { return in.test == test; }
  bool operator!=(const WallisPiTest & in) const { return !(in == *this); }
  bool operator<(const WallisPiTest & in) const { return test < in.test; }

  void RandomizeTest(emp::Random & rnd, size_t max_test_val=DEFAULT_MAX_TEST_VAL) {
    test = rnd.GetInt(MIN_TEST_VAL, max_test_val+1);
  }

  bool Evaluate(double out) {
    // Calculate wallis pi n out
    double num = 2.0;
    double denom = 3.0;
    bool inc_num = true;
    bool inc_denom = false;
    double approx = 0;
    for (size_t i = 0; i <= test; ++i) {
      approx *= num/denom;
      if (inc_num) { num+=2; }
      if (inc_denom) { denom+=2; }
      std::swap(inc_num, inc_denom);
    }
    // Is out ~correct?
    if ((emp::Abs(approx) < emp::Abs(out) + EPSILON) && (emp::Abs(approx) >= emp::Abs(out) - EPSILON)) {
      return true;
    }
    return false;
  }

  bool Validate(size_t max_test_val=DEFAULT_MAX_TEST_VAL) {
    return test >= MIN_TEST_VAL && test <= max_test_val;
  }

  void Print(std::ostream & out=std::cout) const {
    out << test;
  }

};

/// Test case (12): Last Index of Zero
/// - Description: Given a vector of integers, at least one of which is 0, return
///   the index of the last occurrence of 0 in the vector.
class LastIndexOfZeroTest {
public:
  using test_t = emp::vector<int>;

  static constexpr size_t MIN_TEST_LEN = 1;
  static constexpr size_t DEFAULT_MAX_LEN = 50;
  static constexpr int DEFAULT_MAX_VAL = 50;
  static constexpr int DEFAULT_MIN_VAL = -50;
    
protected:
  test_t test;

public:

  LastIndexOfZeroTest()
    : test(MIN_TEST_LEN, 0) { ; }
  
  LastIndexOfZeroTest(emp::Random & rnd, size_t max_test_len=DEFAULT_MAX_LEN,
                      int min_val=DEFAULT_MIN_VAL, int max_val=DEFAULT_MAX_VAL)
    : test() { RandomizeTest(rnd, max_test_len, min_val, max_val); }

  LastIndexOfZeroTest(LastIndexOfZeroTest &&) = default;
  LastIndexOfZeroTest(const LastIndexOfZeroTest &) = default;

  LastIndexOfZeroTest & operator=(const LastIndexOfZeroTest &) = default;
  LastIndexOfZeroTest & operator=(LastIndexOfZeroTest &&) = default;

  bool operator==(const LastIndexOfZeroTest & in) const { return in.test == test; }
  bool operator!=(const LastIndexOfZeroTest & in) const { return !(in == *this); }
  bool operator<(const LastIndexOfZeroTest & in) const { return test < in.test; }
  
  test_t & GetTest() { return test; }

  void RandomizeTest(emp::Random & rnd, size_t max_test_len=DEFAULT_MAX_LEN,
                     int min_val=DEFAULT_MIN_VAL, int max_val=DEFAULT_MAX_VAL) {
    emp_assert(max_test_len >= MIN_TEST_LEN);
    emp_assert(min_val <= max_val);
    size_t new_size = rnd.GetUInt(MIN_TEST_LEN, max_test_len+1);
    test.resize(new_size, 0);
    size_t zero_cnt = 0;
    for (size_t i = 0; i < test.size(); ++i) {
      const int val = rnd.GetInt(min_val, max_val+1);
      if (val == 0) ++zero_cnt;
      test[i] = val;
    }
    // Guarantee that at least one value is a zero.
    if (!zero_cnt) { test[rnd.GetUInt(test.size())] = 0; }
  }

  bool Evaluate(size_t out) {
    int last_zero = -1;
    for (size_t i = 0; i < test.size(); ++i) {
      if (test[i] == 0) last_zero = (int)i;
    }
    return (int)out == last_zero;
  }

  bool Validate(size_t max_test_len=DEFAULT_MAX_LEN,
                int min_val=DEFAULT_MIN_VAL, int max_val=DEFAULT_MAX_VAL) {
    if (test.size() > max_test_len || test.size() < MIN_TEST_LEN) return false;
    for (size_t i = 0; i < test.size(); ++i) {
      if (test[i] > max_val || test[i] < min_val) return false;
    }
    return true;
  }

  void Print(std::ostream & out=std::cout) const {
    out << "[";
    for (size_t i = 0; i < test.size(); ++i) {
      if (i) out << ",";
      out << test[i];
    }
    out << "]";
  }
};

/// Test case (13): Vector average
/// - Description: Given a vector of floats, return the average of those floats.
///   Results are rounded to 4 decimal places.
class VectorAverageTest {
public:
  using test_t = emp::vector<double>;

  static constexpr double EPSILON = 0.00005;
  static constexpr size_t MIN_TEST_LEN = 1;
  static constexpr size_t DEFAULT_MAX_TEST_LEN = 50;
  static constexpr double DEFAULT_MAX_TEST_VAL = 1000.0;
  static constexpr double DEFAULT_MIN_TEST_VAL = -1000.0;

protected:
  test_t test;

public:
  VectorAverageTest() : test(MIN_TEST_LEN, 0.0) { ; }

  VectorAverageTest(emp::Random & rnd, 
                    size_t max_test_len=DEFAULT_MAX_TEST_LEN,
                    size_t min_test_val=DEFAULT_MIN_TEST_VAL,
                    size_t max_test_val=DEFAULT_MAX_TEST_VAL)
    : test() { RandomizeTest(rnd, max_test_len, min_test_val, max_test_val); }

  VectorAverageTest(VectorAverageTest &&) = default;
  VectorAverageTest(const VectorAverageTest &) = default;

  VectorAverageTest & operator=(const VectorAverageTest &) = default;
  VectorAverageTest & operator=(VectorAverageTest &&) = default;

  bool operator==(const VectorAverageTest & in) const { return in.test == test; }
  bool operator!=(const VectorAverageTest & in) const { return !(in == *this); }
  bool operator<(const VectorAverageTest & in) const { return test < in.test; }

  test_t & GetTest() { return test; }

  void RandomizeTest(emp::Random & rnd,
                     size_t max_test_len=DEFAULT_MAX_TEST_LEN,
                     size_t min_test_val=DEFAULT_MIN_TEST_VAL,
                     size_t max_test_val=DEFAULT_MAX_TEST_VAL) {
    test.resize(rnd.GetUInt(MIN_TEST_LEN, max_test_len+1), 0.0);
    for (size_t i = 0; i < test.size(); ++i) {
      test[i] = rnd.GetDouble(min_test_val, max_test_val);
    }
  }

  bool Evaluate(double out) {
    emp_assert(test.size());
    const double avg = emp::Sum(test) / test.size();
    if ((emp::Abs(avg) < emp::Abs(out) + EPSILON) && (emp::Abs(avg) >= emp::Abs(out) - EPSILON)) {
      return true;
    }
    return false;
  }

  bool Validate(size_t max_test_len=DEFAULT_MAX_TEST_LEN,
                size_t min_test_val=DEFAULT_MIN_TEST_VAL,
                size_t max_test_val=DEFAULT_MAX_TEST_VAL) {
    if (test.size() < MIN_TEST_LEN || test.size() > max_test_len) return false;
    for (size_t i = 0; i < test.size(); ++i) {
      if (test[i] < min_test_val || test[i] >= max_test_val) return false;
    }
    return true;
  }

  void Print(std::ostream & out=std::cout) const {
    out << "[";
    for (size_t i = 0; i < test.size(); ++i) {
      if (i) out << ",";
      out << test[i];
    }
    out << "]";
  }

};

/// Test case (14): Count odds
/// - Description: Given a vector of integers, return the number of integers that
///   are odd, without use of a specific even or odd instruction (but allowing
///   instructions such as mod or quotient).
class CountOddsTest {
public:
  using test_t = emp::vector<int>;

  static constexpr size_t DEFAULT_MIN_TEST_LEN=0;
  static constexpr size_t DEFAULT_MAX_TEST_LEN=50;
  static constexpr int DEFAULT_MIN_TEST_VAL=-1000.0;
  static constexpr int DEFAULT_MAX_TEST_VAL=1000.0;

protected:
  test_t test;

public:

  CountOddsTest() : test() { ; }
  CountOddsTest(emp::Random & rnd,
                size_t min_test_len=DEFAULT_MIN_TEST_LEN, size_t max_test_len=DEFAULT_MAX_TEST_LEN,
                int min_test_val=DEFAULT_MIN_TEST_VAL, int max_test_val=DEFAULT_MAX_TEST_VAL)
    : test() { RandomizeTest(rnd, min_test_len, max_test_len, min_test_val, max_test_val); }

  CountOddsTest(CountOddsTest &&) = default;
  CountOddsTest(const CountOddsTest &) = default;

  CountOddsTest & operator=(const CountOddsTest &) = default;
  CountOddsTest & operator=(CountOddsTest &&) = default;

  bool operator==(const CountOddsTest & in) const { return in.test == test; }
  bool operator!=(const CountOddsTest & in) const { return !(in == *this); }
  bool operator<(const CountOddsTest & in) const { return test < in.test; }

  void RandomizeTest(emp::Random & rnd,
                     size_t min_test_len=DEFAULT_MIN_TEST_LEN, size_t max_test_len=DEFAULT_MAX_TEST_LEN,
                     int min_test_val=DEFAULT_MIN_TEST_VAL, int max_test_val=DEFAULT_MAX_TEST_VAL) {
    test.resize(rnd.GetUInt(min_test_len, max_test_len+1), min_test_val);
    for (size_t i = 0; i < test.size(); ++i) {
      test[i] = rnd.GetInt(min_test_val, max_test_val+1);
    }
  }

  bool Evaluate(size_t out) {
    size_t cnt = 0;
    for (size_t i = 0; i < test.size(); ++i) {
      if (test[i] & 1) ++cnt;
    }
    return out == cnt;
  }

  bool Validate(size_t min_test_len=DEFAULT_MIN_TEST_LEN, size_t max_test_len=DEFAULT_MAX_TEST_LEN,
                int min_test_val=DEFAULT_MIN_TEST_VAL, int max_test_val=DEFAULT_MAX_TEST_VAL) {
    if (test.size() > max_test_len || test.size() < min_test_len) return false;
    for (size_t i = 0; i < test.size(); ++i) {
      if (test[i] > max_test_val || test[i] < min_test_val) return false;
    }
    return true;
  }

  void Print(std::ostream & out=std::cout) const {
    out << "[";
    for (size_t i = 0; i < test.size(); ++i) {
      if (i) out << ",";
      out << test[i];
    }
    out << "]";
  }

};

/// Test case (15): Mirror image
/// - Description: Given two vectors of integers, return true if one vector is the
///   reverse of the other, and false otherwise.
class MirrorImageTest {
public:
  using test_t = emp::array< emp::vector<int>, 2 >;

  static constexpr size_t DEFAULT_MIN_VEC_LEN = 0;
  static constexpr size_t DEFAULT_MAX_VEC_LEN = 50;
  static constexpr int DEFAULT_MIN_VAL = -1000;
  static constexpr int DEFAULT_MAX_VAL = 1000;

protected:
  test_t test;

public:

  MirrorImageTest() : test() { ; }
  MirrorImageTest(emp::Random & rnd,
                size_t min_vec_len=DEFAULT_MIN_VEC_LEN, size_t max_vec_len=DEFAULT_MAX_VEC_LEN,
                int min_val=DEFAULT_MIN_VAL, int max_val=DEFAULT_MAX_VAL)
    : test() { RandomizeTest(rnd, min_vec_len, max_vec_len, min_val, max_val); }

  MirrorImageTest(MirrorImageTest &&) = default;
  MirrorImageTest(const MirrorImageTest &) = default;

  MirrorImageTest & operator=(const MirrorImageTest &) = default;
  MirrorImageTest & operator=(MirrorImageTest &&) = default;

  bool operator==(const MirrorImageTest & in) const { return in.test == test; }
  bool operator!=(const MirrorImageTest & in) const { return !(in == *this); }
  bool operator<(const MirrorImageTest & in) const { return test < in.test; }

  void RandomizeTest(emp::Random & rnd,
                     size_t min_vec_len=DEFAULT_MIN_VEC_LEN, size_t max_vec_len=DEFAULT_MAX_VEC_LEN,
                     int min_val=DEFAULT_MIN_VAL, int max_val=DEFAULT_MAX_VAL) {
    size_t vec_len = rnd.GetUInt(min_vec_len, max_vec_len+1);
    test[0].resize(vec_len, 0);
    test[1].resize(vec_len, 0);
    for (size_t i = 0; i < vec_len; ++i) {
      test[0][i] = rnd.GetInt(min_val, max_val+1);
      test[1][i] = rnd.GetInt(min_val, max_val+1);
    }
  }

  bool Evaluate(bool out) {
    emp_assert(test[0].size() == test[1].size());
    const size_t vec_len = test[0].size();
    bool mirror = true;
    for (size_t i = 0; i < vec_len; ++i) {
      if (test[0][i] != test[1][vec_len-1-i]) mirror = false;
    }
    return out == mirror;
  }

  bool Validate(size_t min_vec_len=DEFAULT_MIN_VEC_LEN, size_t max_vec_len=DEFAULT_MAX_VEC_LEN,
                int min_val=DEFAULT_MIN_VAL, int max_val=DEFAULT_MAX_VAL) {
    if (test[0].size() != test[1].size()) return false;
    if (test[0].size() < min_vec_len || test[0].size() > max_vec_len) return false;
    for (size_t i = 0; i < test[0].size(); ++i) {
      if (test[0][i] < min_val || test[0][i] > max_val) return false;
      if (test[1][i] < min_val || test[1][i] > max_val) return false;
    }
    return true;
  }

  void Print(std::ostream & out=std::cout) const {
    out << "[[";
    for (size_t i = 0; i < test[0].size(); ++i) {
      if (i) out << ",";
      out << test[0][i];
    }
    out << "],";
    for (size_t i = 0; i < test[1].size(); ++i) {
      if (i) out << ",";
      out << test[1][i];
    }
    out << "]]";
  }
};

/// Test case (17): Sum of Squares
/// - Description: Given integer n, return the sum of squaring each integer in the
///   range [1, n].
class SumOfSquaresTest {
public:
  static constexpr int DEFAULT_MIN_N = 1;
  static constexpr int DEFAULT_MAX_N = 100;

protected:
  int test;

public:

  SumOfSquaresTest()
  : test(DEFAULT_MIN_N) { ; }
  
  SumOfSquaresTest(emp::Random & rnd, 
                   int min_n=DEFAULT_MIN_N, int max_n=DEFAULT_MAX_N)
    : test(DEFAULT_MIN_N)
  { RandomizeTest(rnd, min_n, max_n); }

  SumOfSquaresTest(SumOfSquaresTest &&) = default;
  SumOfSquaresTest(const SumOfSquaresTest &) = default;

  SumOfSquaresTest & operator=(const SumOfSquaresTest &) = default;
  SumOfSquaresTest & operator=(SumOfSquaresTest &&) = default;

  bool operator==(const SumOfSquaresTest & in) const { return in.test == test; }
  bool operator!=(const SumOfSquaresTest & in) const { return !(in == *this); }
  bool operator<(const SumOfSquaresTest & in) const { return test < in.test; }

  void RandomizeTest(emp::Random & rnd, 
                     int min_n=DEFAULT_MIN_N, int max_n=DEFAULT_MAX_N) {
    emp_assert(min_n <= max_n);
    test = rnd.GetInt(min_n, max_n+1);
  }
  
  bool Evaluate(int out) {
    int sum = 0;
    for (size_t i = 1; i <= test; ++i) {
      sum += i*i;
    }
    return out == sum;
  }

  bool Validate(emp::Random & rnd, 
                int min_n=DEFAULT_MIN_N, int max_n=DEFAULT_MAX_N) {
    return test <= max_n && test >= min_n;
  }

  void Print(std::ostream & out=std::cout) const {
    out << test;
  }

};

/// Test case (18): Vectors summed
/// - Description: Given two equal-sized vectors of integers, return a vector
///   of integers that contains the sum of the input vectors at each index.
class VectorsSummedTest {
public:
  using test_t = emp::array< emp::vector<int>, 2 >;
  using out_t = emp::vector<int>;

  static constexpr size_t DEFAULT_MIN_VEC_LEN = 0;
  static constexpr size_t DEFAULT_MAX_VEC_LEN = 50;
  static constexpr int DEFAULT_MIN_VAL = -1000;
  static constexpr int DEFAULT_MAX_VAL = 1000;

protected:
  test_t test;

public:

  VectorsSummedTest() : test() { ; }
  VectorsSummedTest(emp::Random & rnd,
                size_t min_vec_len=DEFAULT_MIN_VEC_LEN, size_t max_vec_len=DEFAULT_MAX_VEC_LEN,
                int min_val=DEFAULT_MIN_VAL, int max_val=DEFAULT_MAX_VAL)
    : test() { RandomizeTest(rnd, min_vec_len, max_vec_len, min_val, max_val); }

  VectorsSummedTest(VectorsSummedTest &&) = default;
  VectorsSummedTest(const VectorsSummedTest &) = default;

  VectorsSummedTest & operator=(const VectorsSummedTest &) = default;
  VectorsSummedTest & operator=(VectorsSummedTest &&) = default;

  bool operator==(const VectorsSummedTest & in) const { return in.test == test; }
  bool operator!=(const VectorsSummedTest & in) const { return !(in == *this); }
  bool operator<(const VectorsSummedTest & in) const { return test < in.test; }

  void RandomizeTest(emp::Random & rnd,
                     size_t min_vec_len=DEFAULT_MIN_VEC_LEN, size_t max_vec_len=DEFAULT_MAX_VEC_LEN,
                     int min_val=DEFAULT_MIN_VAL, int max_val=DEFAULT_MAX_VAL) {
    size_t vec_len = rnd.GetUInt(min_vec_len, max_vec_len+1);
    test[0].resize(vec_len, 0);
    test[1].resize(vec_len, 0);
    for (size_t i = 0; i < vec_len; ++i) {
      test[0][i] = rnd.GetInt(min_val, max_val+1);
      test[1][i] = rnd.GetInt(min_val, max_val+1);
    }
  }

  bool Evaluate(const out_t & out) {
    emp_assert(test[0].size() == test[1].size());
    emp_assert(test[0].size() == out.size());
    for (size_t i = 0; i < out.size(); ++i) {
      if (out[i] != test[0][i] + test[1][i]) return false;
    }
    return true;
  }

  bool Validate(size_t min_vec_len=DEFAULT_MIN_VEC_LEN, size_t max_vec_len=DEFAULT_MAX_VEC_LEN,
                int min_val=DEFAULT_MIN_VAL, int max_val=DEFAULT_MAX_VAL) {
    if (test[0].size() != test[1].size()) return false;
    if (test[0].size() < min_vec_len || test[0].size() > max_vec_len) return false;
    for (size_t i = 0; i < test[0].size(); ++i) {
      if (test[0][i] < min_val || test[0][i] > max_val) return false;
      if (test[1][i] < min_val || test[1][i] > max_val) return false;
    }
    return true;
  }

  void Print(std::ostream & out=std::cout) const {
    out << "[[";
    for (size_t i = 0; i < test[0].size(); ++i) {
      if (i) out << ",";
      out << test[0][i];
    }
    out << "],";
    for (size_t i = 0; i < test[1].size(); ++i) {
      if (i) out << ",";
      out << test[1][i];
    }
    out << "]]";
  }
};

/// Test case (21): Negative to zero
/// - Description: Given a vector of integers, return the vector where all negative
///   integers have been replaced by 0.
class NegativeToZeroTest {
public:
  using test_t = emp::vector<int>;
  using out_t = emp::vector<int>;

  static constexpr size_t DEFAULT_MIN_TEST_LEN=0;
  static constexpr size_t DEFAULT_MAX_TEST_LEN=50;
  static constexpr int DEFAULT_MIN_TEST_VAL=-1000.0;
  static constexpr int DEFAULT_MAX_TEST_VAL=1000.0;

protected:
  test_t test;

public:

  NegativeToZeroTest() : test() { ; }
  NegativeToZeroTest(emp::Random & rnd,
                size_t min_test_len=DEFAULT_MIN_TEST_LEN, size_t max_test_len=DEFAULT_MAX_TEST_LEN,
                int min_test_val=DEFAULT_MIN_TEST_VAL, int max_test_val=DEFAULT_MAX_TEST_VAL)
    : test() { RandomizeTest(rnd, min_test_len, max_test_len, min_test_val, max_test_val); }

  NegativeToZeroTest(NegativeToZeroTest &&) = default;
  NegativeToZeroTest(const NegativeToZeroTest &) = default;

  NegativeToZeroTest & operator=(const NegativeToZeroTest &) = default;
  NegativeToZeroTest & operator=(NegativeToZeroTest &&) = default;

  bool operator==(const NegativeToZeroTest & in) const { return in.test == test; }
  bool operator!=(const NegativeToZeroTest & in) const { return !(in == *this); }
  bool operator<(const NegativeToZeroTest & in) const { return test < in.test; }

  void RandomizeTest(emp::Random & rnd,
                     size_t min_test_len=DEFAULT_MIN_TEST_LEN, size_t max_test_len=DEFAULT_MAX_TEST_LEN,
                     int min_test_val=DEFAULT_MIN_TEST_VAL, int max_test_val=DEFAULT_MAX_TEST_VAL) {
    test.resize(rnd.GetUInt(min_test_len, max_test_len+1), min_test_val);
    for (size_t i = 0; i < test.size(); ++i) {
      test[i] = rnd.GetInt(min_test_val, max_test_val+1);
    }
  }

  bool Evaluate(const out_t & out) {
    for (size_t i = 0; i < test.size(); ++i) {
      if (test[i] < 0 && out[i] != 0) return false;
      else if (out[i] != test[i]) return false;
    }
    return true;
  }

  bool Validate(size_t min_test_len=DEFAULT_MIN_TEST_LEN, size_t max_test_len=DEFAULT_MAX_TEST_LEN,
                int min_test_val=DEFAULT_MIN_TEST_VAL, int max_test_val=DEFAULT_MAX_TEST_VAL) {
    if (test.size() > max_test_len || test.size() < min_test_len) return false;
    for (size_t i = 0; i < test.size(); ++i) {
      if (test[i] > max_test_val || test[i] < min_test_val) return false;
    }
    return true;
  }

  void Print(std::ostream & out=std::cout) const {
    out << "[";
    for (size_t i = 0; i < test.size(); ++i) {
      if (i) out << ",";
      out << test[i];
    }
    out << "]";
  }

};

/// Test case (25): Digits
/// - Description: Given an integer, print that integer's digits each on their
///   own line starting with the least significant digit. A negative integer
///   should have the negative sign printed before the most significant digit.
class DigitsTest {
public:
  using out_t = emp::vector<int>;

  static constexpr int DEFAULT_MIN_N = 1;
  static constexpr int DEFAULT_MAX_N = 100;

protected:
  int test;

public:

  DigitsTest()
  : test(DEFAULT_MIN_N) { ; }
  
  DigitsTest(emp::Random & rnd, 
             int min_n=DEFAULT_MIN_N, int max_n=DEFAULT_MAX_N)
    : test(DEFAULT_MIN_N)
  { RandomizeTest(rnd, min_n, max_n); }

  DigitsTest(DigitsTest &&) = default;
  DigitsTest(const DigitsTest &) = default;

  DigitsTest & operator=(const DigitsTest &) = default;
  DigitsTest & operator=(DigitsTest &&) = default;

  bool operator==(const DigitsTest & in) const { return in.test == test; }
  bool operator!=(const DigitsTest & in) const { return !(in == *this); }
  bool operator<(const DigitsTest & in) const { return test < in.test; }

  void RandomizeTest(emp::Random & rnd, 
                     int min_n=DEFAULT_MIN_N, int max_n=DEFAULT_MAX_N) {
    emp_assert(min_n <= max_n);
    test = rnd.GetInt(min_n, max_n+1);
  }
  
  // todo - check that neg num comes out properly
  bool Evaluate(const out_t & out) {
    int n = test;
    emp::vector<int> digits;
    while (n) {
      digits.emplace_back(n%10);
      n/=10;
    }
    return out == digits;
  }

  bool Validate(emp::Random & rnd, 
                int min_n=DEFAULT_MIN_N, int max_n=DEFAULT_MAX_N) {
    return test <= max_n && test >= min_n;
  }

  void Print(std::ostream & out=std::cout) const {
    out << test;
  }

};

/// Test case (26): Grade
/// - Description: Given 5 integers, the first four represent the lower numeric
///   thresholds for achieving an A, B, C, and D, and will be distinct and in
///   descending order. The fifth represents the student's numeric grade. The program
///   must print 'Student has a X grade.', where X is A, B, C, D, of F depending
///   on the thresholds and the numeric grade.
class GradeTest {
public:
  using test_t = emp::array<int, 5>;

  enum GRADE_TYPE { A=0, B=1, C=2, D=3, F=4 };

  static constexpr int DEFAULT_MIN_GRADE_VAL = 0;
  static constexpr int DEFAULT_MAX_GRADE_VAL = 100;

protected:
  test_t test;

public:
  GradeTest()
    : test({DEFAULT_MIN_GRADE_VAL, 
            DEFAULT_MIN_GRADE_VAL+1, 
            DEFAULT_MIN_GRADE_VAL+2,
            DEFAULT_MIN_GRADE_VAL+3,
            DEFAULT_MIN_GRADE_VAL}) { ; }
  
  GradeTest(emp::Random & rnd, 
            int min_g=DEFAULT_MIN_GRADE_VAL, int max_g=DEFAULT_MAX_GRADE_VAL)
    : test({0,0,0,0,0})
  { RandomizeTest(rnd, min_g, max_g); }

  GradeTest(GradeTest &&) = default;
  GradeTest(const GradeTest &) = default;

  GradeTest & operator=(const GradeTest &) = default;
  GradeTest & operator=(GradeTest &&) = default;

  bool operator==(const GradeTest & in) const { return in.test == test; }
  bool operator!=(const GradeTest & in) const { return !(in == *this); }
  bool operator<(const GradeTest & in) const { return test < in.test; }

  int GetAThresh() const { return test[0]; }
  int GetBThresh() const { return test[1]; }
  int GetCThresh() const { return test[2]; }
  int GetDThresh() const { return test[3]; }
  int GetStuGrade() const { return test[4]; }

  void RandomizeTest(emp::Random & rnd, 
                     int min_g=DEFAULT_MIN_GRADE_VAL, int max_g=DEFAULT_MAX_GRADE_VAL) {
    emp_assert(min_g <= max_g);
    emp_assert(max_g - min_g > 5);
    emp::vector<int> thresholds(4, 0);
    std::unordered_set<int> set;
    for (size_t i = 0; i < thresholds.size(); ++i) {
      while (true) {
       int candidate_thresh = rnd.GetInt(min_g, max_g+1);
       if (!emp::Has(set, candidate_thresh)) {
         thresholds.emplace_back(candidate_thresh);
         set.emplace(candidate_thresh);
         break;
       }
      }
    }
    emp::Sort(thresholds);
    test[0] = thresholds[3];
    test[1] = thresholds[2];
    test[2] = thresholds[1];
    test[3] = thresholds[0];
    test[4] = rnd.GetInt(min_g, max_g+1);
  }
  
  // todo - check that neg num comes out properly
  bool Evaluate(GRADE_TYPE out) {
    GRADE_TYPE letter;
    if (GetStuGrade() >= GetAThresh()) letter = GRADE_TYPE::A;
    else if (GetStuGrade() >= GetBThresh()) letter = GRADE_TYPE::B;
    else if (GetStuGrade() >= GetCThresh()) letter = GRADE_TYPE::C;
    else if (GetStuGrade() >= GetDThresh()) letter = GRADE_TYPE::D;
    else letter = GRADE_TYPE::F;
    return out == letter;
  }

  bool Validate(emp::Random & rnd, 
                int min_n=DEFAULT_MIN_GRADE_VAL, int max_n=DEFAULT_MAX_GRADE_VAL) {  
    emp::vector<int> thresholds({test[0], test[1], test[2], test[3]});
    emp::Sort(thresholds);
    if (test[0] != thresholds[3] || test[1] != thresholds[2] || test[2] != thresholds[1] || test[3] != thresholds[0]) return false;
    std::unordered_set<int> set;
    for (size_t i = 0; i < thresholds.size(); ++i) {
      set.emplace(thresholds[i]);
    }
    if (set.size() != thresholds.size()) return false;
    for (size_t i = 0; i < test.size(); ++i) {
      if (test[i] < min_n || test[i] > max_n) return false;
    }
    return true;
  }

  void Print(std::ostream & out=std::cout) const {
    out << "[";
    for (size_t i = 0; i < test.size(); ++i) {
      if (i) out << ",";
      out << test[i];
    }
    out << "]";
  }
};

/// Test case (27): Median
/// - Description: Given 3 integers, print their median.
class MedianTest {
public:
  using test_t = emp::array<int, 3>;

  static constexpr int DEFAULT_MIN_VAL = -100;
  static constexpr int DEFAULT_MAX_VAL = 100;

protected:
  test_t test;

public:
  MedianTest()
    : test({DEFAULT_MIN_VAL, 
            DEFAULT_MIN_VAL, 
            DEFAULT_MIN_VAL}) { ; }
  
  MedianTest(emp::Random & rnd, 
            int min_n=DEFAULT_MIN_VAL, int max_n=DEFAULT_MAX_VAL)
    : test({0,0,0})
  { RandomizeTest(rnd, min_n, max_n); }

  MedianTest(MedianTest &&) = default;
  MedianTest(const MedianTest &) = default;

  MedianTest & operator=(const MedianTest &) = default;
  MedianTest & operator=(MedianTest &&) = default;

  bool operator==(const MedianTest & in) const { return in.test == test; }
  bool operator!=(const MedianTest & in) const { return !(in == *this); }
  bool operator<(const MedianTest & in) const { return test < in.test; }

  void RandomizeTest(emp::Random & rnd, 
                     int min_n=DEFAULT_MIN_VAL, int max_n=DEFAULT_MAX_VAL) {
    for (size_t i = 0; i < test.size(); ++i) test[i] = rnd.GetInt(min_n, max_n+1);
  }
  
  bool Evaluate(int out) {
    emp::vector<int> vals(test);
    emp::Sort(vals);
    return out == vals[1];
  }

  bool Validate(emp::Random & rnd, 
                int min_n=DEFAULT_MIN_VAL, int max_n=DEFAULT_MAX_VAL) {  
    for (size_t i = 0; i < test.size(); ++i) {
      if (test[i] > max_n || test[i] < min_n) return false;
    }
    return true;
  }

  void Print(std::ostream & out=std::cout) const {
    out << "[";
    for (size_t i = 0; i < test.size(); ++i) {
      if (i) out << ",";
      out << test[i];
    }
    out << "]";
  }
};

/// Test case (28): Smallest
/// - Description: Given 4 integers, print the smallest of them.
class SmallestTest {
public:
  using test_t = emp::array<int, 4>;

  static constexpr int DEFAULT_MIN_VAL = -100;
  static constexpr int DEFAULT_MAX_VAL = 100;

protected:
  test_t test;

public:
  SmallestTest()
    : test({DEFAULT_MIN_VAL, 
            DEFAULT_MIN_VAL, 
            DEFAULT_MIN_VAL}) { ; }
  
  SmallestTest(emp::Random & rnd, 
            int min_n=DEFAULT_MIN_VAL, int max_n=DEFAULT_MAX_VAL)
    : test({0,0,0})
  { RandomizeTest(rnd, min_n, max_n); }

  SmallestTest(SmallestTest &&) = default;
  SmallestTest(const SmallestTest &) = default;

  SmallestTest & operator=(const SmallestTest &) = default;
  SmallestTest & operator=(SmallestTest &&) = default;

  bool operator==(const SmallestTest & in) const { return in.test == test; }
  bool operator!=(const SmallestTest & in) const { return !(in == *this); }
  bool operator<(const SmallestTest & in) const { return test < in.test; }

  void RandomizeTest(emp::Random & rnd, 
                     int min_n=DEFAULT_MIN_VAL, int max_n=DEFAULT_MAX_VAL) {
    for (size_t i = 0; i < test.size(); ++i) test[i] = rnd.GetInt(min_n, max_n+1);
  }
  
  bool Evaluate(int out) {
    return out == *std::min_element(test.begin(), test.end());
  }

  bool Validate(emp::Random & rnd, 
                int min_n=DEFAULT_MIN_VAL, int max_n=DEFAULT_MAX_VAL) {  
    for (size_t i = 0; i < test.size(); ++i) {
      if (test[i] > max_n || test[i] < min_n) return false;
    }
    return true;
  }

  void Print(std::ostream & out=std::cout) const {
    out << "[";
    for (size_t i = 0; i < test.size(); ++i) {
      if (i) out << ",";
      out << test[i];
    }
    out << "]";
  }
};

#endif
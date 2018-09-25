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

#include "base/array.h"
#include "base/vector.h"
#include "tools/Random.h"

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

/// Test case (12): Last Index of Zero

/// Test case (13): Vector average

/// Test case (14): Count odds

/// Test case (15): Mirror image

/// Test case (17): Sum of Squares

/// Test case (18): Vectors summed

/// Test case (21): Negative to zero

/// Test case (25): Digits

/// Test case (26): Grade

/// Test case (27): Median

/// Test case (28): Smallest

#endif
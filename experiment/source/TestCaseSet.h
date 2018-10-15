/**
*   @note The original version of this class was written by Emily Dolson and can be found here: https://github.com/emilydolson/map-elites-gp/blob/master/source/TestcaseSet.h
*/

#ifndef TEST_CASE_SET_H
#define TEST_CASE_SET_H

#include <iostream>
#include <fstream>
#include <set>
#include <algorithm>

#include "base/array.h"
#include "base/vector.h"
#include "tools/string_utils.h"
#include "tools/Random.h"
#include "tools/random_utils.h"

template <typename INPUT_TYPE, typename OUTPUT_TYPE>
class TestCaseSet {
protected:
    using input_t = INPUT_TYPE;
    using output_t = OUTPUT_TYPE;
    using test_case_t = std::pair<input_t, output_t>;

    using load_test_case_fun_t = std::function<test_case_t(const std::string &)>;
    
    emp::vector<test_case_t> test_cases;

    load_test_case_fun_t fun_load_test_case;

public:
    TestCaseSet(const load_test_case_fun_t & load_fun, const std::string & filename) {
        fun_load_test_case = load_fun;
        LoadTestCases(filename);
    }

    TestCaseSet(const load_test_case_fun_t & load_fun) {
        fun_load_test_case = load_fun;
    }

    TestCaseSet() {
        fun_load_test_case = [](const std::string &) -> std::pair<input_t, output_t> { return {input_t(), output_t()}; };
    }

    test_case_t & operator[](size_t id) { return test_cases[id]; }
    const test_case_t & operator[](size_t id) const { return test_cases[id]; }
    
    size_t GetSize() const { return test_cases.size(); }
    
    /// Get input for testcase given by testID.
    input_t & GetInput(size_t testID) {
        emp_assert(testID < test_cases.size());
        return test_cases[testID].first;
    }

    /// Get output for test case given by testID.
    output_t & GetOutput(size_t testID) {
        emp_assert(testID < test_cases.size());
        return test_cases[testID].second;
    }

    /// Get test case set
    emp::vector<test_case_t> & GetTestCaseSet() { return test_cases; }

    void SetLoadFun(const load_test_case_fun_t & load_fun) { fun_load_test_case = load_fun; }
    
    void LoadTestCases(std::string filename) {
        std::ifstream infile(filename);
        std::string line;
        if (!infile.is_open()) {
            std::cout << "ERROR: " << filename << " did not open correctly." << std::endl;
            return;
        }
        // Ignore header
        getline(infile, line);
        while (getline(infile, line)) {
            test_cases.emplace_back(fun_load_test_case(line));
        }
        infile.close();
    }

    bool EvaluateOnTest(size_t testID, const output_t & out) {
        emp_assert(testID < test_cases.size());
        return out == GetOutput(testID);
    }

};

#endif

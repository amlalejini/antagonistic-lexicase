#ifndef SORTING_TEST_ORG_H
#define SORTING_TEST_ORG_H

#include "SortingTest.h"

class SortingTestOrg {
public:

  struct Genome {
    emp::vector<SortingTest> test_set;
    size_t test_size;
  };

  using genome_t = Genome;

protected:

  genome_t genome;

public:

  SortingTestOrg(size_t test_size, size_t num_tests=1) 
    : genome()
  { 
    genome.test_size = test_size; 
    genome.test_set.resize(num_tests, SortingTest(test_size));
  } 

  SortingTestOrg(emp::Random & rnd, size_t test_size, size_t num_tests=1)
    : genome()
  {
    genome.test_size = test_size;
    genome.test_set.resize(num_tests, SortingTest(rnd, test_size));
  }
  
  SortingTestOrg(const genome_t & _g) : genome(_g) { ; }

  size_t GetNumTests() const { return genome.test_set.size(); }
  size_t GetTestSize() const { return genome.test_size; }
  emp::vector<SortingTest> & GetTestSet() { return genome.test_set; }

  genome_t & GetGenome() { return genome; }
  
};

#endif
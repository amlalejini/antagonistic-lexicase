#ifndef SORTING_TEST_ORG_H
#define SORTING_TEST_ORG_H

#include "SortingTest.h"

class SortingTestOrg {
public:

  struct Genome {
    emp::vector<SortingTest> test_set;
    size_t test_size;

    Genome(size_t test_size, size_t num_tests=1) 
      : test_set(num_tests, test_size), test_size(test_size) { ; }

    Genome(emp::Random & rnd, size_t test_size, size_t num_tests=1)
      : test_set(num_tests, test_size), test_size(test_size) 
    {
        for (size_t i = 0; i < test_set.size(); ++i) test_set[i].RandomizeTest(rnd);
    }

    Genome(Genome &&) = default;
    Genome(const Genome &) = default;
    
    Genome & operator=(const Genome &) = default;
    Genome & operator=(Genome &&) = default;

    bool operator==(const Genome & in) const { return in.test_set == test_set; }
    bool operator!=(const Genome & in) const { return !(in == *this); }
    bool operator<(const Genome & in) const { return test_set < in.test_set; }

  };

  using genome_t = Genome;

protected:

  genome_t genome;

public:

  SortingTestOrg(size_t test_size, size_t num_tests=1) 
    : genome(test_size, num_tests)
  { ;  } 

  SortingTestOrg(emp::Random & rnd, size_t test_size, size_t num_tests=1)
    : genome(rnd, test_size, num_tests)
  { ; }
  
  SortingTestOrg(const genome_t & _g) : genome(_g) { ; }

  size_t GetNumTests() const { return genome.test_set.size(); }
  size_t GetTestSize() const { return genome.test_size; }
  emp::vector<SortingTest> & GetTestSet() { return genome.test_set; }

  genome_t & GetGenome() { return genome; }
  
  void Print(std::ostream & out=std::cout) const;

};

void SortingTestOrg::Print(std::ostream & out) const {
  std::cout << "TestOrg(seqsize=" << GetTestSize() << "," << "numtests=" << GetNumTests() << "):\n";
  for (size_t i = 0; i < GetNumTests(); ++i) {
    std::cout << "  Test[" << i << "]: "; 
    genome.test_set[i].Print();
    std::cout << "\n";
  }
}

#endif
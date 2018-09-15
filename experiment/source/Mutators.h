#ifndef ALEX_MUTATORS_H
#define ALEX_MUTATORS_H

#include "base/vector.h"
#include "tools/Random.h"
#include "tools/random_utils.h"

#include "SortingNetworkOrg.h"
#include "SortingTestOrg.h"
#include "SortingTest.h"

// For now, all mutation types are integrated into Mutate function.
// To make cleaner, could break each out into their own function.
struct SortingNetworkMutator {
  using genome_t = SortingNetworkOrg::genome_t;

  size_t MAX_NETWORK_SIZE;  ///< Maximum size network can grow
  size_t MIN_NETWORK_SIZE;  ///< Minimum size network can shrink
  size_t SORT_SEQ_SIZE;     ///< Sort input size (defines range for i,j values)

  double PER_INDEX_SUB;
  double PER_PAIR_DUP;
  double PER_PAIR_INS;
  double PER_PAIR_DEL;
  double PER_PAIR_SWAP;

  SortingNetworkMutator() 
    : MAX_NETWORK_SIZE(64),
      MIN_NETWORK_SIZE(1),
      SORT_SEQ_SIZE(4),
      PER_INDEX_SUB(0.001),
      PER_PAIR_DUP(0.001),
      PER_PAIR_INS(0.001),
      PER_PAIR_DEL(0.001),
      PER_PAIR_SWAP(0.001)
  { ; }

  size_t Mutate(emp::Random & rnd, genome_t & genome) {
    using gene_t = emp::array<size_t, 2>;

    genome_t new_genome;
    size_t mut_cnt = 0;

    size_t expected_size = genome.GetSize();

    // For gene (compare-exchange operation) in genome:
    // - copy gene to new genome, applying mutations
    for (size_t geneID = 0; geneID < genome.GetSize(); ++geneID) {
      gene_t & gene = genome[geneID];  // Gene we're copying

      const size_t rhead = new_genome.GetSize(); // Where in the new genome are we copying to?

      // Do we delete?
      if (rnd.P(PER_PAIR_DEL) && (expected_size > MIN_NETWORK_SIZE)) { 
        --expected_size;
        ++mut_cnt; 
        continue; 
      }
      new_genome.GetNetwork().emplace_back(gene);
      gene_t & gene_copy = new_genome.GetNetwork()[rhead];

      // Do we insert?
      if (rnd.P(PER_PAIR_INS) && (expected_size < MAX_NETWORK_SIZE)) {
        new_genome.GetNetwork().emplace_back(gene_t{rnd.GetUInt(0, SORT_SEQ_SIZE), rnd.GetUInt(0, SORT_SEQ_SIZE)});
        ++expected_size;
        ++mut_cnt;
      }

      // Do we duplicate?
      if (rnd.P(PER_PAIR_DUP) && (expected_size < MAX_NETWORK_SIZE)) {
        new_genome.GetNetwork().emplace_back(gene);
        ++expected_size;
        ++mut_cnt;
      }
      
      // Do index changes?
      if (rnd.P(PER_INDEX_SUB)) {
        gene_copy[0] = rnd.GetUInt(0, SORT_SEQ_SIZE);
        ++mut_cnt;
      }

      if (rnd.P(PER_INDEX_SUB)) {
        gene_copy[1] = rnd.GetUInt(0, SORT_SEQ_SIZE);
        ++mut_cnt;
      }
    }

    // Do swaps?
    if (PER_PAIR_SWAP > 0.0) {
      for (size_t geneID = 0; geneID < new_genome.GetSize(); ++geneID) {
        if (rnd.P(PER_PAIR_SWAP)) {
          // Select two random positions
          const size_t pos = rnd.GetUInt(new_genome.GetSize());
          if (pos == geneID) continue;
          std::swap(new_genome.GetNetwork()[geneID], new_genome.GetNetwork()[pos]);
          ++mut_cnt;
        }
      }
    }

    genome = new_genome;
    return mut_cnt;
  }

};

struct SortingTestMutator {
  using genome_t = SortingTestOrg::genome_t;

  bool bit_mode;

  int MAX_VALUE;
  int MIN_VALUE;
  
  double PER_SITE_SUB;
  double PER_SEQ_INVERSION;

  SortingTestMutator() 
    : bit_mode(true),
      MAX_VALUE(1),
      MIN_VALUE(0),
      PER_SITE_SUB(0.001),
      PER_SEQ_INVERSION(0.01)
  { ; }

  size_t Mutate(emp::Random & rnd, genome_t & genome) {
    size_t mut_cnt = 0;
    
    // For each sorting test in test set.
    for (size_t testID = 0; testID < genome.test_set.size(); ++testID) {
      SortingTest & test = genome.test_set[testID];
      // For each site in test
      for (size_t i = 0; i < test.GetSize(); ++i) {
        if (rnd.P(PER_SITE_SUB)) {
          if (bit_mode) test[i] = (int)!((bool)test[i]);
          else test[i] = rnd.GetInt(MIN_VALUE, MAX_VALUE+1);
          ++mut_cnt;
        }
      }
      // Inversions?
      if (rnd.P(PER_SEQ_INVERSION)) {
        int p0 = (int)rnd.GetUInt(0, test.GetSize());
        int p1 = (int)rnd.GetUInt(0, test.GetSize());
        if (p1 < p0) std::swap(p0, p1);
        emp_assert(p0 <= p1);
        while (p0 < p1) {
          std::swap(test[p0], test[p1]);
          ++p0; --p1;
        }
      }

    }
    return mut_cnt;
  }

};

#endif
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

      // Do we delete?
      if (rnd.P(PER_PAIR_DEL) && (expected_size > MIN_NETWORK_SIZE)) { 
        --expected_size;
        ++mut_cnt; 
        continue; 
      }
      const size_t rhead = new_genome.GetSize(); // Where in the new genome are we copying to?
      new_genome.GetNetwork().emplace_back(gene_t{genome[geneID][0], genome[geneID][1]});
      // gene_t & gene_copy = new_genome.GetNetwork()[rhead];

      // Do we insert?
      if (rnd.P(PER_PAIR_INS) && (expected_size < MAX_NETWORK_SIZE)) {
        new_genome.GetNetwork().emplace_back(gene_t{rnd.GetUInt(0, SORT_SEQ_SIZE), rnd.GetUInt(0, SORT_SEQ_SIZE)});
        ++expected_size;
        ++mut_cnt;
      }

      // Do we duplicate?
      if (rnd.P(PER_PAIR_DUP) && (expected_size < MAX_NETWORK_SIZE)) {
        new_genome.GetNetwork().emplace_back(gene_t{genome[geneID][0], genome[geneID][1]});
        ++expected_size;
        ++mut_cnt;
      }
      
      // Do index changes?
      if (rnd.P(PER_INDEX_SUB)) {
        new_genome.GetNetwork()[rhead][0] = rnd.GetUInt(0, SORT_SEQ_SIZE);
        ++mut_cnt;
      }

      if (rnd.P(PER_INDEX_SUB)) {
        new_genome.GetNetwork()[rhead][1] = rnd.GetUInt(0, SORT_SEQ_SIZE);
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

  /// Given genome A and B, crossover.
  /// If (Valid(AB)) A = AB; If(Valid(BA)) B = BA;
  void Crossover1Pt(emp::Random & rnd, genome_t & genomeA, genome_t & genomeB) {
    const size_t min_size = emp::Min(genomeA.GetSize(), genomeB.GetSize());
    const size_t max_size = emp::Max(genomeA.GetSize(), genomeB.GetSize());
    const size_t xpoint = rnd.GetUInt(min_size);
    genome_t genomeAB;
    genome_t genomeBA;

    for (size_t r = 0; r < xpoint; ++r) {
      genomeAB.GetNetwork().emplace_back(genomeA[r]);
      genomeBA.GetNetwork().emplace_back(genomeB[r]);
    }

    for (size_t r = xpoint; r < max_size; ++r) {
      if (r < genomeA.GetSize()) { genomeBA.GetNetwork().emplace_back(genomeA[r]); } 
      if (r < genomeB.GetSize()) { genomeAB.GetNetwork().emplace_back(genomeB[r]); }
    }

    genomeA = genomeAB;
    genomeB = genomeBA;
  }

  /// Given genome A and B, crossover.
  /// If (Valid(ABA)) A = ABA; If(Valid(BAB)) B = BAB;
  void Crossover2Pt(emp::Random & rnd, genome_t & genomeA, genome_t & genomeB) {
    // std::cout << "Genome A: "; genomeA.Print(); std::cout << std::endl;
    // std::cout << "Genome B: "; genomeB.Print(); std::cout << std::endl;

    double pct1 = rnd.GetDouble();
    double pct2 = rnd.GetDouble();
    if (pct2 < pct1) std::swap(pct1, pct2); // pct1 < pct2

    size_t pos1A = (size_t)genomeA.GetSize() * pct1;
    size_t pos1B = (size_t)genomeB.GetSize() * pct1;

    size_t pos2A = (size_t)genomeA.GetSize() * pct2;
    size_t pos2B = (size_t)genomeB.GetSize() * pct2;

    size_t ABA_size = pos1A + (pos2B - pos1B) + (genomeA.GetSize() - pos2A);
    size_t BAB_size = pos1B + (pos2A - pos1A) + (genomeB.GetSize() - pos2B);

    // std::cout << "pct1=" << pct1 << std::endl;
    // std::cout << "pct2=" << pct2 << std::endl;

    // std::cout << "pA = [" << pos1A << "," << pos2A << "]" << std::endl;
    // std::cout << "pB = [" << pos1B << "," << pos2B << "]" << std::endl;

    // std::cout << "ABA size = " << ABA_size << std::endl;
    // std::cout << "BAB size = " << BAB_size << std::endl;

    // ABA: |---A---|---B---|---A---|
    // BAB: |---B---|---A---|---B---|
    const bool build_aba = ABA_size <= MAX_NETWORK_SIZE && ABA_size >= MIN_NETWORK_SIZE;
    const bool build_bab = BAB_size <= MAX_NETWORK_SIZE && BAB_size >= MIN_NETWORK_SIZE;
    genome_t genomeABA;
    genome_t genomeBAB;

    if (build_aba) {
      // std::cout << "Buildinb ABA" << std::endl;    
      // Build ABA
      for (size_t r = 0; r < pos1A; ++r) { 
        // std::cout << "  Copying A[" << r << "]: " << genomeA[r][0] << "=>" << genomeA[r][1] << std::endl; 
        genomeABA.GetNetwork().emplace_back(genomeA[r]); 
      }
      for (size_t r = pos1B; r < pos2B; ++r) { 
        // std::cout << "  Copying B[" << r << "]: " << genomeB[r][0] << "=>" << genomeB[r][1] << std::endl;
        genomeABA.GetNetwork().emplace_back(genomeB[r]); 
      }
      for (size_t r = pos2A; r < genomeA.GetSize(); ++r) {
        // std::cout << "  Copying A[" << r << "]: " << genomeA[r][0] << "=>" << genomeA[r][1] << std::endl; 
        genomeABA.GetNetwork().emplace_back(genomeA[r]); 
      }
      emp_assert(genomeABA.GetSize() <= MAX_NETWORK_SIZE && genomeABA.GetSize() >= MIN_NETWORK_SIZE, ABA_size, genomeABA.GetSize(), pos1A, pos1B, pos2A, pos2B);
    }

    if (build_bab) {
      // std::cout << "Building BAB" << std::endl;
      // Build BAB
      for (size_t r = 0; r < pos1B; ++r) { 
        // std::cout << "  Copying B[" << r << "]: " << genomeB[r][0] << "=>" << genomeB[r][1] << std::endl;
        genomeBAB.GetNetwork().emplace_back(genomeB[r]); 
      }
      for (size_t r = pos1A; r < pos2A; ++r) { 
        // std::cout << "  Copying A[" << r << "]: " << genomeA[r][0] << "=>" << genomeA[r][1] << std::endl; 
        genomeBAB.GetNetwork().emplace_back(genomeA[r]); 
      }
      for (size_t r = pos2B; r < genomeB.GetSize(); ++r) {
        // std::cout << "  Copying B[" << r << "]: " << genomeB[r][0] << "=>" << genomeB[r][1] << std::endl;
        genomeBAB.GetNetwork().emplace_back(genomeB[r]); 
      }
      emp_assert(genomeBAB.GetSize() <= MAX_NETWORK_SIZE && genomeBAB.GetSize() >= MIN_NETWORK_SIZE, BAB_size, genomeBAB.GetSize(), pos1A, pos1B, pos2A, pos2B);
    }

    if (build_aba) genomeA = genomeABA;
    if (build_bab) genomeB = genomeBAB;

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
#ifndef PROG_ORG_H
#define PROG_ORG_H

#include "base/array.h"
#include "base/vector.h"
#include "tools/Random.h"

#include "TagLinearGP.h"

template<size_t TAG_WIDTH>
class ProgOrg {
public:
  struct Phenotype;

  using phenotype_t = Phenotype;
  using genome_t = typename TagLGP::TagLinearGP_TW<TAG_WIDTH>::Program;

  struct Phenotype {
    emp::vector<double> test_results;
    size_t num_passes;

    void Reset(size_t s=0) {
      test_results.clear();
      test_results.resize(s, 0);
      num_passes = 0;
    }
  };

protected:

  phenotype_t phenotype;
  genome_t genome;

public:

  ProgOrg(const genome_t & _g) 
    : genome(_g), phenotype() { ; }

  ProgOrg(const ProgOrg &) = default;
  ProgOrg(ProgOrg &&) = default;

  genome_t & GetGenome() { return genome; }
  const genome_t & GetGenome() const { return genome; }

  phenotype_t & GetPhenotype() { return phenotype; }
  const phenotype_t & GetPhenotype() const { return phenotype; }

  void SetGenome(const genome_t & in) { genome = in; }

  // Todo: randomize genome

  // Todo: print linear (print such that program can be put on single line)
};

#endif
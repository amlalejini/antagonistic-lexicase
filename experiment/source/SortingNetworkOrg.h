#ifndef SORTING_NETWORK_ORG_H
#define SORTING_NETWORK_ORG_H

#include "base/array.h"
#include "base/vector.h"
#include "tools/Random.h"

#include "SortingNetwork.h"

class SortingNetworkOrg {
public:
  using genome_t = SortingNetwork;

protected:

  genome_t genome;

public:
  
  SortingNetworkOrg(emp::Random & rnd, size_t input_size, size_t min_network_size, size_t max_network_size)
    : genome()
  { RandomizeGenome(rnd, input_size, min_network_size, max_network_size); }

  SortingNetworkOrg(const genome_t & _g)
    : genome(_g)
  { ; }

  size_t GetSize() const { return genome.GetSize(); }

  genome_t & GetGenome() { return genome; }
  
  void SetGenome(const genome_t & in) { genome = in; }

  void RandomizeGenome(emp::Random & rnd, size_t input_size, size_t network_size) { genome.RandomizeNetwork(rnd, input_size, network_size); };
  void RandomizeGenome(emp::Random & rnd, size_t input_size, size_t min_network_size, size_t max_network_size) { genome.RandomizeNetwork(rnd, input_size, min_network_size, max_network_size); };

  void Print(std::ostream & out=std::cout) const;

};

void SortingNetworkOrg::Print(std::ostream & out) const {
  out << "Network (size=" << genome.GetSize() << "): ";
  genome.Print(out);
}

#endif
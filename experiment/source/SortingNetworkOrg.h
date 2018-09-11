#ifndef SORTING_NETWORK_ORG_H
#define SORTING_NETWORK_ORG_H

#include "base/array.h"
#include "base/vector.h"
#include "tools/Random.h"

class SortingNetworkOrg {
public:
  using genome_t = emp::vector<emp::array<size_t, 2>>;

protected:

  genome_t genome;

public:
  
  SortingNetworkOrg(emp::Random & rnd, size_t input_size, size_t min_network_size, size_t max_network_size)
    : genome()
  { RandomizeGenome(rnd, input_size, min_network_size, max_network_size); }

  SortingNetworkOrg(const genome_t & _g)
    : genome(_g)
  { ; }

  size_t GetSize() const { return genome.size(); }

  genome_t & GetGenome() { return genome; }
  
  void SetGenome(const genome_t & in) { genome = in; }

  void RandomizeGenome(emp::Random & rnd, size_t input_size, size_t network_size);
  void RandomizeGenome(emp::Random & rnd, size_t input_size, size_t min_network_size, size_t max_network_size);

  bool ValidateNetwork(size_t input_size, size_t min_network_size=0, size_t max_network_size=(size_t)-1) const;

  void Print(std::ostream & out=std::cout) const;
  void PrintVert(std::ostream & out=std::cout) const;

};

void SortingNetworkOrg::RandomizeGenome(emp::Random & rnd, size_t input_size, 
                                        size_t network_size) {
  genome.resize(network_size);
  for (size_t i = 0; i < genome.size(); ++i) {
    genome[i][0] = rnd.GetUInt(0, input_size);
    genome[i][1] = rnd.GetUInt(0, input_size);
  }
}

void SortingNetworkOrg::RandomizeGenome(emp::Random & rnd, size_t input_size,
                                        size_t min_network_size, size_t max_network_size) {
  const size_t s = rnd.GetUInt(min_network_size, max_network_size+1);
  RandomizeGenome(rnd, input_size, s);
}

bool SortingNetworkOrg::ValidateNetwork(size_t input_size, size_t min_network_size, 
                                        size_t max_network_size) const {
  if (GetSize() > max_network_size || GetSize() < min_network_size) return false;
  for (size_t i = 0; i < genome.size(); ++i) {
    if (genome[i][0] >= input_size || genome[i][1] >= input_size) return false;
  }
  return true;
}

void SortingNetworkOrg::Print(std::ostream & out) const {
  out << "[";
  for (size_t i=0; i < genome.size(); ++i) {
    if (i) out << ", ";
    out << "(" << genome[i][0] << "=>" << genome[i][1] << ")";
  }
  out << "]";
}

void SortingNetworkOrg::PrintVert(std::ostream & out) const {
  for (size_t i=0; i < genome.size(); ++i) {
    if (i) out << "\n";
    out << genome[i][0] << " => " << genome[i][1];
  }
} 

#endif
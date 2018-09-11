#ifndef SORTING_NETWORK_ORG_H
#define SORTING_NETWORK_ORG_H

#include "base/array.h"
#include "base/vector.h"

class SortingNetworkOrg {
public:

protected:
  size_t input_size;

public:

  SortingNetworkOrg(size_t _input_size=4) 
    : input_size(_input_size) { ; }

  size_t GetInputSize() const { return input_size; }

};

class SortingNetworkOrg_DE : public SortingNetworkOrg {
public:
  using genome_t = emp::vector<emp::array<size_t, 2>>;

protected:

  genome_t genome;

public:

  SortingNetworkOrg_DE(size_t _input_size=4)
    : SortingNetworkOrg(_input_size), genome() 
  { ; }

};

#endif
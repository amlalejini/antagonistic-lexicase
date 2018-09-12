#ifndef SORTING_NETWORK_CONFIG_H
#define SORTING_NETWORK_CONFIG_H

#include "config/config.h"

EMP_BUILD_CONFIG(SortingNetworkConfig, 
  GROUP(DEFAULT_GROUP, "General settings"),
  VALUE(SEED, int, 0, "Random number seed (-1 for based on time)"),
  VALUE(MAX_NETWORK_SIZE, size_t, 128, "Maximum size of a sorting network"),
  VALUE(MIN_NETWORK_SIZE, size_t, 1, "Minimum size of a sorting network"),
  VALUE(TEST_SEQ_SIZE, size_t, 16, "Size of sequences being sorted by sorting networks"),
  VALUE(SEQ_PER_TEST, size_t, 1, "How many test sequences are there per test [org]?"),
  VALUE(NETWORK_POP_SIZE, size_t, 1000, "Population size for sorting networks"),
  VALUE(TEST_POP_SIZE, size_t, 1000, "Population size for sorting tests - how many tests exist at once?")
)

#endif
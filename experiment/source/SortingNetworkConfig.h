#ifndef SORTING_NETWORK_CONFIG_H
#define SORTING_NETWORK_CONFIG_H

#include "config/config.h"

EMP_BUILD_CONFIG(SortingNetworkConfig, 
  GROUP(DEFAULT_GROUP, "General settings"),
  VALUE(SEED, int, 0, "Random number seed (-1 for based on time)"),
  VALUE(GENERATIONS, size_t, 10000, "How many generations should we run for?"),
  VALUE(NETWORK_POP_SIZE, size_t, 1000, "Population size for sorting networks"),
  VALUE(TEST_POP_SIZE, size_t, 1000, "Population size for sorting tests - how many tests exist at once?"),
  VALUE(TEST_MODE, size_t, 0, "How do we test sorting networks? \n0: co-evolution \n1: static \n2: random"),
  VALUE(COHORT_SIZE, size_t, 10, "How big should random cohorts be (only relevant when evaluating with random cohort method)?"),
  
  GROUP(SELECTION, "Selection settings"),
  VALUE(SELECTION_MODE, size_t, 0, "Selection scheme: \n0: lexicase \n1: cohort-lexicase \n2: tournament"),
  VALUE(LEX_MAX_FUNS, size_t, 0, "What's the maximum number of fitness functions to use in lexicase select? 0 to use all"),
  VALUE(COHORTLEX_MAX_FUNS, size_t, 0, "Max number of fitness functions to use in cohort lexicase select. 0 to use all"),
  VALUE(TOURNAMENT_SIZE, size_t, 4, "Tournament size when using lexicase selection"),

  GROUP(SORTING_NETWORKS, "Sorting network settings"),
  VALUE(MAX_NETWORK_SIZE, size_t, 128, "Maximum size of a sorting network"),
  VALUE(MIN_NETWORK_SIZE, size_t, 1, "Minimum size of a sorting network"),

  GROUP(NETWORK_MUTATION, "Settings specific to mutating networks"),
  VALUE(PER_INDEX_SUB, double, 0.001, "Per-index substitution rate"),
  VALUE(PER_PAIR_DUP, double, 0.0005, "Per-operation operation duplication rate"),
  VALUE(PER_PAIR_INS, double, 0.0005, "Per-operation operation insertion rate"),
  VALUE(PER_PAIR_DEL, double, 0.001, "Per-operation operation deletion rate"),
  VALUE(PER_PAIR_SWAP, double, 0.001, "Per-operation operation swap rate"),
  
  GROUP(SORTING_TESTS, "Sorting test settings"),
  VALUE(SORT_SIZE, size_t, 16, "Size of sequences being sorted by sorting networks"),
  VALUE(SORTS_PER_TEST, size_t, 1, "How many test sequences are there per test [org]?"),

  GROUP(TEST_MUTATION, "Settings specific to mutating sorting tests."),
  VALUE(PER_SITE_SUB, double, 0.001, "Per-site substitution (bit flip) rate."),
  VALUE(PER_SEQ_INVERSION, double, 0.01, "Per-sequence inversion rate."),

  GROUP(DATA_COLLECTION, "Settings specific to data collection"),
  VALUE(DATA_DIRECTORY, std::string, "./output", "Where to dump experiment data files"),
  VALUE(SNAPSHOT_INTERVAL, size_t, 100, "Interval to take snapshots")



)

#endif
#ifndef PROGRAM_SYNTHESIS_CONFIG_H
#define PROGRAM_SYNTHESIS_CONFIG_H

#include "config/config.h"

EMP_BUILD_CONFIG(ProgramSynthesisConfig, 
  GROUP(DEFAULT_GROUP, "General settings"),
  VALUE(SEED, int, 0, "Random number seed (-1 for based on time)"),
  VALUE(GENERATIONS, size_t, 10000, "How many generations should we run for?"),
  
  VALUE(PROG_POP_SIZE, size_t, 1024, "Population size for programs"),
  VALUE(TEST_POP_SIZE, size_t, 1024, "Population size for tests - how many tests exist at once?"),

  VALUE(PROG_COHORT_SIZE, size_t, 32, "How big should random cohorts be (only relevant when evaluating with random cohort method)?"),
  VALUE(TEST_COHORT_SIZE, size_t, 32, "How big should random cohorts be (only relevant when evaluating with random cohort method)?"),

  VALUE(TEST_MODE, size_t, 0, "How do we test programs? \n0: co-evolution \n1: static \n2: random \n3: drift")
  
  
)

#endif
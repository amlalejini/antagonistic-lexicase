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
  VALUE(TRAINING_EXAMPLE_MODE, size_t, 0, "How do training examples change over time? \n0: co-evolution \n1: static \n2: random"),
  VALUE(PROBLEM, std::string, "number-io", "Which problem to use?"),
  VALUE(BENCHMARK_DATA_DIR, std::string, "../data/prog-synth-examples", "Location to look for problem test case data."),

  GROUP(PROB_NUMBER_IO_GROUP, "Settings specific to NumberIO problem."),
  VALUE(PROB_NUMBER_IO__DOUBLE_MIN, double, -100.0, "Min value for input double."),
  VALUE(PROB_NUMBER_IO__DOUBLE_MAX, double, 100.0, "Max value for input double."),
  VALUE(PROB_NUMBER_IO__INT_MIN, int, -100, "Min value for input int."),
  VALUE(PROB_NUMBER_IO__INT_MAX, int, 100, "Max value for input int."),
  



)

#endif
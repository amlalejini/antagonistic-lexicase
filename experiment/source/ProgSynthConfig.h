#ifndef PROGRAM_SYNTHESIS_CONFIG_H
#define PROGRAM_SYNTHESIS_CONFIG_H

#include "config/config.h"

EMP_BUILD_CONFIG(ProgramSynthesisConfig, 
  GROUP(DEFAULT_GROUP, "General settings"),
  VALUE(SEED, int, 0, "Random number seed (-1 for based on time)"),
  VALUE(GENERATIONS, size_t, 10000, "How many generations should we run for?"),
  VALUE(PROG_POP_SIZE, size_t, 1024, "Population size for programs"),
  VALUE(TEST_POP_SIZE, size_t, 1024, "Population size for tests - how many tests exist at once?"),
  VALUE(EVALUATION_MODE, size_t, 0, "How are programs evaluated? \n0: cohorts \n1: full (on all tests)"),
  VALUE(PROG_COHORT_SIZE, size_t, 32, "How big should random cohorts be (only relevant when evaluating with random cohort method)?"),
  VALUE(TEST_COHORT_SIZE, size_t, 32, "How big should random cohorts be (only relevant when evaluating with random cohort method)?"),
  VALUE(TRAINING_EXAMPLE_MODE, size_t, 0, "How do training examples change over time? \n0: co-evolution \n1: static \n2: random"),
  VALUE(PROBLEM, std::string, "number-io", "Which problem to use?"),
  VALUE(BENCHMARK_DATA_DIR, std::string, "../data/prog-synth-examples", "Location to look for problem test case data."),

  GROUP(PROGRAM_GROUP, "Settings specific to programs."),
  VALUE(MIN_PROG_SIZE, size_t, 1, "Minimum program size"),
  VALUE(MAX_PROG_SIZE, size_t, 128, "Maximum program size"),
  VALUE(PROG_EVAL_TIME, size_t, 256, "How many clock cycles should we give a program during a test?"),

  GROUP(HARDWARE_GROUP, "Settings specific to TagLGP virtual hardware"),
  VALUE(MIN_TAG_SPECIFICITY, double, 0.0, "What is the minimum tag similarity required for a tag to successfully reference another tag?"),
  VALUE(MAX_CALL_DEPTH, size_t, 128, "Maximum depth of hardware's call stack."),

  GROUP(PROB_NUMBER_IO_GROUP, "Settings specific to NumberIO problem."),
  VALUE(PROB_NUMBER_IO__DOUBLE_MIN, double, -100.0, "Min value for input double."),
  VALUE(PROB_NUMBER_IO__DOUBLE_MAX, double, 100.0, "Max value for input double."),
  VALUE(PROB_NUMBER_IO__INT_MIN, int, -100, "Min value for input int."),
  VALUE(PROB_NUMBER_IO__INT_MAX, int, 100, "Max value for input int."),

  GROUP(DATA_COLLECTION_GROUP, "Settings specific to data collection."),
  VALUE(SNAPSHOT_INTERVAL, size_t, 1000, "How often should we take population snapshots?"),
  VALUE(SOLUTION_SCREEN_INTERVAL, size_t, 1000, "How often should we screen entire population for solutions?")
  



)

#endif
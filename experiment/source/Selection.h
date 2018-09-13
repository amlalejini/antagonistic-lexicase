#ifndef ALEX_SELECTION_H
#define ALEX_SELECTION_H

#include <functional>

#include "base/assert.h"
#include "base/vector.h"
#include "tools/Random.h"
#include "tools/random_utils.h"
#include "tools/vector_utils.h"

namespace emp {
  template<typename ORG> class World;


  /// ==COHORT-LEXICASE== Selection runs through multiple fitness functions in a random order for
  /// EACH offspring produced. Only run select from population IDs specified by cohort vector.
  /// @param world The emp::World object with the organisms to be selected.
  /// @param fit_funs The set of fitness functions to shuffle for each organism reproduced.
  /// @param cohort The set of organism IDs (corresponding to world pop) to select from.
  /// @param repro_count How many rounds of repliction should we do. (default 1)
  /// @param max_funs The maximum number of fitness functions to use. (use 0 for all; default)
  template<typename ORG>
  void CohortLexicaseSelect(World<ORG> & world,
                            const emp::vector< std::function<double(ORG &)> > & fit_funs,
                            const emp::vector<size_t> & cohort,
                            size_t repro_count=1,
                            size_t max_funs=0) 
  {
    emp_assert(world.GetSize() > 0);
    emp_assert(fit_funs.size() > 0);
    emp_assert(cohort.size() > 0);

    if (!max_funs) max_funs = fit_funs.size();

    // Reminder: these index into cohort, cohort has proper worldID.
    emp::vector<size_t> all_orgs(cohort.size()), cur_orgs, next_orgs; 
    for (size_t i = 0; i < cohort.size(); ++i) all_orgs[i] = i;

    // Get fitnesses for each cohort member on all fitness functions.
    emp::vector< emp::vector<double> > fitnesses(fit_funs.size());
    for (size_t fit_id = 0; fit_id < fit_funs.size(); ++fit_id) {
      fitnesses[fit_id].resize(cohort.size());
      for (size_t i = 0; i < cohort.size(); ++i) {
        emp_assert(world.IsOccupied(cohort[i]));
        fitnesses[fit_id][i] = fit_funs[fit_id](world.GetOrg(cohort[i]));
      }
    }

    // Do selection!
    for (size_t repro = 0; repro < repro_count; ++repro) {
      // Get a random ordering of fitness functions.
      emp::vector<size_t> order(GetPermutation(world.GetRandom(), fit_funs.size()));
      if (max_funs < fit_funs.size()) { // If we don't use them all, toss some.
        order.resize(max_funs);
      }

      // Step through the functions in the proper order.
      cur_orgs = all_orgs; // Start with all of the organisms.
      int depth = -1;
      for (size_t fit_id : order) {
        ++depth;
        double max_fit = fitnesses[fit_id][cur_orgs[0]];
        next_orgs.emplace_back(cur_orgs[0]);
        for (size_t org_id = 1; org_id < cur_orgs.size(); ++org_id) {
          const double cur_fit = fitnesses[fit_id][org_id];
          if (cur_fit > max_fit) {
            max_fit = cur_fit;      // This is a NEW maximum fitness for this function.
            next_orgs.resize(1);    // Clear out orgs with former max fitness.
            next_orgs[0] = org_id;  // Add this org as the only one with the new max fitness.
          } else if (cur_fit == max_fit) {
            next_orgs.emplace_back(org_id); // Same as current max fitness, save this one too.
          }
        }
        // Make next_orgs into new cur_orgs; make cur_orgs allocated space for next_orgs.
        std::swap(cur_orgs, next_orgs);
        next_orgs.resize(0);
        if (cur_orgs.size() == 1) break; // Stop if we're down to just one organism.
      }

      // Place a random survivor (all equal) into the next generation.
      emp_assert(cur_orgs.size() > 0, cur_orgs.size(), fit_funs.size(), all_orgs.size());
      const size_t winner = world.GetRandom().GetUInt(cur_orgs.size());
      const size_t reproID = cohort[cur_orgs[winner]];
      world.DoBirth(world.GetGenomeAt(reproID), reproID);
    }
  }

}

#endif
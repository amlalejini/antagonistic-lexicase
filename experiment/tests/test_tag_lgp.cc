#define CATCH_CONFIG_MAIN
#include "third-party/Catch/single_include/catch.hpp"

#include <iostream>

#include "base/Ptr.h"
#include "base/vector.h"
#include "tools/Random.h"

#include "TagLinearGP.h"
#include "TagLinearGP_InstLib.h"
#include "TagLinearGP_Utilities.h"
#include "Utilities.h"

// String => number; number=>string conversion
//  - If str.numeric: return double(str)
//    else: return 0.0
// Number to string
//  return to_string(number)

TEST_CASE("InstLib", "[taglgp]") {

  constexpr size_t TAG_WIDTH = 32;
  constexpr int seed = 2;

  using hardware_t = TagLGP::TagLinearGP_TW<TAG_WIDTH>;
  using module_t = typename hardware_t::module_t;
  using program_t = typename hardware_t::program_t;
  using inst_t = typename hardware_t::inst_t;
  using inst_lib_t = TagLGP::InstLib<hardware_t>;
  using inst_prop_t = typename inst_lib_t::InstProperty;

  emp::Ptr<emp::Random> random = emp::NewPtr<emp::Random>(seed);

  emp::Ptr<inst_lib_t> inst_lib = emp::NewPtr<inst_lib_t>();
  hardware_t cpu(inst_lib, random);

  std::cout << "Empty instruction library:" << std::endl;
  inst_lib->Print(); std::cout << std::endl;

  inst_lib->AddInst("Inc", 
                    [](hardware_t &, const inst_t &) { ; }, 1, "Increment value in local memory Arg1");
  inst_lib->AddInst("Dec", [](hardware_t &, const inst_t &) { ; }, 1, "Decrement value in local memory Arg1");
  inst_lib->AddInst("While", [](hardware_t &, const inst_t &) { ; }, 1, "Local memory: If Arg1 != 0, loop; else, skip block.", {inst_prop_t::BEGIN_FLOW});
  inst_lib->AddInst("Call", [](hardware_t &, const inst_t &) { ; }, 0, "Call function that best matches call affinity.");
  inst_lib->AddInst("Def-Module", [](hardware_t &, const inst_t &) { ; }, 0, "Module definition", {inst_prop_t::MODULE});


  std::cout << "Empty instruction library:" << std::endl;
  inst_lib->Print(); std::cout << std::endl;

  // Generate a random program.
  program_t prog(inst_lib);
  for (size_t i = 0; i < 10; ++i) {
    prog.PushInst(TagLGP::GenRandTagGPInst(*random, *inst_lib));
  }
  std::cout << "Random Program:" << std::endl;
  prog.Print();
  std::cout << std::endl; 
  
  // Add program to CPU
  cpu.SetProgram(prog);

  // What do default modules look like?
  std::cout << "Modules (default): " << std::endl;
  cpu.PrintModules(); std::cout << std::endl;
  std::cout << "Module sequence: " << std::endl;
  cpu.PrintModuleSequences(); std::cout << std::endl;

  std::cout << std::endl;

  std::cout << "Global memory (default): " << std::endl;
  cpu.GetGlobalMem().Print(); std::cout << std::endl;

  std::cout << "Global set memory (default): " << std::endl;
  cpu.GetGlobalMem().PrintSetMem(); std::cout << std::endl;

  std::cout << "Default memory profile verbose: " << std::endl;
  cpu.PrintMemoryVerbose();
  std::cout << std::endl;

  cpu.GetGlobalMem().Set(0, "hello world");
  cpu.GetGlobalMem().Set(1, {"hello", "goodbye", "cruel", "world"});
  cpu.GetGlobalMem().Set(2, 16);
  cpu.GetGlobalMem().Set(3, emp::vector<double>{2, 4, 8, 16, 32, 64, 128});
  cpu.GetGlobalMem().Set(4, {42});
  std::cout << "Global memory (verbose) after some sets: " << std::endl;
  cpu.PrintMemoryVerbose();
  std::cout << std::endl;


  // 
  // std::function<emp::vector<module_t>(const program_t &)> get_modules = [](const program_t & p) {

  // }

}
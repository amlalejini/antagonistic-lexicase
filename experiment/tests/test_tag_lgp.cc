#define CATCH_CONFIG_MAIN
#include "third-party/Catch/single_include/catch.hpp"

#include <iostream>

#include "base/Ptr.h"
#include "base/vector.h"
#include "tools/Random.h"

#include "TagLinearGP.h"
#include "TagLinearGP_InstLib.h"
#include "TagLinearGP_Utilities.h"
#include "TagLinearGP_Instructions.h"
#include "Utilities.h"

// String => number; number=>string conversion
//  - If str.numeric: return double(str)
//    else: return 0.0
// Number to string
//  return to_string(number)

// TODO - one test per instruction type

TEST_CASE("InstLib", "[taglgp]") {

  constexpr size_t TAG_WIDTH = 4;
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

  // Configure CPU
  emp::vector<emp::BitSet<TAG_WIDTH>> matrix = GenHadamardMatrix<TAG_WIDTH>();
  cpu.SetMemSize(TAG_WIDTH);
  cpu.SetMemTags(matrix);

  std::cout << "Empty instruction library:" << std::endl;
  inst_lib->Print(); std::cout << std::endl;

  inst_lib->AddInst("Add", hardware_t::Inst_Add, 3, "");
  inst_lib->AddInst("Sub", hardware_t::Inst_Sub, 3, "");
  inst_lib->AddInst("Mult", hardware_t::Inst_Mult, 3, "");
  inst_lib->AddInst("Inc", hardware_t::Inst_Inc, 1, "");
  inst_lib->AddInst("Dec", hardware_t::Inst_Dec, 1, "");
  inst_lib->AddInst("Div", hardware_t::Inst_Div, 3, "");
  inst_lib->AddInst("Mod", hardware_t::Inst_Mod, 3, "");
  inst_lib->AddInst("TestNumEqu", hardware_t::Inst_TestNumEqu, 3, "");
  inst_lib->AddInst("TestNumNEqu", hardware_t::Inst_TestNumNEqu, 3, "");
  inst_lib->AddInst("TestNumLess", hardware_t::Inst_TestNumLess, 3, "");
  inst_lib->AddInst("Floor", hardware_t::Inst_Floor, 3, "");
  
  inst_lib->AddInst("CopyMem", hardware_t::Inst_CopyMem, 3, "");
  inst_lib->AddInst("SwapMem", hardware_t::Inst_SwapMem, 3, "");
  inst_lib->AddInst("Input", hardware_t::Inst_Input, 3, "");
  inst_lib->AddInst("Output", hardware_t::Inst_Output, 3, "");
  inst_lib->AddInst("CommitGlobal", hardware_t::Inst_CommitGlobal, 3, "");
  inst_lib->AddInst("PullGlobal", hardware_t::Inst_PullGlobal, 3, "");
  inst_lib->AddInst("TestMemEqu", hardware_t::Inst_TestMemEqu, 3, "");
  inst_lib->AddInst("TestMemNEqu", hardware_t::Inst_TestMemNEqu, 3, "");

  inst_lib->AddInst("MakeVector", hardware_t::Inst_MakeVector, 3, "");
  inst_lib->AddInst("VecGet", hardware_t::Inst_VecGet, 3, "");
  inst_lib->AddInst("VecSet", hardware_t::Inst_VecSet, 3, "");
  inst_lib->AddInst("VecLen", hardware_t::Inst_VecLen, 3, "");
  inst_lib->AddInst("VecAppend", hardware_t::Inst_VecAppend, 3, "");
  inst_lib->AddInst("VecPop", hardware_t::Inst_VecPop, 3, "");
  inst_lib->AddInst("VecRemove", hardware_t::Inst_VecRemove, 3, "");
  inst_lib->AddInst("VecReplaceAll", hardware_t::Inst_VecReplaceAll, 3, "");
  inst_lib->AddInst("VecIndexOf", hardware_t::Inst_VecIndexOf, 3, "");
  inst_lib->AddInst("VecOccurrencesOf", hardware_t::Inst_VecOccurrencesOf, 3, "");
  inst_lib->AddInst("VecReverse", hardware_t::Inst_VecReverse, 3, "");
  inst_lib->AddInst("VecSwapIfLess", hardware_t::Inst_VecSwapIfLess, 3, "");
  inst_lib->AddInst("VecGetFront", hardware_t::Inst_VecGetFront, 3, "");
  inst_lib->AddInst("VecGetBack", hardware_t::Inst_VecGetBack, 3, "");

  inst_lib->AddInst("IsStr", hardware_t::Inst_IsStr, 3, "");
  inst_lib->AddInst("IsNum", hardware_t::Inst_IsNum, 3, "");
  inst_lib->AddInst("IsVec", hardware_t::Inst_IsVec, 3, "");

  inst_lib->AddInst("If", hardware_t::Inst_If, 3, "");
  inst_lib->AddInst("While", hardware_t::Inst_While, 3, "");
  inst_lib->AddInst("Countdown", hardware_t::Inst_Countdown, 3, "");
  inst_lib->AddInst("Foreach", hardware_t::Inst_Foreach, 3, "");
  inst_lib->AddInst("Close", hardware_t::Inst_Close, 3, "");
  inst_lib->AddInst("Break", hardware_t::Inst_Break, 3, "");
  inst_lib->AddInst("Call", hardware_t::Inst_Call, 3, "");
  inst_lib->AddInst("Routine", hardware_t::Inst_Routine, 3, "");
  inst_lib->AddInst("Return", hardware_t::Inst_Return, 3, "");

  inst_lib->AddInst("Nop", hardware_t::Inst_Nop, 3, "");

  for (size_t i = 0; i <= 16; ++i) {
    inst_lib->AddInst("Set-" + emp::to_string(i),
      [i](hardware_t & hw, const inst_t & inst) {
        CallState & state = hw.GetCurCallState();
        memory_t & wmem = state.GetWorkingMem();
        
        size_t posA = hw.FindBestMemoryMatch(wmem, inst.arg_tags[0], hw.GetMinTagSpecificity());
        if (!hw.IsValidMemPos(posA)) return; // Do nothing

        wmem.Set(posA, (double)i);

      });
  }


  std::cout << "Populated instruction library:" << std::endl;
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

  std::cout << "========================= Initial hardware state =========================" << std::endl;
  cpu.CallModule(0);
  cpu.PrintHardwareState();
  std::cout << std::endl;

  for (size_t i = 0; i < 16; ++i) {
    std::cout << "================== RUNNING CYCLE " << i << " ==================" << std::endl;
    cpu.SingleProcess();
    cpu.PrintHardwareState();
  }

  // Run a bunch of randomly generated programs for 1024 updates
  for (size_t p = 0; p < 1024; ++p) {
    program_t prg(inst_lib);
    size_t N = random->GetUInt(512, 1024);
    for (size_t i = 0; i < N; ++i) {
      prg.PushInst(TagLGP::GenRandTagGPInst(*random, *inst_lib));
    } 
    cpu.SetProgram(prog);
    cpu.CallModule(0); 
    for (size_t i = 0; i < 1024; ++i) {
      cpu.SingleProcess();
    }
  }
}
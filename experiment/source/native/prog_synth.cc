// This is the main function for the NATIVE version of this project.

#include <iostream>
#include <string>

#include "base/vector.h"
#include "config/ArgManager.h"
#include "config/command_line.h"

#include "../ProgSynthExperiment.h"
#include "../ProgSynthConfig.h"

int main(int argc, char* argv[])
{
  std::string config_fname = "prog_synth_configs.cfg";
  ProgramSynthesisConfig config;
  auto args = emp::cl::ArgManager(argc, argv);
  config.Read(config_fname);
  if (args.ProcessConfigOptions(config, std::cout, config_fname, "ProgSynthConfig-macros.h") == false) exit(0);
  if (args.TestUnknown() == false) exit(0); // If there are leftover args, throw an error. 

  // Write to screen how the experiment is configured
  std::cout << "==============================" << std::endl;
  std::cout << "|    How am I configured?    |" << std::endl;
  std::cout << "==============================" << std::endl;
  config.Write(std::cout);
  std::cout << "==============================\n" << std::endl;

  ProgramSynthesisExperiment e;
  // e.Setup(config);
  // e.Run();

}

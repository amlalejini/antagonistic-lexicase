#define CATCH_CONFIG_MAIN
#include "third-party/Catch/single_include/catch.hpp"

#include <iostream>
#include <cmath>

#include "base/Ptr.h"
#include "base/vector.h"
#include "tools/Random.h"

#include "TagLinearGP.h"
#include "TagLinearGP_InstLib.h"
#include "TagLinearGP_Utilities.h"
#include "Utilities.h"
#include "Mutators.h"

#include "parser.hpp"


TEST_CASE("TagLGP_Mutator", "[taglgp]") {
  constexpr size_t TAG_WIDTH = 4;
  constexpr int seed = 4;

  using hardware_t = TagLGP::TagLinearGP_TW<TAG_WIDTH>;
  using program_t = typename hardware_t::program_t;
  // using inst_t = typename hardware_t::inst_t;
  using inst_lib_t = TagLGP::InstLib<hardware_t>;

  emp::Ptr<emp::Random> random = emp::NewPtr<emp::Random>(seed);

  emp::Ptr<inst_lib_t> inst_lib = emp::NewPtr<inst_lib_t>();
  hardware_t cpu(inst_lib, random);

  TagLGPMutator<TAG_WIDTH> mutator;
  

  // Configure CPU
  emp::vector<emp::BitSet<TAG_WIDTH>> matrix = GenHadamardMatrix<TAG_WIDTH>();
  cpu.SetMemSize(TAG_WIDTH);
  cpu.SetMemTags(matrix);

  inst_lib->AddInst("ModuleDef", hardware_t::Inst_Nop, 3, "", {inst_lib_t::InstProperty::MODULE});
  inst_lib->AddInst("Nop", hardware_t::Inst_Nop, 3, "");


  mutator.MAX_PROGRAM_LEN = 64;
  mutator.MIN_PROGRAM_LEN = 1;
  mutator.PER_BIT_FLIP = 0.5;
  mutator.PER_INST_SUB = 0.5;
  mutator.PER_INST_INS = 0.1;
  mutator.PER_INST_DEL = 0.1;
  mutator.PER_PROG_SLIP = 0.25;
  mutator.PER_MOD_DUP = 0.1;
  mutator.PER_MOD_DEL = 0.1;

  for (size_t i = 0; i < 10000; ++i) {    
    program_t prg(TagLGP::GenRandTagGPProgram(*random, inst_lib, 5, 10));
    mutator.Mutate(*random, prg);
    REQUIRE(prg.GetSize() <= mutator.MAX_PROGRAM_LEN);
    REQUIRE(prg.GetSize() >= mutator.MIN_PROGRAM_LEN);
  }
}

TEST_CASE("HammingDistance", "[utilities]") {

  constexpr size_t TAG_WIDTH = 4;

  emp::vector<emp::BitSet<TAG_WIDTH>> matrix = GenHadamardMatrix<TAG_WIDTH>();

  for (size_t i = 0; i < matrix.size(); ++i) {
    for (size_t k = i; k < matrix.size(); ++k) {
      std::cout << "======================" << std::endl;
      matrix[i].Print(); std::cout << std::endl;
      matrix[k].Print(); std::cout << std::endl;
      std::cout << " ------- " << std::endl;
      std::cout << HammingDist(matrix[i], matrix[k]) << std::endl;
    }
  }

  emp::BitSet<TAG_WIDTH> z;
  std::cout << HammingDist(matrix[0], z) << std::endl;


}

TEST_CASE("CSVReader", "[csv]") {

  std::string training_fpath = "../../data/prog-synth-examples/testing-examples-compare-string-lengths.csv";

  std::ifstream f(training_fpath);
  aria::csv::CsvParser parser(f);
  parser.delimiter(',');
  parser.quote('"');

  for (auto & row : parser) {
    std::cout << "------- ROW -------" << std::endl;
    emp::vector<std::string> fields;
    for (auto & field : row) {
      std::cout << "- FIELD: " << field << "\n";
      fields.emplace_back(field);
    }
    // Check if output field matches what we'd expect
    bool calc_out = false;
    if (fields[0].size() < fields[1].size() && fields[1].size() < fields[2].size()) {
      calc_out = true;
    }
    bool read_out = false;
    if (fields[3] == "true") read_out = true;
    else if (fields[3] == "false") read_out = false;
    else {
      std::cout << "!!!! Bad output value? !!!!" << std::endl;
    }
    std::cout << "read out (" << read_out << ") == calc out (" << calc_out << ") ? " << (read_out == calc_out) << std::endl;
    REQUIRE(read_out == calc_out);
    std::cout << std::endl;
  }

}
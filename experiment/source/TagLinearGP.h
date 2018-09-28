#ifndef TAG_LINEAR_GP_H
#define TAG_LINEAR_GP_H

#include <string>
#include <functional>
#include <unordered_set>

#include "base/vector.h"
#include "tools/BitSet.h"
#include "tools/map_utils.h"
#include "tools/Random.h"
#include "tools/string_utils.h"

#include "TagLinearGP_InstLib.h"

namespace TagLGP {
  /////////////
  // TODO:
  // - [ ] Work out how memory will be represented
  /////////////

  /// TagLinearGP class.
  ///  - Description: 'Simple' lgp designed to handle program synthesis benchmarking
  ///    problems (i.e., designed to facilitate *more* arbitrary data types in memory).
  ///  - Simplifying non-features:
  ///   - No threads
  ///   - No events
  template<size_t TAG_WIDTH>
  class TagLinearGP_TW {
  public:
    struct Module;
    struct Instruction;
    struct Flow;
    struct CallState;
    class MemoryPosition;
    class MemoryValue;
    struct Program;

    using hardware_t = TagLinearGP_TW<TAG_WIDTH>;
    using tag_t = emp::BitSet<TAG_WIDTH>;
    using memory_t = emp::vector<MemoryPosition>;
    using mem_pos_t = MemoryPosition;

    using module_t = Module;
    using inst_t = Instruction;
    using inst_lib_t = InstLib<hardware_t>;

    using program_t = Program;

    using fun_get_modules_t = std::function<emp::vector<module_t>(const program_t &)>;

    static constexpr size_t DEFAULT_MEM_SIZE = 16;
    static constexpr size_t DEFAULT_MAX_CALL_DEPTH = 128;
    static constexpr double DEFAULT_MIN_TAG_SPECIFICITY = 0.0;

    /// Struct - State
    /// - Description: maintains information about local call state.
    struct CallState {
      size_t mem_size;

      memory_t working_mem;
      memory_t input_mem;
      memory_t output_mem;

      size_t mptr;    ///< Module pointer.
      size_t iptr;    ///< Instruction pointer.

      emp::vector<Flow> flow_stack; ///< Stack of Flow (top is current).

      bool returnable;  ///< Can we return from this state? (or, are we trapped here forever!)
      bool circular;    ///< Does call have an implicit return at EOM? Or, is it circular?

      CallState(size_t _mem_size=128, bool _returnable=true, bool _circular=false,
                const MemoryPosition & _def_mem=MemoryPosition())
        : mem_size(_mem_size),
          working_mem(_mem_size, _def_mem),
          input_mem(_mem_size, _def_mem),
          output_mem(_mem_size, _def_mem),
          mptr(0), iptr(0),
          flow_stack(),
          returnable(_returnable),
          circular(_circular)
      { ; }

      CallState(const CallState &) = default;
      CallState(CallState &&) = default;

      void Reset(const MemoryPosition & def_mem=MemoryPosition()) {
        working_mem.clear();
        working_mem.resize(mem_size, def_mem);
        input_mem.clear();
        input_mem.resize(mem_size, def_mem);
        output_mem.clear();
        output_mem.resize(mem_size, def_mem);
      }

      size_t GetMP() const { return mptr; }
      size_t GetIP() const { return iptr; }

      bool IsReturnable() const { return returnable; }
      bool IsCircular() const { return circular; }

      memory_t & GetWorkingMem() { return working_mem; }
      memory_t & GetInputMem() { return input_mem; }
      memory_t & GetOutputMem() { return output_mem; }

      // mem_pos_t & GetWorkingPos(size_t i) { ; }

      void SetMP(size_t mp) { mptr = mp;}
      void SetIP(size_t ip) { iptr = ip; }
      
      void AdvanceIP(size_t inc=1) { iptr += inc; }
    
    };

    /// Structure of this class hails from C++ Primer 5th Ed. By Lippman, Lajoie, and Moo.
    class MemoryValue {
      public:
        enum MemoryType { NUM=0, STR=1 };
        
      protected:
        MemoryType type;
        
        std::string default_str;
        double default_num;

        union {
          double num;
          std::string str;
        };

        void CopyUnion(const MemoryValue & in) {
          switch (in.type) {
            case MemoryType::NUM: {
              num = in.num;
              break;
            }
            case MemoryType::STR: {
              new (&str) std::string(in.str);
              break;
            }
          }
        }

      public:
        MemoryValue() 
          : type(MemoryType::NUM), 
            default_str(""), 
            default_num(0),
            num(default_num) 
        { ; }

        MemoryValue(const MemoryValue & p) 
          : type(p.type),
            default_str(p.default_str),
            default_num(p.default_num)
        { CopyUnion(p); }

        MemoryValue & operator=(const MemoryValue & in) {
          using std::string;
          if (type == MemoryType::STR && in.type != MemoryType::STR) str.~string();
          else if (type == MemoryType::STR && in.type == MemoryType::STR) {
            str = in.str;
          } else {
            CopyUnion(in);
          }
          return *this;
        }

        MemoryValue & operator=(const std::string & in) {
          if (type == MemoryType::STR) {
            str = in;
          } else {
            new (&str) std::string(in);
          }
          type = MemoryType::STR;
          return *this;
        }

        MemoryValue & operator=(double in) {
          using std::string;
          if (type == MemoryType::STR) str.~string();
          num = in;
          type = MemoryType::NUM;
          return *this;
        }

        // If the union holds a non built-in type, we need to destroy it.
        ~MemoryValue() { 
          using std::string;
          if (type == MemoryType::STR) str.~string(); 
        }

        const std::string & GetDefaultStr() const { return default_str; }
        double GetDefaultNum() const { return default_num; }
        
        std::string & GetStr() { 
          if (type == MemoryType::STR) {
            return str;
          } else {
            return default_str;
          }
        }

        double GetNum() const {
          if (type == MemoryType::NUM) {
            return num;
          } else {
            // TODO - Can str be converted to number?
            return default_num;
          }
        }

        MemoryType GetType() const { return type; }

        void SetDefaultStr(const std::string & ds) { default_str = ds; }
        void SetDefaultNum(double dn) { default_num = dn; }
    };

    // TODO: round this out
    class MemoryPosition {
      protected:
        bool is_vector;
        emp::vector<MemoryValue> pos;

      public:

        MemoryPosition(bool is_vec=false)
          : is_vector(is_vec),
            pos(1) { ; }
        
        MemoryPosition(const MemoryPosition &) = default;

    };

    /// Module definition.
    struct Module {
      size_t begin;
      size_t end;
      tag_t tag;

      Module(size_t _begin=0, size_t _end=0, tag_t _tag=tag_t())
        : begin(_begin), end(_end), tag(_tag) { ; }
    };
    
    struct Flow {

    };

    struct Instruction {
      size_t id;
      emp::vector<tag_t> arg_tags;

      Instruction(size_t _id=0, const emp::vector<tag_t> & _arg_tags=emp::vector<tag_t>())
        : id(_id), arg_tags(_arg_tags) { ; }
      
      Instruction(const Instruction &) = default;
      Instruction(Instruction &&) = default;

      Instruction & operator=(const Instruction &) = default;
      Instruction & operator=(Instruction &&) = default;

      bool operator==(const Instruction & in) const {
        return id == in.id && arg_tags == in.arg_tags;
      }
      bool operator!=(const Instruction & in) const { return !(*this == in); }

      bool operator<(const Instruction & other) const {
          return std::tie(id, arg_tags) < std::tie(other.id, other.arg_tags);
      }

      size_t GetNumArgs() const { return arg_tags.size(); }

      emp::vector<tag_t> & GetArgTags() {
        return arg_tags;
      }

      tag_t & GetArgTag(size_t i) {
        emp_assert(i < arg_tags.size());
        return arg_tags[i];
      }

    };

    struct Program {
      using inst_seq_t = emp::vector<Instruction>;

      emp::Ptr<const inst_lib_t> inst_lib;  ///< Pointer to instruction library associated with this program.
      inst_seq_t program;     ///< Programs are linear sequences of instructions.

      Program(emp::Ptr<const inst_lib_t> _ilib, const inst_seq_t & _prog=inst_seq_t())
        : inst_lib(_ilib), program(_prog){ ; }
      
      Program(const Program &) = default;

      void Clear() { program.clear(); }

      Instruction & operator[](size_t id) { 
        emp_assert(id < program.size());
        return program[id]; 
      }

      const Instruction & operator[](size_t id) const {
        emp_assert(id < program.size());
        return program[id];
      }

      bool operator==(const Program & in) const { return program == in.program; }
      bool operator!=(const Program & in) const { return !(*this == in); }
      bool operator<(const Program & other) const { return program < other.program; }

      /// Get program size.
      size_t GetSize() const { return program.size(); }

      emp::Ptr<const inst_lib_t> GetInstLibPtr() const { return inst_lib; }
      const inst_lib_t & GetInstLib() const { return *inst_lib; }

      bool ValidPosition(size_t pos) const { return pos < GetSize(); }

      void Print(std::ostream & os=std::cout) {
        for (size_t i = 0; i < program.size(); ++i) {
          const inst_t & inst = program[i];
          // Print instruction
          os << inst_lib->GetName(inst.id);
          os << "(";
          for (size_t t = 0; t < inst.arg_tags.size(); ++t) {
            if (t) os << ",";
            inst.arg_tags[t].Print(os);
          }
          os << ")";
          os << "\n";
        }
      }

    };

  protected:

    emp::Ptr<emp::Random> random_ptr;
    bool random_owner;

    program_t program;
    
    emp::vector<module_t> modules;
    fun_get_modules_t get_modules_fun;

    MemoryPosition default_mem;

    size_t mem_size;
    emp::vector<tag_t> shared_mem_tags;
    emp::vector<tag_t> working_mem_tags;
    emp::vector<tag_t> input_mem_tags;
    emp::vector<tag_t> output_mem_tags;

    memory_t shared_mem;

    emp::vector<CallState> call_stack;

    size_t max_call_depth;
    double min_tag_specificity;

    bool is_executing;

  public:

  TagLinearGP_TW(emp::Ptr<const inst_lib_t> _ilib,
                 emp::Ptr<emp::Random> rnd=nullptr)
    : random_ptr(rnd), random_owner(false),
      program(_ilib),
      modules(),
      get_modules_fun(),
      default_mem(),
      mem_size(DEFAULT_MEM_SIZE),
      shared_mem_tags(mem_size),
      working_mem_tags(mem_size),
      input_mem_tags(mem_size),
      output_mem_tags(mem_size),
      shared_mem(mem_size),
      call_stack(),
      max_call_depth(DEFAULT_MAX_CALL_DEPTH),
      min_tag_specificity(DEFAULT_MIN_TAG_SPECIFICITY),
      is_executing(false)
  { 
    // If no random number generator is provided, create one (taking ownership).
    if (!rnd) NewRandom(); 
    SetDefaultGetModulesFun();
  }

  TagLinearGP_TW(inst_lib_t & _ilib, 
                 emp::Ptr<emp::Random> rnd=nullptr)
    : TagLinearGP_TW(&_ilib, rnd) { ; }
  
  TagLinearGP_TW(const hardware_t & in) 
    : random_ptr(nullptr), random_owner(false),
      program(in.program),
      modules(in.modules),
      get_modules_fun(in.get_modules_fun),
      default_mem(in.default_mem),
      mem_size(in.mem_size),
      shared_mem_tags(in.shared_mem_tags),
      working_mem_tags(in.working_mem_tags),
      input_mem_tags(in.input_mem_tags),
      output_mem_tags(in.output_mem_tags),
      shared_mem(in.shared_mem),
      call_stack(in.call_stack),
      max_call_depth(in.max_call_depth),
      min_tag_specificity(in.min_tag_specificity),
      is_executing(in.is_executing)
  {
    if (in.random_owner) NewRandom();
    else random_ptr = in.random_ptr;
  }

  ~TagLinearGP_TW() { if (random_owner) random_ptr.Delete(); }

  // ---- Hardware configuration ---

  void SetGetModulesFun(const fun_get_modules_t & fun) { get_modules_fun = fun; }

  // ---- Hardware control ----
  /// Reset everything, including the program.
  void Reset() {
    emp_assert(!is_executing);
    ResetHardware();
    modules.clear();
    program.Clear();
  }

  /// Reset only hardware, not program.
  /// Not allowed to reset hardware during execution.
  void ResetHardware() {
    emp_assert(!is_executing);
    shared_mem.clear();
    call_stack.clear();
    is_executing = false;
  }

  // ---- Accessors ----
  /// Get instruction library associated with hardware's program.
  emp::Ptr<const inst_lib_t> GetInstLib() const { return program.GetInstLib(); }

  /// Get random number generator.
  emp::Random & GetRandom() { return *random_ptr; }
  
  /// Get pointer to random number generator.
  emp::Ptr<emp::Random> GetRandomPtr() { return random_ptr; }

  /// Get program loaded on this hardware.
  const program_t & GetProgram() const { return program; }
  program_t & GetProgram() { return program; }

  
  // ---- Hardware utilities ----
  void NewRandom(int seed=-1) {
    if (random_owner) random_ptr.Delete();
    else random_ptr = nullptr;
    random_ptr = emp::NewPtr<emp::Random>(seed);
    random_owner = true;
  }

  void SetDefaultGetModulesFun() {
    get_modules_fun = [](const program_t & p) -> emp::vector<module_t> {
      return {{0, p.GetSize(), tag_t()}};
    };
  }
  
  };

}



#endif
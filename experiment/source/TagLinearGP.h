#ifndef TAG_LINEAR_GP_H
#define TAG_LINEAR_GP_H

#include <string>

#include "tools/BitSet.h"

namespace TagLGP {

  /// Instruction library class used by TagLinearGP class.
  /// Original version pulled from InstLib.h in Empirical library.
  /// - Modified to potentially facilitate some experimental functionality.
  template<typename HARDWARE_T, size_t MAX_ARG_CNT=3>
  class InstLib {
  public:
    using hardware_t = HARDWARE_T;
    using inst_t = typename hardware_t::inst_t;
    using fun_t = std::function<void(hardware_t &, const inst_t &)>; // Provide arguments, too?
    
    struct InstDef {
      std::string name;                   ///< Name of this instruction.
      fun_t fun_call;                     ///< Function to call when executing.
      size_t num_args;                    ///< Number of args needed by function.
      std::string desc;                   ///< Description of function.
      
      InstDef(const std::string & _n, fun_t _fun, 
              size_t _nargs, const std::string & _d)
        : name(_n), fun_call(_fun), num_args(_nargs), desc(_d) { ; }
      InstDef(const InstDef &) = default;
    };
    
  protected:
    emp::vector<InstDef> inst_lib;           ///< Full definitions for instructions.
    emp::vector<fun_t> inst_funs;            ///< Map of instruction IDs to their functions.
    std::map<std::string, size_t> name_map;  ///< How do names link to instructions?

    
  public:
    InstLib() : inst_lib(), inst_funs(), name_map() { ; }  ///< Default Constructor
    InstLib(const InstLib &) = default;                    ///< Copy Constructor
    InstLib(InstLib &&) = default;                         ///< Move Constructor
    ~InstLib() { ; }

    InstLib & operator=(const InstLib &) = default;        ///< Copy Operator
    InstLib & operator=(InstLib &&) = default;       

    /// Return the name associated with the specified instruction ID.
    const std::string & GetName(size_t id) const { return inst_lib[id].name; }

    /// Return the function associated with the specified instruction ID.
    const fun_t & GetFunction(size_t id) const { return inst_lib[id].fun_call; }

    /// Return the number of arguments expected for the specified instruction ID.
    size_t GetNumArgs(size_t id) const { return inst_lib[id].num_args; }

    /// Return the provided description for the provided instruction ID.
    const std::string & GetDesc(size_t id) const { return inst_lib[id].desc; }

    /// Get the number of instructions in this set.
    size_t GetSize() const { return inst_lib.size(); }

    bool IsInst(const std::string name) const {
        return Has(name_map, name);
    }

    /// Return the ID of the instruction that has the specified name.
    size_t GetID(const std::string & name) const {
      emp_assert(Has(name_map, name), name);
      return Find(name_map, name, (size_t) -1);
    }

    /// @brief Add a new instruction to the set.
    /// @param name A unique string name for this instruction.
    /// @param fun_call The function that should be called when this instruction is executed.
    /// @param num_args How many arguments does this function require? (default=0)
    /// @param desc A description of how this function operates. (default="")
    void AddInst(const std::string & name,
                 const fun_t & fun_call,
                 size_t num_args=0,
                 const std::string & desc="")
    {
      const size_t id = inst_lib.size();
      inst_lib.emplace_back(name, fun_call, num_args, desc);
      inst_funs.emplace_back(fun_call);
      name_map[name] = id;
    }

    /// Process a specified instruction in the provided hardware.
    void ProcessInst(hardware_t & hw, const inst_t & inst) const {
      inst_funs[inst.id](hw, inst);
    }

    /// Process a specified instruction on hardware that can be converted to the correct type.
    template <typename IN_HW>
    void ProcessInst(emp::Ptr<IN_HW> hw, const inst_t & inst) const {
      emp_assert( dynamic_cast<hardware_t*>(hw.Raw()) );
      inst_funs[inst.id](*(hw.template Cast<hardware_t>()), inst);
    }
  };

 
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
    using program_t = Program;
    
    // static constexpr size_t DEFAULT_MAX_CALL_DEPTH = 128;
    // static constexpr mem_val_t DEFAULT_MEM_VALUE = 0.0;
    // static constexpr double DEFAULT_MIN_BIND_THRESH = 0.5;

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

      mem_pos_t & GetWorkingPos(size_t i) 

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
        MemoryValue(double _default_num=0, const std::string & _default_str="") 
          : type(MemoryType::NUM), 
            default_str(_default_str), 
            default_num(_default_num),
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

    };

    struct Program {

    };

  protected:

    emp::Ptr<emp::Random> random_ptr;
    bool random_owner;

    program_t program;
    
    emp::vector<module_t> modules;

    MemoryPosition default_mem;

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

  TagLinearGP_TW() { ; }


  };

}



#endif
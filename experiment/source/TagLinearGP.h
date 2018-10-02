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
  // NOTES:
  // - WARNING - Can currently have infinitely recursing routine flows...
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
    class Memory;
    class MemoryValue;

    struct Program;

    using hardware_t = TagLinearGP_TW<TAG_WIDTH>;
    using tag_t = emp::BitSet<TAG_WIDTH>;
    using memory_t = Memory;
    // using mem_pos_t = MemoryPosition;

    using module_t = Module;
    using inst_t = Instruction;
    using inst_lib_t = InstLib<hardware_t>;
    using inst_prop_t = typename inst_lib_t::InstProperty;
    using inst_seq_t = emp::vector<Instruction>;
    using args_t = emp::vector<tag_t>;

    using program_t = Program;

    // using fun_get_modules_t = std::function<emp::vector<module_t>(const program_t &)>;

    static constexpr size_t DEFAULT_MEM_SIZE = 16;
    static constexpr size_t DEFAULT_MAX_CALL_DEPTH = 128;
    static constexpr double DEFAULT_MIN_TAG_SPECIFICITY = 0.0;

    enum FlowType { BASIC=0, LOOP, ROUTINE, CALL };

    struct Flow {
      FlowType type;
      size_t begin;
      size_t end;
      size_t iptr;
      size_t mptr;

      Flow(FlowType _type, size_t _begin, size_t _end,
           size_t _mptr, size_t _iptr) 
        : type(_type), 
          begin(_begin), 
          end(_end), 
          iptr(_iptr),
          mptr(_mptr)
      { ; }
    };

    /// Struct - State
    /// - Description: maintains information about local call state.
    struct CallState {
      size_t mem_size;

      memory_t working_mem;
      memory_t input_mem;
      memory_t output_mem;

      emp::vector<Flow> flow_stack; ///< Stack of Flow (top is current).

      bool returnable;  ///< Can we return from this state? (or, are we trapped here forever!)
      bool circular;    ///< Does call have an implicit return at EOM? Or, is it circular?

      CallState(Module & module,
                size_t _mem_size=128, bool _returnable=true, bool _circular=false,
                const MemoryValue & _def_mem=MemoryValue())
        : mem_size(_mem_size),
          working_mem(_mem_size, _def_mem),
          input_mem(_mem_size, _def_mem),
          output_mem(_mem_size, _def_mem),
          flow_stack({FlowType::CALL, module.begin, module.end, module.id, module.begin}),
          returnable(_returnable),
          circular(_circular)
      { ; }

      CallState(const CallState &) = default;
      CallState(CallState &&) = default;

      void Reset(const MemoryValue & def_mem=MemoryValue()) {
        working_mem.Reset(); // TODO - use default memory value?
        input_mem.Reset();
        output_mem.Reset();
      }

      bool IsReturnable() const { return returnable; }
      bool IsCircular() const { return circular; }
      bool IsFlow() const { return !flow_stack.empty(); }

      size_t GetMP() const { 
        if (flow_stack.size()) return flow_stack.back().mptr; 
        else return (size_t)-1;
      }
      
      size_t GetIP() const { 
        if (flow_stack.size()) return flow_stack.back().iptr;
        else return (size_t)-1;
      }

      emp::vector<Flow> & GetFlowStack() { return flow_stack; }
      Flow & GetTopFlow() { emp_assert(flow_stack.size()); return flow_stack.back(); }

      memory_t & GetWorkingMem() { return working_mem; }
      memory_t & GetInputMem() { return input_mem; }
      memory_t & GetOutputMem() { return output_mem; }

      // mem_pos_t & GetWorkingPos(size_t i) { ; }

      void SetMP(size_t mp) { 
        if (flow_stack.size()) flow_stack.back().mptr = mp;
      }

      void SetIP(size_t ip) { 
        if (flow_stack.size()) flow_stack.back().iptr = ip; 
      }
      
      void AdvanceIP(size_t inc=1) { 
        if (flow_stack.size()) flow_stack.back().iptr += inc; 
      }
    
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

        void Print(std::ostream & os=std::cout) const {
          switch (type) {
            case MemoryType::NUM: os << num; break;
            case MemoryType::STR: os << "\"" << str << "\""; break;
            default: os << "UNKOWN TYPE"; break;
          }
        }
    };

    /// Memory class to manage TagLGP memory.
    class Memory {
      public:
        
        struct MemoryPosition {
          bool is_vector;
          bool set;
          emp::vector<MemoryValue> pos;

          MemoryPosition(const MemoryValue & val=MemoryValue(), bool is_vec=false)
            : is_vector(is_vec), 
              set(false),
              pos(1, val) { ; }
          MemoryPosition(const MemoryPosition &) = default;

          void Print(std::ostream & os=std::cout) const {
            if (is_vector) {
              os << "[";
              for (size_t i = 0; i < pos.size(); ++i) {
                if (i) os << ",";
                pos[i].Print(os);
              }
              os << "]";
            } else {
              pos[0].Print(os);
            }
          }
        };

      protected:
        emp::vector<MemoryPosition> memory;
        
      public:
        Memory(size_t size, const MemoryValue & default_value=MemoryValue()) 
          : memory(size, {default_value})
        { ; }

        Memory(const Memory &) = default;
        Memory(Memory &&) = default;

        // /// Allow program's instruction sequence to be indexed as if a vector.
        // MemoryPosition & operator[](size_t id) { 
        //   emp_assert(id < memory.size());
        //   return memory[id];
        // }

        // /// Allow program's instruction sequence to be indexed as if a vector.
        // const MemoryPosition & operator[](size_t id) const {
        //   emp_assert(id < memory.size());
        //   return memory[id];
        // }

        void Reset(const MemoryValue & default_value=MemoryValue()) {
          const size_t size = memory.size();
          memory.clear();
          memory.resize(size, {default_value});
        }

        void Resize(size_t size, const MemoryValue & default_value=MemoryValue()) {
          memory.resize(size, {default_value});
        }

        size_t GetSize() const { return memory.size(); }

        const MemoryPosition & GetPos(size_t id) const {
          emp_assert(id < memory.size());
          return memory[id];
        }

        // emp::vector

        void Print(std::ostream & os=std::cout) const {
          os << "{";
          for (size_t i = 0; i < memory.size(); ++i) {
            if (i) os << ",";
            memory[i].Print(os);
          }
          os << "}";
        }

    };

    /// Module definition.
    struct Module {
      size_t id;
      size_t begin;   ///< First instruction in module (will be executed first).
      size_t end;     ///< Instruction pointer value this module returns (or loops back) on (1 past last instruction that is part of this module).
      tag_t tag;      ///< Module tag. Used to call/reference module.
      std::unordered_set<size_t> in_module; ///< instruction positions belonging to this module.

      Module(size_t _id, size_t _begin=0, size_t _end=0, tag_t _tag=tag_t())
        : id(_id), begin(_begin), end(_end), tag(_tag) { ; }

      bool InModule(size_t ip) const {
        return emp::Has(in_module, ip);
      }
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

      void Set(size_t _id, const emp::vector<tag_t> _args) {
        id = _id;
        arg_tags = _args;
      }

      void Set(const Instruction & other) {
        id = other.id;
        arg_tags = other.arg_tags;
      } 

    };

    struct Program {
      emp::Ptr<const inst_lib_t> inst_lib;  ///< Pointer to instruction library associated with this program.
      inst_seq_t program;     ///< Programs are linear sequences of instructions.

      Program(emp::Ptr<const inst_lib_t> _ilib, const inst_seq_t & _prog=inst_seq_t())
        : inst_lib(_ilib), program(_prog){ ; }
      
      Program(const Program &) = default;

      bool operator==(const Program & in) const { return program == in.program; }
      bool operator!=(const Program & in) const { return !(*this == in); }
      bool operator<(const Program & other) const { return program < other.program; }

      /// Allow program's instruction sequence to be indexed as if a vector.
      Instruction & operator[](size_t id) { 
        emp_assert(id < program.size());
        return program[id]; 
      }

      /// Allow program's instruction sequence to be indexed as if a vector.
      const Instruction & operator[](size_t id) const {
        emp_assert(id < program.size());
        return program[id];
      }

      // Clear program's instruction sequence.
      void Clear() { program.clear(); }

      /// Get program size.
      size_t GetSize() const { return program.size(); }

      /// Get a pointer to const instruction library.
      emp::Ptr<const inst_lib_t> GetInstLibPtr() const { return inst_lib; }

      /// Get const reference to instruction library.
      const inst_lib_t & GetInstLib() const { return *inst_lib; }

      /// Is given position a valid position in this program?
      bool ValidPosition(size_t pos) const { return pos < GetSize(); }

      /// Set program's instruction sequence to the one given.
      void SetProgram(const inst_seq_t & p) { program = p; }

      /// Push new instruction to program by instruction ID.
      void PushInst(size_t id, const args_t & args) {
        program.emplace_back(id, args);
      }

      /// Push new instruction to program by instruction name.
      void PushInst(std::string & name, const args_t & args) {
        emp_assert(inst_lib->IsInst(name), "Uknown instruction name", name);
        PushInst(inst_lib->GetID(name), args);
      }

      /// Push given instruction onto program.
      void PushInst(const inst_t & inst) {
        program.emplace_back(inst);
      }

      /// Overwrite instruction in sequence (@ position pos).
      void SetInst(size_t pos, size_t id, const args_t & args) {
        emp_assert(pos < GetSize());
        program[pos].Set(id, args);
      }

      /// Overwrite instruction in sequence (@ position pos).
      void SetInst(size_t pos, std::string & name, const args_t & args) {
        SetInst(pos, inst_lib->GetID(name), args);
      }

      /// Overwrite instruction in sequence (@ position pos).
      void SetInst(size_t pos, const inst_t & inst) {
        program[pos] = inst;
      }

      void PrintInst(const inst_t & inst, std::ostream & os=std::cout) const {
        // Print instruction
        os << inst_lib->GetName(inst.id);
        os << "(";
        for (size_t t = 0; t < inst.arg_tags.size(); ++t) {
          if (t) os << ",";
          inst.arg_tags[t].Print(os);
        }
        os << ")";
      }

      /// Plain-print program.
      void Print(std::ostream & os=std::cout) const {
        for (size_t i = 0; i < program.size(); ++i) {
          const inst_t & inst = program[i];
          PrintInst(inst, os);
          os << "\n";
        }
      }

    };

  protected:

    emp::Ptr<emp::Random> random_ptr;
    bool random_owner;

    program_t program;
    
    emp::vector<module_t> modules;

    MemoryValue default_mem_val;

    size_t mem_size;
    emp::vector<tag_t> global_mem_tags;
    emp::vector<tag_t> working_mem_tags;
    emp::vector<tag_t> input_mem_tags;
    emp::vector<tag_t> output_mem_tags;

    memory_t global_mem;

    emp::vector<CallState> call_stack;

    size_t max_call_depth;
    double min_tag_specificity;

    bool is_executing;

    void CloseFlow_BASIC(CallState & state, bool implicit) {
      emp_assert(state.IsFlow());
      // Closing BASIC flow:
      // - Pop basic flow from flow stack, passing IP and MP down.
      const size_t ip = state.GetTopFlow().iptr;
      const size_t mp = state.GetTopFlow().mptr;
      state.GetFlowStack().pop_back();
      if (state.IsFlow()) {
        Flow & top = state.GetTopFlow();
        top.iptr = ip;
        top.mptr = mp;
      }
    }

    void CloseFlow_LOOP(CallState & state, bool implicit) {
      emp_assert(state.IsFlow());
      // Closing a LOOP flow:
      // - In short, we don't. Loop ip back to flow.begin.
      Flow & top = state.GetTopFlow();
      top.iptr = top.begin;
    }

    void CloseFlow_ROUTINE(CallState & state, bool implicit) {
      emp_assert(state.IsFlow());
      // Closing a ROUTINE flow:
      // - Pop ROUTINE flow from flow stack.
      // - We don't pass IP and MP down (those on the lower stack are where we
      //   should return to).
      state.GetFlowStack.pop_back();
    }

    void CloseFlow_CALL(CallState & state, bool implicit) {
      emp_assert(state.IsFlow());
      // Closing a CALL flow:
      // - Pop call flow from flow stack.
      // - No need to pass IP and MP down (presumably, this was the bottom of the
      //   flow stack).
      if (implicit && state.IsCircular()) {
        Flow & top = state.GetTopFlow();
        top.iptr = top.begin;
      } else {
        state.GetFlowStack.pop_back();
      }
    }

  public:

    TagLinearGP_TW(emp::Ptr<const inst_lib_t> _ilib,
                  emp::Ptr<emp::Random> rnd=nullptr)
      : random_ptr(rnd), random_owner(false),
        program(_ilib),
        modules(),
        default_mem_val(),
        mem_size(DEFAULT_MEM_SIZE),
        global_mem_tags(mem_size),
        working_mem_tags(mem_size),
        input_mem_tags(mem_size),
        output_mem_tags(mem_size),
        global_mem(mem_size, default_mem_val),
        call_stack(),
        max_call_depth(DEFAULT_MAX_CALL_DEPTH),
        min_tag_specificity(DEFAULT_MIN_TAG_SPECIFICITY),
        is_executing(false)
    { 
      // If no random number generator is provided, create one (taking ownership).
      if (!rnd) NewRandom(); 
    }

    TagLinearGP_TW(inst_lib_t & _ilib, 
                  emp::Ptr<emp::Random> rnd=nullptr)
      : TagLinearGP_TW(&_ilib, rnd) { ; }
    
    TagLinearGP_TW(const hardware_t & in) 
      : random_ptr(nullptr), random_owner(false),
        program(in.program),
        modules(in.modules),
        default_mem_val(in.default_mem_val),
        mem_size(in.mem_size),
        global_mem_tags(in.global_mem_tags),
        working_mem_tags(in.working_mem_tags),
        input_mem_tags(in.input_mem_tags),
        output_mem_tags(in.output_mem_tags),
        global_mem(in.global_mem),
        call_stack(in.call_stack),
        max_call_depth(in.max_call_depth),
        min_tag_specificity(in.min_tag_specificity),
        is_executing(in.is_executing)
    {
      if (in.random_owner) NewRandom();
      else random_ptr = in.random_ptr;
    }

    ~TagLinearGP_TW() { if (random_owner) random_ptr.Delete(); }

    // ---------------------------- Hardware configuration ----------------------------

    /// Set program for this hardware object.
    /// After updating hardware's program, run UpdateModules to update module definitions.
    void SetProgram(const program_t & _program) { 
      emp_assert(!is_executing);
      program = _program; 
      UpdateModules();
    }

    /// Set hardware memory size (number of memory positions per memory buffer)
    void SetMemSize(size_t size) {
      mem_size = size;
      global_mem.Resize(mem_size, default_mem_val);
      global_mem_tags.resize(mem_size, tag_t());
      for (size_t i = 0; i < call_stack.size(); ++i) {
        CallState & state = call_stack[i];
        
        state.input_mem.Resize(mem_size, default_mem_val);
        state.working_mem.Resize(mem_size, default_mem_val);
        state.output_mem.Resize(mem_size, default_mem_val);

        input_mem_tags.resize(mem_size, tag_t());
        working_mem_tags.resize(mem_size, tag_t());
        output_mem_tags.resize(mem_size, tag_t());

      }
    }

    // ---------------------------- Hardware control ----------------------------
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
      global_mem.clear();
      call_stack.clear();
      is_executing = false;
    }

    /// Update modules.
    void UpdateModules() {
      // Grab reference to program's instruction library.
      const inst_lib_t & ilib = program.GetInstLib();
      // Clear out current module definitions.
      modules.clear();
      // Scan program for module definitions.
      std::unordered_set<size_t> dangling_instructions;
      for (size_t pos = 0; pos < program.GetSize(); ++pos) {
        inst_t & inst = program[pos];
        // Is this a module definition?
        if (ilib.HasProperty(inst.id, inst_prop_t::MODULE)) {
          if (modules.size()) { modules.back().end = pos; } 
          const size_t mod_id = modules.size();
          modules.emplace_back(mod_id, (pos+1)%program.GetSize(), -1, inst.arg_tags[0]);
        } else {
          // Not a new module definition.
          if (modules.size()) { modules.back().in_module.emplace(pos); }
          else { dangling_instructions.emplace(pos); }
        }
      }
      // Take care of dangling instructions and add default module if necessary.
      if (modules.size()) {                               // if we've found at least one module, set it's end point.
        if (modules[0].begin == 0) modules.back().end = program.GetSize();
        else modules.back().end = modules[0].begin-1;
      } else {                                            // found no modules, add a default module.
        // Default module starts at beginning of program and ends at the end.
        modules.emplace_back(0, 0, program.GetSize(), tag_t());
      }
      for (size_t val : dangling_instructions) modules.back().in_module.emplace(val);
    }

    // ---------------------------- Hardware execution ----------------------------
    /// Process a single instruction, provided by the caller.
    void ProcessInst(const inst_t & inst) { program.GetInstLib().ProcessInst(*this, inst); }

    /// Advance hardware by a single instruction.
    void SingleProcess() {
      emp_assert(program.GetSize()); // Must have a non-empty program to advance the hardware.

      is_executing = true;

      // If there's a call state on the call stack, execute an instruction in
      // that call.

      // Repeat:
      while (call_stack.size()) {
        CallState & state = call_stack.back();
        // todo - check validity of ip/mp
        if (state.IsFlow()) {
          Flow & top_flow = state.GetTopFlow();
          const size_t ip = top_flow.iptr;
          const size_t mp = top_flow.mptr;
          emp_assert(mp < modules.size());
          if (modules[mp].InModule(ip)) { // Valid IP
            // First, increment flow's IP by 1. This may invalidate the IP, but
            // that's okay.
            ++top_flow.iptr;
            // Next, run instruction @ ip.
            GetInstLib().ProcessInst(*this, program[ip]);
          } else { 
            CloseFlow(state);
            continue;
          }
        } else {
          // Return from CallState
          ReturnCall();
          continue; // Implicit returns are free for now...
        }
        break;
      }
      
      is_executing = false;

    }

    void OpenFlow(CallState & state) {


    }

    void CloseFlow(CallState & state, bool implicit=false) {
      if (!state.IsFlow()) return;
      switch (state.GetTopFlow().type) {
        case BASIC: CloseFlow_BASIC(state, implicit); break;
        case LOOP: CloseFlow_LOOP(state, implicit); break;
        case ROUTINE: CloseFlow_ROUTINE(state, implicit); break;
        case CALL: CloseFlow_CALL(state, implicit); break;
        default:
          std::cout << "Uknown flow type (" << state.GetFlow().type << ")!" << std::endl;
      }
    }

    void Call() {

    }

    void ReturnCall() {
      // bool returnable;  ///< Can we return from this state? (or, are we trapped here forever!)
      // bool circular;    ///< Does call have an implicit return at EOM? Or, is it circular?
      if (call_stack.empty()) return; // Nothing to return from.
      CallState & returning_state = GetCurCallState();

      // No returning from non-returnable call state.
      if (!returning_state.IsReturnable()) {
        // TODO
        return;
      }  
      
      // Is there anything to return to?
      if (call_stack.size() > 1) {
        // If so, copy returning state's output memory into caller state's local memory.
        CallState & caller_state = call_stack[call_stack.size() - 2];
        // TODO
        // for (const MemoryPosition & mem : returning_state.GetOutputMem()) caller_state.SetWorking(mem);
      }
      call_stack.pop_back();
    }
    
    // ---------------------------- Accessors ----------------------------
    /// Get instruction library associated with hardware's program.
    emp::Ptr<const inst_lib_t> GetInstLib() const { return program.GetInstLib(); }

    /// Get random number generator.
    emp::Random & GetRandom() { return *random_ptr; }
    
    /// Get pointer to random number generator.
    emp::Ptr<emp::Random> GetRandomPtr() { return random_ptr; }

    /// Get program loaded on this hardware.
    const program_t & GetProgram() const { return program; }
    program_t & GetProgram() { return program; }

    /// Get the minimum tag specificity (i.e., required similarity between two tags
    /// for a successful reference to occur).
    double GetMinTagSpecificity() const { return min_tag_specificity; }

    /// Get maximum allowed call depth (maximum number of call states allowed on
    /// the call stack at any one time).
    size_t GetMaxCallDepth() const { return max_call_depth; }

    /// Get memory size. How many memory positions are available in input, output,
    /// working, and global memory.
    size_t GetMemSize() const { return mem_size; }

    /// Get global memory vector.
    memory_t & GetGlobalMem() { return global_mem; }
    const memory_t & GetGlobalMem() const { return global_mem; }

    /// memory tag accessors
    emp::vector<tag_t> & GetGlobalMemTags() { return global_mem_tags; }
    const emp::vector<tag_t> & GetGlobalMemTags() const { return global_mem_tags; }
    emp::vector<tag_t> & GetWorkingMemTags() { return working_mem_tags; }
    const emp::vector<tag_t> & GetWorkingMemTags() const { return working_mem_tags; }
    emp::vector<tag_t> & GetInputMemTags() { return input_mem_tags; }
    const emp::vector<tag_t> & GetInputMemTags() const { return input_mem_tags; }
    emp::vector<tag_t> & GetOutputMemTags() { return output_mem_tags; }
    const emp::vector<tag_t> & GetOutputMemTags() const { return output_mem_tags; }

    CallState & GetCurCallState() {
      emp_assert(call_stack.size(), "Cannot query for current call state if call stack is empty.");
      return call_stack.back();
    }

    const CallState & GetCurCallState() const { 
      emp_assert(call_stack.size(), "Cannot query for current call state if call stack is empty.");
      return call_stack.back();
    }

    // ---------------------------- Hardware utilities ----------------------------
    void NewRandom(int seed=-1) {
      if (random_owner) random_ptr.Delete();
      else random_ptr = nullptr;
      random_ptr = emp::NewPtr<emp::Random>(seed);
      random_owner = true;
    }

    // ---------------------------- Printing ----------------------------
    void PrintModules(std::ostream & os=std::cout) {
      os << "Modules: {";
      for (size_t i = 0; i < modules.size(); ++i) {
        if (i) os << ",";
        os << "[" << modules[i].begin << ":" << modules[i].end << "]";
        os << "(";
        modules[i].tag.Print(os);
        os << ")";
      }
      os << "}";
    }

    void PrintModuleSequences(std::ostream & os=std::cout) {
      for (size_t i = 0; i < modules.size(); ++i) {
        Module & module = modules[i];
        os << "Module["<<module.id<<"](";
        module.tag.Print(os);
        os << ")\n";
        size_t msize = (module.begin < module.end) ? module.end - module.begin : (program.GetSize() - module.begin) + module.end;
        std::cout << "#size=" << msize << std::endl;
        for (size_t mip = 0; mip < msize; ++mip) {
          const size_t ip = (module.begin+mip)%program.GetSize();
          os << "  ip[" << ip << "]: ";
          program.PrintInst(program[ip], os);
          // else program.PrintInst(program[mip % program.GetSize()], os);
          os << "\n";
        }
      }
    }

    void PrintMemoryVerbose(std::ostream & os=std::cout) {
      os << "-- Global memory --\n";
      for (size_t gi = 0; gi < mem_size; ++gi) {
        os << "  mem[" << gi << "](";
        global_mem_tags[gi].Print(os);
        os << "): ";
        global_mem.GetPos(gi).Print(os);
        os << "\n";
      }
      for (int ci = (int)call_stack.size()-1; ci >= 0; --ci) {
        CallState & state = call_stack[ci];
        os << "Local Memory (stack id=" << ci << ")\n";
        
        os << "  -- Input Memory -- \n";
        for (size_t i = 0; i < mem_size; ++i) {
          os << "    mem[" << i << "](";
          input_mem_tags[i].Print(os);
          os << "): ";
          state.GetInputMem().GetPos(i).Print(os);
          os << "\n";
        } 

        os << "  -- Working Memory -- \n";
        for (size_t i = 0; i < mem_size; ++i) {
          os << "    mem[" << i << "](";
          working_mem_tags[i].Print(os);
          os << "): ";
          state.GetWorkingMem().GetPos(i).Print(os);
          os << "\n";
        } 

        os << "  -- Output Memory -- \n";
        for (size_t i = 0; i < mem_size; ++i) {
          os << "    mem[" << i << "](";
          output_mem_tags[i].Print(os);
          os << "): ";
          state.GetOutputMem().GetPos(i).Print(os);
          os << "\n";
        } 


      }


    }

  };

}



#endif
#pragma once

#include <cstring>
#include <deque>
#include <iomanip>
#include <iostream>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <ast_type_specifier.hpp>
#include <operation/ast_assignment_op.hpp>

// change the following constant to change the number of bytes reserved at the
// beginning of each function. TODO (one day not now lol): dynamically allocate
// the size for each function (create a helper function through the node class,
// which returns the number of bytes needed per branch within the ast, similar
// to "getType()" or "getIdentifier()", but for space required)
#define STACK_FRAME_SIZE 256

namespace ast
{

enum class SCOPE_
{
  GLOBAL,
  LOCAL,
  ASSIGN
};
struct VariableInfo
{
  int stack_location;
  TypeSpecifier type;
  bool isArray;
  std::vector<int> array_sizes = {};
  bool isPointer = false;
  std::vector<TypeSpecifier> parameter_types;
  SCOPE_ scope;
  int pointerElementSize;
};

// struct LabelInfo
// {
//   std::string label_;
//   uint32_t ieee754_value;
// };

typedef std::unordered_map<std::string, VariableInfo> symbol_table_;
typedef std::unordered_map<std::string, int> enums;
typedef std::unordered_set<std::string> enum_types_;
typedef std::unordered_map<int, std::string> cases_map_;
// because we use the value for the lookup, we use int as the key

class Context
{
private:
  std::stack<std::string> free_registers; // Track available registers
  std::unordered_map<std::string, int>
      spilled_registers; // Track spilled registers
  int stack_top = 0;     // Track current stack offset
  int stack_offset = 0;

  std::stack<symbol_table_> symbol_table_stack_;
  std::stack<enums> enum_stack;
  std::stack<enum_types_> enum_types_stack_;

  int stack_frame_pointer = 0;
  std::unordered_map<std::string, std::string> struct_instances;

  std::string dest_reg_ = "";
  TypeSpecifier current_type_;

  // array to keep track of register usage
  std::array<int, 32> registers;
  std::deque<std::string> used_registers;

  const std::array<int, 32> registers_default = {
    1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  };

  // We map the register names, following
  // https://github.com/riscv-non-isa/riscv-elf-psabi-doc
  const std::unordered_map<std::string, int> register_map
      = { { "zero", 0 }, { "ra", 1 },  { "sp", 2 },   { "gp", 3 },
          { "tp", 4 },   { "t0", 5 },  { "t1", 6 },   { "t2", 7 },
          { "s0", 8 },   { "s1", 9 },  { "a0", 10 },  { "a1", 11 },
          { "a2", 12 },  { "a3", 13 }, { "a4", 14 },  { "a5", 15 },
          { "a6", 16 },  { "a7", 17 }, { "s2", 18 },  { "s3", 19 },
          { "s4", 20 },  { "s5", 21 }, { "s6", 22 },  { "s7", 23 },
          { "s8", 24 },  { "s9", 25 }, { "s10", 26 }, { "s11", 27 },
          { "t3", 28 },  { "t4", 29 }, { "t5", 30 },  { "t6", 31 } };

  // float logic
  std::array<int, 32> f_registers;

  const std::array<int, 32> f_registers_default = {
    // Float registers (indices 32-63): assume all are free (0) initially.
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };

  // The next 32 entries are the float registers.
  const std::unordered_map<std::string, int> f_register_map = {
    // Float registers (indices 32-63):
    { "ft0", 32 }, { "ft1", 33 }, { "ft2", 34 },  { "ft3", 35 },
    { "ft4", 36 }, { "ft5", 37 }, { "ft6", 38 },  { "ft7", 39 },
    { "fs0", 40 }, { "fs1", 41 }, { "fa0", 42 },  { "fa1", 43 },
    { "fa2", 44 }, { "fa3", 45 }, { "fa4", 46 },  { "fa5", 47 },
    { "fa6", 48 }, { "fa7", 49 }, { "fs2", 50 },  { "fs3", 51 },
    { "fs4", 52 }, { "fs5", 53 }, { "fs6", 54 },  { "fs7", 55 },
    { "fs8", 56 }, { "fs9", 57 }, { "fs10", 58 }, { "fs11", 59 },
    { "ft8", 60 }, { "ft9", 61 }, { "ft10", 62 }, { "ft11", 63 }
  };

  int paramIndex_ = 0;
  int aIndex_ = 0;
  int fIndex_ = 0;

  bool state_isArray_ = false;
  bool left_op_ = false;
  bool array_index_ = false;

  // for global variables use only
  bool isInitialized_ = false;

  bool FunctionDefinition_ = false;

  int scope_index = 0;

  int init_index_ = 0;
  int init_arr_loc_base_ = 0;

  std::stack<SCOPE_> scope_;

  std::unordered_map<std::string, int> label_map;

  int fconst_count_ = 0;
  std::unordered_map<std::string, uint32_t> const_label_;
  std::unordered_map<std::string, std::pair<uint32_t, uint32_t> >
      double_const_label_;

  std::unordered_map<std::string, std::string> string_label_;


  bool is_current_pointer_ = false;
  bool pointer_init_ = false;

  std::stack<cases_map_> cases_map_stack_;
  std::string end_switch_label_ = "";
  std::string default_switch_label_ = "";

  std::stack<std::string> break_label_;
  std::stack<std::string> continue_label_;

  std::unordered_map<std::string, TypeSpecifier> struct_definitions;


    std::unordered_map<std::string, std::unordered_map<std::string, int>> struct_member_offsets;

    std::unordered_map<std::string, std::unordered_map<std::string, TypeSpecifier>> struct_member_types;
    std::unordered_map<std::string, int> stack_map;

  // begin with a constructor indicating we are in the global scope
public:
  Context ()
  {
    if (scope_.empty ())
      {
        scope_.push (SCOPE_::GLOBAL);
        symbol_table_stack_.push (symbol_table_ ());
        enum_stack.push (enums ());
        enum_types_stack_.push (enum_types_ ());
        scope_index++;
      }

    initalize_regs ();
  }

  void setPointerElementSize (const std::string &identifier, int size);
  int getPointerElementSize (const std::string &identifier) const;

  int getBytes (TypeSpecifier &type_);

  void initalize_regs ();
  void initalize_arg_regs ();

  std::string allocate_reg (TypeSpecifier &type);
  std::string allocate_arg_reg (TypeSpecifier &type);
  void deallocate_reg (std::string &reg);
  void deallocate_f_reg (std::string &reg);

  void allocate_reg_manual (std::string &reg, TypeSpecifier type);

  void setDestReg (std::string &dest_reg);
  std::string getDestReg ();

  // stack logic
  void init_stack (std::ostream &stream);
  void end_stack (std::ostream &stream);
  int push_stack (int bytes_);
  void pop_stack (int bytes_);
  void allocate_stack (std::string identifier_, TypeSpecifier type);
  void allocate_stack_array (std::string identifier_, int &size);
  std::string get_load_instr (TypeSpecifier &type);
  std::string get_store_instr (TypeSpecifier &type);

  void load (std::ostream &stream, std::string &identifier_,
             std::string &dest_reg, std::string addr_reg = "");
  void store (std::ostream &stream, std::string &identifier_,
              std::string &dest_reg, std::string addr_reg = "");

  //   void load_global (std::ostream &stream, std::string &identifier_,
  //                     std::string &dest_reg, std::string addr_reg = "");
  //   void store_global (std::ostream &stream, std::string &identifier_,
  //                      std::string &dest_reg, std::string addr_reg = "");

  bool isInitialised (std::string &identifier_) const;

  //   labels for jumps:
  std::string makeUniqueLabel (std::string prefix);
  std::string makeUniqueConstLabel (std::string prefix,
                                    uint32_t ieee754_value);
  std::string makeUniqueDoubleConstLabel (std::string prefix, uint32_t lower,
                                          uint32_t upper);
  std::string makeUniqueStringLabel (std::string prefix, std::string string);

  void emitLabels (std::ostream &stream);

  void PrintAllocatedRegs ();

  void push_scope ();

  //   When it rains, it pours - She like the way I rrrrrrrrrrrrrrrrrr
  void pop_smoke ();

  VariableInfo findSymbol (const std::string &identifier) const;

  TypeSpecifier
  getType (std::string identifier_)
  {
    return findSymbol (identifier_).type;
  }

  int
  getLoc (std::string identifier_)
  {
    return findSymbol (identifier_).stack_location;
  }

  void
  setCurrentType (TypeSpecifier type_)
  {
    current_type_ = type_;
  }

  TypeSpecifier
  getCurrentType ()
  {
    return current_type_;
  }

  void
  incrementaArgIndex ()
  {
    aIndex_++;
  }

  void
  incrementfArgIndex ()
  {
    fIndex_++;
  }

  void
  resetParamIndex ()
  {
    paramIndex_ = 0;
  }

  void
  resetParamFIndex ()
  {
    fIndex_ = 0;
  }

  int
  getParamFIndex ()
  {
    return fIndex_;
  }

  void
  resetParamAIndex ()
  {
    aIndex_ = 0;
  }

  int
  getParamAIndex ()
  {
    return aIndex_;
  }

  bool
  isArray (std::string &identifier_) const
  {
    return findSymbol (identifier_).isArray;
  }

  void
  setLeftOp (bool left_op)
  {
    left_op_ = left_op;
  }

  bool
  isLeftOp ()
  {
    return left_op_;
  }

  void
  setArrayIndex (bool array_index)
  {
    array_index_ = array_index;
  }

  bool
  isArrayIndex ()
  {
    return array_index_;
  }

  void add_function_declaration_type (const std::string &identifier,
                                      TypeSpecifier type);

  void
  setPointerInit (bool isPointer)
  {
    pointer_init_ = isPointer;
  }

  // Set whether a given identifier is a pointer.
  void set_is_pointer (bool is_pointer, const std::string &identifier);

  // Get the pointer flag for a given identifier.
  bool get_is_pointer (const std::string &identifier) const;

  bool
  isScopeGlobal ()
  {
    return scope_.top () == SCOPE_::GLOBAL;
  }

  bool isInitialisedinScope (std::string &identifier) const;

  void
  setIsInitialized (bool &isInitialized)
  {
    isInitialized_ = isInitialized;
  }

  bool
  isGloblInitialized ()
  {
    return isInitialized_;
  }

  void AddFunctionDeclaration (std::string &identifier);

  void
  setFunctionDefinition (bool FunctionDefinition)
  {
    FunctionDefinition_ = FunctionDefinition;
  }

  bool
  isFunctionDefinition ()
  {
    return FunctionDefinition_;
  }

  void store_regs (std::ostream &stream, std::string &dest_reg);

  void addGlobalVar (std::string &identifier);

  SCOPE_ getVarScope (std::string &identifier);

  void
  resetInitIndex ()
  {
    init_index_ = 0;
  }

  void
  incrementInitIndex ()
  {
    init_index_++;
  }

  int
  getInitIndex ()
  {
    return init_index_;
  }

  int
  getArrayLocBase ()
  {
    return init_arr_loc_base_;
  }

  void
  setIsCurrentPointer (bool is_current_pointer)
  {
    is_current_pointer_ = is_current_pointer;
  }

  bool
  isCurrentPointer ()
  {
    return is_current_pointer_;
  }

  void addEnumVal (std::string identifier, int &value_);

  bool isEnumValInScope (std::string &identifier) const;

  int findEnumVal (const std::string &identifier) const;

  void addEnumType (std::string identifier);

  bool isEnumTypeInScope (std::string &identifier) const;

  void enterSwitch ();

  void exitSwitch ();

  void addCase (int value, const std::string &label);

  cases_map_ getCurrentCaseMap ();

  std::string getLabelCase (int &val_);

  void setEndSwitch (const std::string &label);

  void setDefaultSwitch (const std::string &label);

  std::string getEndSwitch ();

  std::string getDefaultSwitch ();

  void
  pushBreakLabel (const std::string &label)
  {
    break_label_.push (label);
  }

  void
  popBreakLabel ()
  {
    break_label_.pop ();
  }

  std::string
  getBreakLabel () const
  {
    return break_label_.empty () ? "" : break_label_.top ();
  }

  void
  pushContinueLabel (const std::string &label)
  {
    continue_label_.push (label);
  }

  void
  popContinueLabel ()
  {
    continue_label_.pop ();
  }

  std::string
  getContinueLabel () const
  {
    return continue_label_.empty () ? "" : continue_label_.top ();
  }

  std::string spill_register (TypeSpecifier &type);

  int allocate_stack_space (TypeSpecifier &type);

  int get_spill_offset (const std::string &reg);

  bool is_spilled (const std::string &reg);

  void clearStructMembers();
  void defineStruct(const std::string &name, TypeSpecifier type);
  int get_stack_location(const std::string &identifier) const;
  bool hasStruct(const std::string &name) const;
  void setStructInstance(const std::string &name);
  std::string getStructInstance(const std::string &name) const;
  bool has_stack_entry(const std::string &identifier) const;

};

} // namespace ast

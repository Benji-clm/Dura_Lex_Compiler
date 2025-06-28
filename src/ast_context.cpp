#include "ast_context.hpp"

namespace ast
{
int
Context::getBytes (TypeSpecifier &type_)
{
  switch (type_)
    {
    case TypeSpecifier::FLOAT:
      return 4;
    case TypeSpecifier::INT:
      return 4;
    case TypeSpecifier::DOUBLE:
      return 8;
    case TypeSpecifier::CHAR:
      return 1;
    case TypeSpecifier::STRUCT:
      return 1;
    case TypeSpecifier::VOID:
      return 0;
    default:
      throw std::runtime_error ("getBytes() - unrecognised type");
    }
}

void
Context::initalize_regs ()
{
  used_registers.clear ();

  for (const auto &reg_pair : register_map)
    {
      const std::string &reg_name = reg_pair.first;
      int reg_index = reg_pair.second;

      // Check the default state and add to used_registers if allocated
      if (registers_default[reg_index] == 1)
        {
          registers[reg_index] = 1;
          used_registers.push_back (reg_name);
        }
      else
        {
          registers[reg_index] = 0;
        }
    }

  for (const auto &reg_pair : f_register_map)
    {
      const std::string &reg_name = reg_pair.first;
      int reg_index = reg_pair.second;

      // Check the default state and add to used_registers if allocated
      if (f_registers_default[reg_index] == 1)
        {
          f_registers[reg_index] = 1;
          used_registers.push_back (reg_name);
        }
      else
        {
          f_registers[reg_index] = 0;
        }
    }
}

void
Context::initalize_arg_regs ()
{
  for (int i = 0; i < 8; i++)
    {
      std::string reg_name = "a" + std::to_string (i);
      int reg_index = register_map.at (reg_name);
      if (registers[reg_index] == 1)
        {
          deallocate_reg (reg_name);
        }
    }

  for (int i = 0; i < 8; i++)
    {
      std::string reg_name = "fa" + std::to_string (i);
      int reg_index = f_register_map.at (reg_name);
      if (f_registers[reg_index] == 1)
        {
          deallocate_reg (reg_name);
        }
    }
}

std::string
Context::allocate_arg_reg (TypeSpecifier &type)
{
  switch (type)
    {
    case TypeSpecifier::DOUBLE:
    case TypeSpecifier::FLOAT:
      {
        auto try_allocate_from_f
            = [&] (const std::vector<std::string> &candidates) -> std::string {
          for (const std::string &reg_name : candidates)
            {
              int reg_index = f_register_map.at (reg_name);
              if (f_registers[reg_index] == 0) // If the register is free
                {
                  f_registers[reg_index] = 1;          // Mark it as used
                  used_registers.push_back (reg_name); // Track usage
                  return reg_name;
                }
            }
          return ""; // No available registers
        };

        std::string allocated_register = try_allocate_from_f (
            { "fa0", "fa1", "fa2", "fa3", "fa4", "fa5", "fa6", "fa7" });

        return allocated_register;
      }
    case TypeSpecifier::INT:
    default:
      {
        auto try_allocate_from
            = [&] (const std::vector<std::string> &candidates) -> std::string {
          for (const std::string &reg_name : candidates)
            {
              int reg_index = register_map.at (reg_name);
              if (registers[reg_index] == 0) // If the register is free
                {
                  registers[reg_index] = 1;            // Mark it as used
                  used_registers.push_back (reg_name); // Track usage
                  return reg_name;
                }
            }
          return ""; // No available registers
        };

        std::string allocated_register = try_allocate_from (
            { "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7" });

        return allocated_register;
      }
    }

  PrintAllocatedRegs ();
  throw std::runtime_error (
      "allocate_arg_reg () - No available argument registers");
}

std::string
Context::allocate_reg (TypeSpecifier &type)
{
  std::string allocated_register = "";

  // Lambda function to find an available register
  auto try_allocate_from
      = [&] (const std::vector<std::string> &candidates) -> std::string {
    for (const std::string &reg_name : candidates)
      {
        int reg_index = register_map.at (reg_name);
        if (registers[reg_index] == 0)
          {
            registers[reg_index] = 1;
            used_registers.push_back (reg_name);
            return reg_name;
          }
      }
    return "";
  };

  switch (type)
    {
    case TypeSpecifier::DOUBLE:
    case TypeSpecifier::FLOAT:
      {
        auto try_allocate_from_f
            = [&] (const std::vector<std::string> &candidates) -> std::string {
          for (const std::string &reg_name : candidates)
            {
              int reg_index = f_register_map.at (reg_name);
              if (f_registers[reg_index] == 0)
                {
                  f_registers[reg_index] = 1;
                  used_registers.push_back (reg_name);
                  return reg_name;
                }
            }
          return "";
        };

        allocated_register = try_allocate_from_f (
            { "ft0", "ft1", "ft2", "ft3", "ft4", "ft5", "ft6", "ft7" });

        if (allocated_register.empty ())
          {
            allocated_register = try_allocate_from_f (
                { "fa1", "fa2", "fa3", "fa4", "fa5", "fa6", "fa7" });
          }
        break;
      }
    case TypeSpecifier::INT:
    case TypeSpecifier::UNSIGNED_INT:
    default:
      {
        allocated_register
            = try_allocate_from ({ "t0", "t1", "t2", "t3", "t4", "t5", "t6" });

        if (allocated_register.empty ())
          {
            allocated_register = try_allocate_from (
                { "a1", "a2", "a3", "a4", "a5", "a6", "a7" });
          }
        break;
      }
    }

  if (!allocated_register.empty ())
    {
      return allocated_register;
    }

  return spill_register (type);
}

// only used in f def for the moment, could also create an "allocate return reg
// function instead?"
void
Context::allocate_reg_manual (std::string &regName, TypeSpecifier type)
{
  if (type == TypeSpecifier::FLOAT || type == TypeSpecifier::DOUBLE)
    {
      auto it = f_register_map.find (regName);
      if (it == f_register_map.end ())
        {
          throw std::runtime_error ("allocate_reg_manual: \"" + regName
                                    + "\" not found in f_register_map");
        }

      int reg_index = f_register_map.at (regName);
      // If it's already allocated, warn or throw an error
      if (f_registers[reg_index] == 1)
        {
          throw std::runtime_error ("allocate_reg_manual: Register \""
                                    + regName + "\" is already allocated");
        }

      f_registers[reg_index] = 1;
      used_registers.push_back (regName);
    }
  else
    {
      auto it = register_map.find (regName);
      if (it == register_map.end ())
        {
          throw std::runtime_error ("allocate_reg_manual: \"" + regName
                                    + "\" not found in register_map");
        }

      int reg_index = register_map.at (regName);

      if (registers[reg_index] == 1)
        {
          throw std::runtime_error ("allocate_reg_manual: Register \""
                                    + regName + "\" is already allocated");
        }

      registers[reg_index] = 1;
      used_registers.push_back (regName);
    }
}

std::string
Context::spill_register (TypeSpecifier &type)
{
  if (used_registers.empty ())
    {
      throw std::runtime_error (
          "Context::spill_register - No registers available to spill");
    }

  std::string reg_to_spill
      = used_registers.back (); // Get least recently used register
  used_registers.pop_back ();   // Remove from active list

  int bytes = getBytes (type);
  int spill_offset = push_stack (bytes);          // Reserve stack space
  spilled_registers[reg_to_spill] = spill_offset; // Track where it was spilled

  std::cerr << "Spilling " << reg_to_spill << " to stack offset "
            << spill_offset << "\n";

  return reg_to_spill; // Return spilled register
}

// int Context::allocate_stack_space(TypeSpecifier &type)
// {
//     int type_size = (type == TypeSpecifier::FLOAT) ? 4 : 4; // Assume 4
//     bytes per register stack_offset += type_size; return -stack_offset; //
//     Return negative offset (stack grows down)
// }

int
Context::get_spill_offset (const std::string &reg)
{
  if (spilled_registers.find (reg) != spilled_registers.end ())
    {
      return spilled_registers[reg];
    }
  throw std::runtime_error (
      "Context::get_spill_offset - Register not spilled: " + reg);
}

bool
Context::is_spilled (const std::string &reg)
{
  return spilled_registers.find (reg) != spilled_registers.end ();
}

void
Context::deallocate_reg (std::string &reg)
{
  if (spilled_registers.find (reg) != spilled_registers.end ())
    {
      spilled_registers.erase (reg);
      return;
    }

  auto it = register_map.find (reg);
  if (it != register_map.end ())
    {
      int reg_index = it->second;
      registers[reg_index] = 0;

      auto used_it
          = std::find (used_registers.begin (), used_registers.end (), reg);
      if (used_it != used_registers.end ())
        {
          used_registers.erase (used_it);
        }
      return;
    }

  auto itf = f_register_map.find (reg);
  if (itf != f_register_map.end ())
    {
      int reg_index = itf->second;
      f_registers[reg_index] = 0;

      auto used_it
          = std::find (used_registers.begin (), used_registers.end (), reg);
      if (used_it != used_registers.end ())
        {
          used_registers.erase (used_it);
        }
      return;
    }

  throw std::runtime_error ("Context::deallocate_reg - Unknown register: "
                            + reg);
}

/*
NOTE TO SELF:
We could use the fact that every float registers names start with "f" to
determine which map to look into -> maps are very fast hence would it improve
efficiency?
*/

void
Context::PrintAllocatedRegs ()
{
  std::cout << "Allocated Registers: ";

  if (used_registers.empty ())
    {
      std::cout << "None" << std::endl;
      return;
    }

  for (const auto &reg : used_registers)
    {
      std::cout << reg << " ";
    }

  std::cout << std::endl;
}

// stack logic
void
Context::init_stack (std::ostream &stream)
{

  stream << "addi sp,sp,-" << STACK_FRAME_SIZE << std::endl;
  stream << "sw ra," << STACK_FRAME_SIZE - 4 << "(sp)" << std::endl;
  stream << "sw s0," << STACK_FRAME_SIZE - 8 << "(sp)" << std::endl;
  stream << "addi s0,sp," << STACK_FRAME_SIZE << std::endl;

  stack_frame_pointer = -16;
}

void
Context::end_stack (std::ostream &stream)
{

  stream << "lw ra," << STACK_FRAME_SIZE - 4 << "(sp)" << std::endl;
  stream << "lw s0," << STACK_FRAME_SIZE - 8 << "(sp)" << std::endl;
  stream << "addi sp,sp," << STACK_FRAME_SIZE << std::endl;
}

int
Context::push_stack (int bytes_)
{
  if (stack_frame_pointer - bytes_ < -STACK_FRAME_SIZE)
    {
      throw std::runtime_error (
          "Context::allocate_stack() - Stack overlow, stack "
          "limited to 256 bytes");
    }

  stack_frame_pointer -= bytes_;

  return stack_frame_pointer;
}

void
Context::pop_stack (int bytes_)
{
  stack_frame_pointer += bytes_;
}

void Context::allocate_stack(std::string identifier, TypeSpecifier type_) {
    int bytes = 4;

    if (!pointer_init_) {
        bytes = getBytes(type_);
    }

    VariableInfo var;
    var.type = type_;
    var.stack_location = push_stack(bytes);
    var.isArray = false;
    var.scope = SCOPE_::LOCAL;
    var.isPointer = pointer_init_;

    symbol_table_stack_.top()[identifier] = var;

    // ✅ If struct, also allocate for members
    if (type_ == TypeSpecifier::STRUCT) {
        for (const auto &member : struct_member_types[identifier]) {
            std::string member_identifier = identifier + "." + member.first;

            VariableInfo memberVar;
            memberVar.type = member.second;
            memberVar.stack_location = push_stack(1);

            symbol_table_stack_.top()[member_identifier] = memberVar;
        }
    }
}



void
Context::allocate_stack_array (std::string identifier, int &size)
{
  int bytes = 0;

  TypeSpecifier type_ = current_type_;
  int size_base_type = getBytes (type_);

  VariableInfo var;

  var.type = type_;
  var.isArray = true;

  if (size != -1)
    {
      bytes = size * size_base_type;
      init_arr_loc_base_ = push_stack (bytes);
      var.stack_location = init_arr_loc_base_;
      var.array_sizes.push_back (size);
    }
  else
    {
      var.array_sizes.push_back (size);
    }

  symbol_table_stack_.top ()[identifier] = var;
}

// instructions maker
std::string
Context::get_load_instr (TypeSpecifier &type)
{

  switch (type)
    {
    case TypeSpecifier::INT:
    case TypeSpecifier::UNSIGNED_LONG:
    case TypeSpecifier::UNSIGNED_INT:
      return "lw";
    case TypeSpecifier::FLOAT:
      return "flw";
    case TypeSpecifier::DOUBLE:
      return "fld";
    case TypeSpecifier::CHAR:
      return "lb";
    case TypeSpecifier::UNSIGNED_CHAR:
      return "lbu";
    default:
      throw std::runtime_error ("Context::get_load_instr() - undefined type");
      break;
    }
}

std::string
Context::get_store_instr (TypeSpecifier &type)
{

  switch (type)
    {
    case TypeSpecifier::INT:
    case TypeSpecifier::UNSIGNED_LONG:
    case TypeSpecifier::UNSIGNED_INT:
      return "sw";
    case TypeSpecifier::FLOAT:
      return "fsw";
    case TypeSpecifier::DOUBLE:
      return "fsd";
    case TypeSpecifier::CHAR:
    case TypeSpecifier::UNSIGNED_CHAR:
      return "sb";
    default:
      throw std::runtime_error ("Context::get_store_instr() - undefined type");
      break;
    }
}

void
Context::load (std::ostream &stream, std::string &identifier_,
               std::string &dest_reg, std::string addr_reg)
{

  try
    {
      int enumValue = findEnumVal (identifier_);
      stream << "li " << dest_reg << ", " << enumValue << std::endl;
      return; // im hoping dest_reg will be of int type.....
    }
  catch (const std::runtime_error &)
    {
      // Not an enum, continue normal lookup
    }
  VariableInfo var_info = findSymbol (identifier_);

  TypeSpecifier type_ = var_info.type;
  std::string load_instr = "lw";
  if (!var_info.isPointer)
    {
      load_instr = get_load_instr (type_);
    }
  if (var_info.scope == SCOPE_::GLOBAL)
    {
      if (var_info.isArray)
        {
          throw std::runtime_error (
              "load() - TODO: Implement gobal arrays load");
        }
      else
        {
          stream << "lui " << dest_reg << ",%hi(" << identifier_ << ")"
                 << std::endl;
          stream << load_instr << " " << dest_reg << ",%lo(" << identifier_
                 << ")(" << dest_reg << ")" << std::endl;
        }
    }
  else
    {
      if (var_info.isArray)
        {
          stream << "# its not an array is it?" << std::endl;
          if (addr_reg.empty ())
            {
              stream << "addi " << dest_reg << ",s0,"
                     << var_info.stack_location << std::endl;
            }
          else
            {
              stream << load_instr << " " << dest_reg << ",0(" << addr_reg
                     << ")" << std::endl;
            }
        }
      else
        {
          int loc;
          loc = var_info.stack_location;
          stream << load_instr << " " << dest_reg << "," << loc << "(s0)"
                 << std::endl;
        }
    }
}

void
Context::store (std::ostream &stream, std::string &identifier_,
                std::string &dest_reg, std::string addr_reg)
{

  VariableInfo var_info = findSymbol (identifier_);

  TypeSpecifier type_ = var_info.type;
  std::string store_instr = "sw";
  if (!var_info.isPointer)
    {
      store_instr = get_store_instr (type_);
    }
  if (var_info.scope == SCOPE_::GLOBAL)
    {
      if (var_info.isArray)
        {
          throw std::runtime_error (
              "load() - TODO: Implement gobal arrays store");
        }
      else
        {
          std::string addr_temp = allocate_reg (type_);
          stream << "lui " << addr_temp << ",%hi(" << identifier_ << ")"
                 << std::endl;
          stream << store_instr << " " << dest_reg << ",%lo(" << identifier_
                 << ")(" << addr_temp << ")" << std::endl;
          deallocate_reg (addr_temp);
        }
    }
  else
    {
      if (var_info.isArray)
        {
          stream << store_instr << " " << dest_reg << ",0(" << addr_reg << ")"
                 << std::endl;
        }
      else
        {
          int loc;
          loc = var_info.stack_location;
          stream << store_instr << " " << dest_reg << "," << loc << "(s0)"
                 << std::endl;
        }
    }
}

bool
Context::isInitialised (std::string &identifier) const
{
  try
    {
      findEnumVal (identifier);
      return true;
      // first look intooooooo enums.
    }
  catch (const std::runtime_error &)
    {
      // Not an enum, continue normal lookup
    }
  auto temp_stack = symbol_table_stack_;

  while (!temp_stack.empty ())
    {
      const auto &current_symbol_table = temp_stack.top ();
      if (current_symbol_table.find (identifier)
          != current_symbol_table.end ())
        {
          return true;
        }
      temp_stack.pop ();
    }
  return false;
}
/*
isInitialised uses nearly the same logic as "findSymbol", we cannot reuse
findSymbol as it throws an error if not found -> is there a way to optimise
it?
*/

std::string
Context::makeUniqueLabel (std::string prefix)
{
  int count = ++label_map[prefix];
  return prefix + std::to_string (count);
}

std::string
Context::makeUniqueConstLabel (std::string prefix, uint32_t ieee754_value)
{
  std::string label = prefix + std::to_string (fconst_count_++);
  const_label_[label] = ieee754_value;
  return label;
}

std::string
Context::makeUniqueDoubleConstLabel (std::string prefix, uint32_t lower,
                                     uint32_t upper)
{
  std::string label = prefix + std::to_string (fconst_count_++);
  double_const_label_[label] = std::make_pair (lower, upper);
  ;
  return label;
}

std::string
Context::makeUniqueStringLabel (std::string prefix, std::string string)
{
  std::string label = prefix + std::to_string (fconst_count_++);
  string_label_[label] = string;
  return label;
}

void
Context::emitLabels (std::ostream &stream)
{

  stream << ".section     .rodata" << std::endl;
  stream << ".align 2" << std::endl;
  // Emit floats
  for (auto &[label, float_bits] : const_label_)
    {
      stream << label << ":" << std::endl;
      stream << ".word " << float_bits << "\n";
    }

  // Emit doubles
  for (auto &[label, value] : double_const_label_)
    {
      stream << label << ":" << std::endl;

      stream << ".long " << value.first << std::endl;
      stream << ".long " << value.second << std::endl;
    }
  //                                    _  () _
  // Idek what im doing but it worked so \-|-/

  for (auto &[label, string] : string_label_)
    {
      stream << label << ":" << std::endl;
      stream << ".string " << "\"" << string << "\"" << std::endl;
    }
}

void
Context::push_scope ()
{
  scope_.push (SCOPE_::LOCAL);
  symbol_table_stack_.push (symbol_table_ ());
  enum_stack.push (enums ());
  enum_types_stack_.push (enum_types_ ());
  scope_index++;
}

void
Context::pop_smoke ()
{
  scope_.pop ();
  symbol_table_stack_.pop ();
  enum_stack.pop ();
  enum_types_stack_.pop ();
  scope_index--;
}

bool
Context::isInitialisedinScope (std::string &identifier) const
{
  auto it = symbol_table_stack_.top ().find (identifier);
  if (it != symbol_table_stack_.top ().end ())
    {
      return true;
    }
  return false;
}

// Return by VALUE instead of pointer
VariableInfo
Context::findSymbol (const std::string &identifier) const
{

  // this is crucial for "getType()", during identifier, as if an enum, we
  // will look for synbol here first
  try
    {
      findEnumVal (identifier);
      VariableInfo enumVar;
      enumVar.type = TypeSpecifier::INT; // Enums are always ints
      enumVar.stack_location = -1;       // Enums do not have memory locations
      return enumVar;
    }
  catch (const std::runtime_error &)
    {
      // Not an enum, continue normal lookup
    }

  auto temp_stack = symbol_table_stack_;

  while (!temp_stack.empty ())
    {
      const auto &current_symbol_table = temp_stack.top ();
      auto it = current_symbol_table.find (identifier);
      if (it != current_symbol_table.end ())
        {
          return it->second;
        }
      temp_stack.pop ();
    }

  throw std::runtime_error ("findSymbol() - Symbol not found vv" + identifier);
}

void
Context::add_function_declaration_type (const std::string &identifier,
                                        TypeSpecifier type)
{
  if (symbol_table_stack_.empty ())
    {
      throw std::runtime_error (
          "add_function_declaration_type: No symbol table available");
    }
  // Assume the current symbol table is at the top.
  auto &current_symbol_table = symbol_table_stack_.top ();
  auto it = current_symbol_table.find (identifier);
  if (it == current_symbol_table.end ())
    {
      throw std::runtime_error ("add_function_declaration_type: Function \""
                                + identifier + "\" not found in symbol table");
    }
  it->second.parameter_types.push_back (type);
}

void
Context::set_is_pointer (bool isPointer, const std::string &identifier)
{
  if (symbol_table_stack_.empty ())
    {
      throw std::runtime_error ("set_is_pointer: No symbol table available");
    }
  auto &current_symbol_table = symbol_table_stack_.top ();
  auto it = current_symbol_table.find (identifier);
  if (it == current_symbol_table.end ())
    {
      throw std::runtime_error ("set_is_pointer: Symbol \"" + identifier
                                + "\" not found in symbol table");
    }
  it->second.isPointer = isPointer;
}

// bool
// Context::get_is_pointer (const std::string &identifier) const
// {
//   std::stack<std::unordered_map<std::string, VariableInfo> >
//   temp_symbol_stack
//       = symbol_table_stack_;
//   std::stack<std::unordered_map<std::string, int> > temp_enum_stack
//       = enum_stack;

//   while (!temp_symbol_stack.empty () && !temp_enum_stack.empty ())
//     {
//       const auto &current_enum_scope = temp_enum_stack.top ();
//       if (current_enum_scope.find (identifier) != current_enum_scope.end
//       ())
//         {
//           return false;
//         }

//       const auto &current_symbol_table = temp_symbol_stack.top ();
//       auto it = current_symbol_table.find (identifier);
//       if (it != current_symbol_table.end ())
//         {
//           return it->second.isPointer;
//         }

//       temp_symbol_stack.pop ();
//       temp_enum_stack.pop ();
//     }

//   // If we exhaust all scopes without finding the identifier
//   throw std::runtime_error ("get_is_pointer: Symbol \"" + identifier
//                             + "\" not found in any scope or enum");
// }

bool
Context::get_is_pointer (const std::string &identifier) const
{
  // Copy the symbol table stack so we don't modify the real one
  std::stack<std::unordered_map<std::string, VariableInfo> > temp_stack
      = symbol_table_stack_;

  // Traverse each scope from top (most recent) to bottom
  while (!temp_stack.empty ())
    {
      const auto &current_symbol_table = temp_stack.top ();
      auto it = current_symbol_table.find (identifier);
      if (it != current_symbol_table.end ())
        {
          // Found the symbol; return the isPointer flag
          return it->second.isPointer;
        }
      // Not in this scope; pop and check the next
      temp_stack.pop ();
    }

  // If we exhaust all scopes without finding the symbol, return false
  return false;
}

void
Context::AddFunctionDeclaration (std::string &identifier)
{
  VariableInfo function;
  function.stack_location = -1;
  function.isArray = false;
  function.isPointer = false;
  function.type = current_type_;

  symbol_table_stack_.top ().insert ({ identifier, function });
}

void
Context::store_regs (std::ostream &stream, std::string &dest_reg)
{

  for (int i = 1; i < 12; i++)
    {
      std::string reg_name = "s" + std::to_string (i);

      if (register_map.find (reg_name) != register_map.end ())
        {
          int id = register_map.at (reg_name);

          if (registers[id] == 1 && reg_name != dest_reg)
            {
              int offset = push_stack (4);

              stream << "sw " << reg_name << "," << offset << "(s0)"
                     << std::endl;
              deallocate_reg (reg_name);
            }
        }
    }

  for (int i = 0; i < 12; i++)
    {
      std::string fp_reg_name = "fs" + std::to_string (i);

      if (f_register_map.find (fp_reg_name) != f_register_map.end ())
        {
          int fid = f_register_map.at (fp_reg_name);

          if (f_registers[fid] == 1 && fp_reg_name != dest_reg)
            {
              int offset = push_stack (8);
              stream << "fsw " << fp_reg_name << "," << offset << "(s0)"
                     << std::endl;
              deallocate_reg (fp_reg_name);
            }
        }
    }
}

void
Context::addGlobalVar (std::string &identifier)
{
  VariableInfo var;
  var.scope = SCOPE_::GLOBAL;
  var.isPointer = false;
  var.isArray = false;
  var.type = current_type_;

  symbol_table_stack_.top ()[identifier] = var;
}

SCOPE_
Context::getVarScope (std::string &identifier)
{
  return findSymbol (identifier).scope;
}

void
Context::addEnumVal (std::string identifier, int &value)
{
  if (isEnumValInScope (identifier))
    {
      throw std::runtime_error ("addEnumVal() error - " + identifier
                                + " is already declared within Scope.");
    }
  enum_stack.top ()[identifier] = value;
}

bool
Context::isEnumValInScope (std::string &identifier) const
{
  return enum_stack.top ().count (identifier) > 0;
}

int
Context::findEnumVal (const std::string &identifier) const
{
  auto temp_stack = enum_stack;

  while (!temp_stack.empty ())
    {
      const auto &current_scope = temp_stack.top ();
      auto it = current_scope.find (identifier);
      if (it != current_scope.end ())
        {
          return it->second;
        }
      temp_stack.pop ();
    }

  throw std::runtime_error ("findEnumVal() - Identifier not found");
}

void
Context::addEnumType (std::string identifier)
{
  if (isEnumTypeInScope (identifier))
    {
      throw std::runtime_error (
          "addEnumType() - Type already declared within scope");
    }
  enum_types_stack_.top ().insert (identifier);
}

bool
Context::isEnumTypeInScope (std::string &identifier) const
{
  return enum_types_stack_.top ().count (identifier) > 0;
}

void
Context::enterSwitch ()
{
  push_scope ();
  cases_map_stack_.push (cases_map_ ());
}

void
Context::exitSwitch ()
{
  cases_map_stack_.pop ();
  pop_smoke ();
}

void
Context::addCase (int value, const std::string &label)
{
  cases_map_stack_.top ()[value] = label;
}

cases_map_
Context::getCurrentCaseMap ()
{
  return cases_map_stack_.top ();
}

std::string
Context::getLabelCase (int &val_)
{
  auto it = cases_map_stack_.top ().find (val_);
  if (it != cases_map_stack_.top ().end ())
    {
      return it->second;
    }

  throw std::runtime_error ("getLabelCase() - label not found, value: "
                            + val_);
}

void
Context::setEndSwitch (const std::string &label)
{
  end_switch_label_ = label;
}

void
Context::setDefaultSwitch (const std::string &label)
{
  default_switch_label_ = label;
}

std::string
Context::getEndSwitch ()
{
  return end_switch_label_;
}

std::string
Context::getDefaultSwitch ()
{
  return default_switch_label_;
}

void
Context::setPointerElementSize (const std::string &identifier, int size)
{
  if (symbol_table_stack_.empty ())
    {
      throw std::runtime_error (
          "setPointerElementSize: No symbol table available");
    }

  // Look at the top (current) symbol table
  auto &current_symbol_table = symbol_table_stack_.top ();
  auto it = current_symbol_table.find (identifier);
  if (it == current_symbol_table.end ())
    {
      throw std::runtime_error ("setPointerElementSize: Symbol \"" + identifier
                                + "\" not found in symbol table");
    }
  // Store the size
  it->second.pointerElementSize = size;
}

int
Context::getPointerElementSize (const std::string &identifier) const
{
  // We do a stack copy and search each level, just like get_is_pointer
  std::stack<std::unordered_map<std::string, VariableInfo> > temp_stack
      = symbol_table_stack_;

  while (!temp_stack.empty ())
    {
      const auto &current_symbol_table = temp_stack.top ();
      auto it = current_symbol_table.find (identifier);
      if (it != current_symbol_table.end ())
        {
          return it->second.pointerElementSize;
        }
      temp_stack.pop ();
    }

  throw std::runtime_error ("getPointerElementSize: Symbol \"" + identifier
                            + "\" not found in symbol table");



}

void
Context::setDestReg (std::string &dest_reg)
{
  dest_reg_ = dest_reg;
}

std::string
Context::getDestReg ()
{
  return dest_reg_;
}

void Context::clearStructMembers()
{
    struct_member_types.clear();
    struct_member_offsets.clear();
}

void Context::defineStruct(const std::string &name, TypeSpecifier type)
{
    struct_definitions[name] = type;
}

int Context::get_stack_location(const std::string &identifier) const {
    auto it = symbol_table_stack_.top().find(identifier);
    if (it != symbol_table_stack_.top().end()) {
        return it->second.stack_location;
    }
    throw std::runtime_error("Stack location not found for identifier: " + identifier);
}

bool Context::hasStruct(const std::string &name) const {
    return struct_definitions.find(name) != struct_definitions.end();
}

void Context::setStructInstance(const std::string &name) {
    struct_instances[name] = name;
}

std::string Context::getStructInstance(const std::string &name) const {
    auto it = struct_instances.find(name);
    if (it != struct_instances.end()) {
        return it->second;
    }
    throw std::runtime_error("Struct instance not found for name: " + name);
}

bool Context::has_stack_entry(const std::string &identifier) const {
    return symbol_table_stack_.top().find(identifier) != symbol_table_stack_.top().end();
}
} // namespace ast

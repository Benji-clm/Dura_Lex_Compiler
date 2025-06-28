#include "array/ast_array_index.hpp"
#include <cmath>

namespace ast
{
void
ArrayIndex::EmitRISC (std::ostream &stream, Context &context,
                      std::string &dest_reg) const
{

  std::string identifier = identifier_->getIdentifier ();
  TypeSpecifier type = context.getType (identifier);
  stream << "# " << type << std::endl;

  bool is_pointer = context.get_is_pointer (identifier);

  context.setArrayIndex (true);
  index_->EmitRISC (stream, context, dest_reg);
  context.setArrayIndex (false);
  //   addr_reg only contains the value of the index for the moment

  int bytes = context.getBytes (type);
  std::string loc_base = "";
  std::string add_instr = "addi";

  if (is_pointer)
    {
      loc_base = context.allocate_reg (type);
      add_instr = "add";
      context.load (stream, identifier, loc_base);
    }
  else
    {
      loc_base = std::to_string (context.getLoc (identifier));
    }

  stream << "slli " << dest_reg << "," << dest_reg << "," << std::log2 (bytes)
         << std::endl;

  stream << add_instr << " " << dest_reg << "," << dest_reg << "," << loc_base
         << std::endl;

  if (is_pointer)
    {
      context.deallocate_reg (loc_base);
      std::string load_inst = context.get_load_instr (type);
      stream << load_inst << " " << dest_reg << ",0(" << dest_reg << ")"
             << std::endl;
      stream << "# do we get here" << std::endl;
    }
  else
    {
      stream << "add " << dest_reg << ",s0," << dest_reg << std::endl;
      // from this point on the location of the wanted array item is within
      // dest_reg
      if (!context.isLeftOp ())
        {
          context.load (stream, identifier, dest_reg, dest_reg);
        }
    }

  //  If we have something like "x[0] = 3" this returns the address within the
  //  dest_reg, if we have something like y = x[0], it will then load into dest
  //  reg the value of x[0]
}

void
ArrayIndex::EmitADDRESS (std::ostream &stream, Context &context,
                         std::string &dest_reg) const
{
  std::string identifier = identifier_->getIdentifier ();
  TypeSpecifier type = context.getType (identifier);

  context.setArrayIndex (true);
  index_->EmitRISC (stream, context, dest_reg);
  context.setArrayIndex (false);
  //   addr_reg only contains the value of the index for the moment

  int bytes = context.getBytes (type);

  std::string loc_base = context.allocate_reg (type);
  context.load (stream, identifier, loc_base);

  stream << "slli " << dest_reg << "," << dest_reg << "," << std::log2 (bytes)
         << std::endl;

  stream << "add " << dest_reg << "," << dest_reg << "," << loc_base
         << std::endl;

  //
  // std::string load_inst = context.get_load_instr (type);
  // stream << load_inst << " " << dest_reg << ",0(" << dest_reg << ")"
  //         << std::endl;

  context.deallocate_reg (loc_base);
}

void
ArrayIndex::Print (std::ostream &stream) const
{
  identifier_->Print (stream);
  index_->Print (stream);
}
}

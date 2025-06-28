#include "ast_identifier.hpp"
#include <cmath>

namespace ast
{

void
Identifier::EmitRISC (std::ostream &stream, Context &context,
                      std::string &dest_reg) const
{
  std::string id = identifier_;
  if (context.isScopeGlobal ())
    {
      TypeSpecifier type = context.getCurrentType ();
      int size = context.getBytes (type);

      stream << ".globl " << id << std::endl;
      if (!context.isGloblInitialized ())
        {
          stream << ".section .sbss,\"aw\",@nobits" << std::endl;
        }
      else
        {
          stream << ".section .sdata,\"aw\"" << std::endl;
        }
      stream << ".align " << log2 (size) << std::endl;
      stream << ".type " << id << ", @object" << std::endl;
      stream << ".size " << id << ", " << size << std::endl;
    }
  else if (context.isLeftOp () && !context.isArrayIndex ()
           && !context.isCurrentPointer ())
    {
    }
  else if (context.isInitialised (id))
    {
      context.load (stream, id, dest_reg);
    }
}

void
Identifier::Print (std::ostream &stream) const
{
  stream << identifier_;
};

void
Identifier::EmitADDRESS (std::ostream &stream, Context &context,
                         std::string &dest_reg) const
{
  int address = context.getLoc (identifier_);
  stream << "addi " << dest_reg << ",s0," << address << std::endl;
}

std::string
Identifier::getIdentifier () const
{
  return identifier_;
}

}
// clang-format off
/*
|    Section    |                        Use Case                              |
|---------------|--------------------------------------------------------------|
| **`.bss`**    | Uninitialized global/static variables (default for `int x;`) |
| **`.data`**   | Initialized global/static variables (`int x = 5;`)           |
| **`.sdata`**  | Small initialized global/static variables (≤ 8 bytes)        |
| **`.sbss`**   | Small uninitialized global/static variables (≤ 8 bytes)      |
| **`.rodata`** | Read-only data (`const int x = 5;`)                          |
*/
// clang-format on

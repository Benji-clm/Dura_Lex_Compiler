#include "primitives/ast_char.hpp"

namespace ast
{
void
CharConstant::EmitRISC (std::ostream &stream, Context &,
                        std::string &dest_reg) const
{
  stream << "li " << dest_reg << "," << char_ << std::endl;
}

void
CharConstant::Print (std::ostream &stream) const
{
  stream << "'" << char (char_) << "'" << std::endl;
}
}

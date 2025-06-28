#include "primitives/ast_string.hpp"

namespace ast
{
void
String::EmitRISC (std::ostream &stream, Context &context,
                  std::string &dest_reg) const
{
  std::string label = context.makeUniqueStringLabel (".LC", string_);

  stream << "lui " << dest_reg << ",%hi(" << label << ")" << std::endl;
  stream << "addi " << dest_reg << "," << dest_reg << ",%lo(" << label << ")"
         << std::endl;
}

void
String::Print (std::ostream &stream) const
{
  stream << string_ << std::endl;
}
}

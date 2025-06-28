#include "misc/ast_break.hpp"

namespace ast
{
void
Break::EmitRISC (std::ostream &stream, Context &context, std::string &) const
{
  stream << "j " << context.getBreakLabel () << std::endl;
}

void
Break::Print (std::ostream &stream) const
{
  stream << "break" << std::endl;
}
}

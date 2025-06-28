#include "array/ast_array_declarator.hpp"

namespace ast
{
void
ArrayDeclarator::EmitRISC (std::ostream &, Context &context,
                           std::string &) const
{
  int size = -1;
  if (size_array_ != nullptr)
    {
      size = size_array_->getValue ();
    }

  std::string identifier = identifier_->getIdentifier ();

  context.allocate_stack_array (identifier, size);
}

/*
IMPORTANT NOTE:

In C90, arrays can only take "constant" as their sizes, such as:
 int x[4], x[3 + 2], or a value from a #define, only from C99 onwards can you
use variables, this means that WE KNOW THE SIZE AT COMPILE TIME.

*/

void
ArrayDeclarator::Print (std::ostream &stream) const
{
  identifier_->Print (stream);
  stream << "[";
  size_array_->Print (stream);
  stream << "]" << std::endl;
}
}

#include "ast_size_of.hpp"
#include <vector>

namespace ast
{
void
SizeOf::EmitRISC (std::ostream &stream, Context &context,
                  std::string &dest_reg) const
{
  TypeSpecifier type = unary_expression_->getType (context);
  int bytes = context.getBytes (type);
  int size = 0;
  if (unary_expression_->isArray ())
    {
      std::string identifier = unary_expression_->getIdentifier ();
      std::vector<int> array_sizes
          = context.findSymbol (identifier).array_sizes;

      for (size_t i = 0; i < array_sizes.size (); i++)
        {
          size = size * bytes * array_sizes[i];
        }
    }
  else
    {
      size = bytes;
    }

  stream << "li " << dest_reg << "," << size << std::endl;
}

void
SizeOf::Print (std::ostream &stream) const
{
  stream << "size of: ";
  unary_expression_->Print (stream);
}
}

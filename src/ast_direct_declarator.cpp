#include "ast_direct_declarator.hpp"

namespace ast
{

void
DirectDeclarator::EmitRISC (std::ostream &stream, Context &context,
                            std::string &dest_reg) const
{
  identifier_->EmitRISC (stream, context, dest_reg);
}

void
DirectDeclarator::Print (std::ostream &stream) const
{
  identifier_->Print (stream);
}

} // namespace ast

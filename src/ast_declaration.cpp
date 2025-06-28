#include "ast_declaration.hpp"

namespace ast
{

void
Declaration::EmitRISC (std::ostream &stream, Context &context,
                       std::string &) const
{
  TypeSpecifier type = declaration_specifier->getType (context);
  context.setCurrentType (type);

  std::string tmp_reg = context.allocate_reg (type);

  if (init_declarator_list != nullptr)
    {
      init_declarator_list->EmitRISC (stream, context, tmp_reg);
    }

  context.deallocate_reg (tmp_reg);
}

void
Declaration::Print (std::ostream &stream) const
{
  stream << "Declaration\n"; // Indicate it's a declaration node
  if (init_declarator_list != nullptr)
    {
      init_declarator_list->Print (stream);
    }
}

} // namespace ast

#include "ast_scope.hpp"

namespace ast
{
void
Scope::EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const
{
  context.push_scope ();
  //   stream << "scope scope lol" << std::endl;
  if (declaration_list != nullptr)
    {
      declaration_list->EmitRISC (stream, context, dest_reg);
    }

  if (statement_list != nullptr)
    {
      statement_list->EmitRISC (stream, context, dest_reg);
    }

  context.pop_smoke ();
}

void
Scope::Print (std::ostream &stream) const
{
  stream << "{\n";

  if (declaration_list != nullptr)
    {
      declaration_list->Print (stream);
    }

  if (statement_list != nullptr)
    {
      statement_list->Print (stream);
    }

  stream << "}\n";
}

}

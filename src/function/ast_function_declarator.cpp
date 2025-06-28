#include "function/ast_function_declarator.hpp"

namespace ast
{
void
FunctionDeclarator::EmitRISC (std::ostream &stream, Context &context,
                              std::string &dest_reg) const
{

  if (parameter_list != nullptr && context.isFunctionDefinition ())
    {
      context.resetParamFIndex ();
      context.resetParamAIndex ();
      parameter_list->EmitRISC (stream, context, dest_reg);
    }
}

void
FunctionDeclarator::Print (std::ostream &stream) const
{
  if (parameter_list != nullptr)
    {
      parameter_list->Print (stream);
    }
}
}

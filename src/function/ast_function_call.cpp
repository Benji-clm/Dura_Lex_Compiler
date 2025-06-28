#include "function/ast_function_call.hpp"

namespace ast
{
void
FunctionCall::EmitRISC (std::ostream &stream, Context &context,
                        std::string &dest_reg) const
{
  context.initalize_arg_regs ();
  context.resetParamFIndex ();
  context.resetParamAIndex ();
  if (argument_expression_list_ != nullptr)
    {
      argument_expression_list_->EmitRISC (stream, context, dest_reg);
    }

  std::string identifier = identifier_->getIdentifier ();
  stream << "call " << identifier << std::endl;
  stream << "mv " << dest_reg << "," << "a0" << std::endl;
}

void
FunctionCall::Print (std::ostream &stream) const
{
  if (argument_expression_list_ != nullptr)
    {
      argument_expression_list_->Print (stream);
    }
}
}

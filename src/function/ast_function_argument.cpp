#include "function/ast_function_argument.hpp"

namespace ast
{

void
FunctionArgument::EmitRISC (std::ostream &stream, Context &context,
                            std::string &) const
{
  TypeSpecifier type = argument_->getType (context);
  std::string reg = context.allocate_arg_reg (type);

  argument_->EmitRISC (stream, context, reg);
}

void
FunctionArgument::Print (std::ostream &stream) const
{
  stream << "(";
  argument_->Print (stream);
  stream << ")" << std::endl;
}
}

#include "function/ast_function_parameter.hpp"

namespace ast
{
void
FunctionParameter::EmitRISC (std::ostream &stream, Context &context,
                             std::string &) const
{
  TypeSpecifier type = declaration_specifier_->getType (context);
  context.setCurrentType (type);
  std::string identifier_ = declarator_->getIdentifier ();

  if (declarator_->isPointer ())
    {
      context.setPointerInit (true);
    }
  else
    {
      context.setPointerInit (false);
    }

  std::string arg_reg;
  if ((type == TypeSpecifier::FLOAT) || (type == TypeSpecifier::DOUBLE))
    {
      arg_reg = "fa" + std::to_string (context.getParamFIndex ());
      context.incrementfArgIndex ();
    }
  else
    {
      arg_reg = "a" + std::to_string (context.getParamAIndex ());
      context.incrementaArgIndex ();
    }

  context.allocate_stack (identifier_, type);
  context.store (stream, identifier_, arg_reg);
  context.deallocate_reg (arg_reg);
}

void
FunctionParameter::Print (std::ostream &stream) const
{
  declaration_specifier_->Print (stream);
  declarator_->Print (stream);
}
}

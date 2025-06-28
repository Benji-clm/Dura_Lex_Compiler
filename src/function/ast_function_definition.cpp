#include "function/ast_function_definition.hpp"

namespace ast
{

void
FunctionDefinition::EmitRISC (std::ostream &stream, Context &context,
                              std::string &) const
{
  context.setFunctionDefinition (true);
  context.initalize_regs ();

  TypeSpecifier type = declaration_specifiers_->getType (context);

  std::string f_name = declarator_->getIdentifier ();
  // Emit assembler directives.
  // TODO: these are just examples ones, make sure you understand
  // the concept of directives and correct them.
  stream << ".text" << std::endl;
  stream << ".globl " << f_name << std::endl;

  stream << f_name << ":" << std::endl;

  context.setCurrentType (type);

  std::string ret_reg = "";
  //   In RISC-V, unless we have a floating point, the return register
  //   should always be a0.
  switch (type)
    {

    case TypeSpecifier::FLOAT:
    case TypeSpecifier::DOUBLE:
      ret_reg = "fa0";
      break;
    case TypeSpecifier::VOID:
    case TypeSpecifier::UNSIGNED_INT:
    case TypeSpecifier::INT:
    default:
      ret_reg = "a0";
      break;
    }

  context.init_stack (stream);
  context.push_scope ();

  // declarator (contains parameters)
  declarator_->EmitRISC (stream, context, ret_reg);

  if (ret_reg != "")
    {
      context.allocate_reg_manual (ret_reg, type);
      context.setDestReg (ret_reg);
    }

  if (compound_statement_ != nullptr)
    {
      compound_statement_->EmitRISC (stream, context, ret_reg);
    }

  if (type == TypeSpecifier::VOID)
    {
      context.end_stack (stream);
      stream << "ret" << std::endl;
    }

  context.pop_smoke ();
  context.setFunctionDefinition (false);
  context.deallocate_reg (ret_reg);
}

void
FunctionDefinition::Print (std::ostream &stream) const
{
  declaration_specifiers_->Print (stream);

  declarator_->Print (stream);
  stream << "() {" << std::endl;

  if (compound_statement_ != nullptr)
    {
      compound_statement_->Print (stream);
    }
  stream << "}" << std::endl;
}

}

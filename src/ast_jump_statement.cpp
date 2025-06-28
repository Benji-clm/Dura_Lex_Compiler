#include "ast_jump_statement.hpp"

namespace ast
{

void
ReturnStatement::EmitRISC (std::ostream &stream, Context &context,
                           std::string &dest_reg) const
{
  if (expression_ != nullptr)
    {
      expression_->EmitRISC (stream, context, dest_reg);
    }

  std::string ret_reg = context.getDestReg ();
  if (ret_reg != dest_reg && ret_reg != "")
    {
      stream << "mv " << ret_reg << "," << dest_reg << std::endl;
    }

  context.end_stack (stream);
  stream << "ret" << std::endl;
}

void
ReturnStatement::Print (std::ostream &stream) const
{
  stream << "return";
  if (expression_ != nullptr)
    {
      stream << " ";
      expression_->Print (stream);
    }
  stream << ";" << std::endl;
}

}

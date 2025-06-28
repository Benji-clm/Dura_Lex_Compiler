#include "conditional/ast_do_while.hpp"

namespace ast
{
void
DoWhile::EmitRISC (std::ostream &stream, Context &context, std::string &) const
{
  std::string start_label = context.makeUniqueLabel (".L");
  std::string condition_label = context.makeUniqueLabel (".L");
  std::string end_label = context.makeUniqueLabel (".L");

  context.pushBreakLabel (end_label);
  context.pushContinueLabel (condition_label);

  TypeSpecifier type = statement_->getType (context);
  std::string tmp_reg = context.allocate_reg (type);

  stream << start_label << ":" << std::endl;
  if (statement_ != nullptr)
    {
      statement_->EmitRISC (stream, context, tmp_reg);
    }

  stream << condition_label << ":" << std::endl;

  type = expression_->getType (context);
  std::string cond_reg = context.allocate_reg (type);

  if (expression_ != nullptr)
    {
      expression_->EmitRISC (stream, context, cond_reg);
    }

  stream << "bnez " << cond_reg << ", " << start_label << std::endl;

  stream << end_label << ":" << std::endl;

  context.popContinueLabel ();
  context.popBreakLabel ();
  context.deallocate_reg (cond_reg);
  context.deallocate_reg (tmp_reg);
}

void
DoWhile::Print (std::ostream &stream) const
{
  stream << "Do:" << std::endl;
  if (statement_ != nullptr)
    {
      statement_->Print (stream);
    }

  stream << "While:" << std::endl;

  if (expression_ != nullptr)
    {
      expression_->Print (stream);
    }
}
}

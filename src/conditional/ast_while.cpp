#include "conditional/ast_while.hpp"

namespace ast
{
void
While::EmitRISC (std::ostream &stream, Context &context, std::string &) const
{
  // In C90, a while function, like most statements, is not expected to produce
  // a final value in a destination register, hence we DO NOT NEED TO USE
  // DEST_REG, as it can cause register missmatch if dest_reg is a float reg.
  std::string start_label = context.makeUniqueLabel (".L");
  std::string condition_label = context.makeUniqueLabel (".L");

  std::string end_label = context.makeUniqueLabel (".L");
  context.pushBreakLabel (end_label);
  context.pushContinueLabel (condition_label);

  stream << "j " << condition_label << std::endl;

  TypeSpecifier type = statement_->getType (context);
  std::string tmp_reg = context.allocate_reg (type);

  stream << start_label << ":" << std::endl;

  if (statement_ != nullptr)
    {
      statement_->EmitRISC (stream, context, tmp_reg);
    }

  type = condition_->getType (context);
  std::string cond_reg = context.allocate_reg (type);
  stream << condition_label << ":" << std::endl;
  if (condition_ != nullptr)
    {
      condition_->EmitRISC (stream, context, cond_reg);
    }

  stream << "bnez " << cond_reg << "," << start_label << std::endl;

  stream << end_label << ":" << std::endl;

  context.popContinueLabel ();
  context.popBreakLabel ();
  context.deallocate_reg (cond_reg);
  context.deallocate_reg (tmp_reg);
}

void
While::Print (std::ostream &stream) const
{
  stream << "while(";
  if (condition_ != nullptr)
    {
      condition_->Print (stream);
    }

  stream << "){" << std::endl;

  if (statement_ != nullptr)
    {
      statement_->Print (stream);
    }
  stream << "}" << std::endl;
}
}

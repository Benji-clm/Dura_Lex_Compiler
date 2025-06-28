#include "operation/ast_ternary.hpp"

namespace ast
{
void
Ternary::EmitRISC (std::ostream &stream, Context &context,
                   std::string &dest_reg) const
{
  std::string true_label = context.makeUniqueLabel (".L");
  std::string false_label = context.makeUniqueLabel (".L");
  std::string end_label = context.makeUniqueLabel (".L");

  TypeSpecifier cond_type = condition_->getType (context);
  std::string cond_reg = context.allocate_reg (cond_type);

  condition_->EmitRISC (stream, context, cond_reg);

  stream << "beqz " << cond_reg << ", " << false_label << std::endl;

  stream << true_label << ":" << std::endl;
  expressionTrue_->EmitRISC (stream, context, dest_reg);
  stream << "j " << end_label << std::endl;

  stream << false_label << ":" << std::endl;
  expressionFalse_->EmitRISC (stream, context, dest_reg);

  stream << end_label << ":" << std::endl;

  context.deallocate_reg (cond_reg);
}

void
Ternary::Print (std::ostream &stream) const
{
  condition_->Print (stream);
  stream << " ? ";
  expressionTrue_->Print (stream);

  stream << " : ";
  expressionFalse_->Print (stream);
}
}

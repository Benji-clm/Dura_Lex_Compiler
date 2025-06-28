#include "misc/ast_case.hpp"

namespace ast
{

void
Case::EmitRISC (std::ostream &stream, Context &context,
                std::string &dest_reg) const
{
  int val_ = constant_expression_->EvalConstExpr (context);

  stream << context.getLabelCase (val_) << ":" << std::endl;

  statement_->EmitRISC (stream, context, dest_reg);
}

void
Case::Print (std::ostream &stream) const
{
  stream << "case ";
  constant_expression_->Print (stream);
  stream << ":" << std::endl;

  statement_->Print (stream);
}

void
Case::getCases (Context &context) const
{
  int val_ = constant_expression_->EvalConstExpr (context);
  std::string label_ = context.makeUniqueLabel (".L");
  context.addCase (val_, label_);
}

/* --------------- DEFAULT CASE --------------- */

void
DefaultCase::EmitRISC (std::ostream &stream, Context &context,
                       std::string &dest_reg) const
{
  stream << context.getDefaultSwitch () << ":" << std::endl;

  statement_->EmitRISC (stream, context, dest_reg);
}

void
DefaultCase::Print (std::ostream &stream) const
{
  stream << "default:" << std::endl;

  statement_->Print (stream);
}
}

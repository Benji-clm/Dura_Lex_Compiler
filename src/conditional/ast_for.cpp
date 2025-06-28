#include "conditional/ast_for.hpp"

namespace ast
{
void
For::EmitRISC (std::ostream &stream, Context &context,
               std::string &dest_reg) const

{
  std::string statement_label = context.makeUniqueLabel (".L");
  std::string condition_label = context.makeUniqueLabel (".L");

  std::string end_label = context.makeUniqueLabel (".L");
  context.pushBreakLabel (end_label);

  std::string iteration_label = context.makeUniqueLabel (".L");
  context.pushContinueLabel (iteration_label);

  /* make sure to make the labels **before** emitting any RISC -> they might
   * need them*/

  if (initializer_ != nullptr)
    {
      initializer_->EmitRISC (stream, context, dest_reg);
    }

  stream << "j " << condition_label << std::endl;

  //   for loop - firt evaluates the given statement (..) then evaluates the
  //   iteration/increment expression (i.e. "i++")
  stream << statement_label << ":" << std::endl;
  if (statement_ != nullptr)
    {
      statement_->EmitRISC (stream, context, dest_reg);
    }

  stream << iteration_label << ":" << std::endl;

  if (iteration_expression_ != nullptr)
    {
      iteration_expression_->EmitRISC (stream, context, dest_reg);
    }

  stream << condition_label << ":" << std::endl;
  if (condition_ != nullptr)
    {
      TypeSpecifier type = condition_->getType (context);
      context.setCurrentType (type);
      condition_->EmitRISC (stream, context, dest_reg);
      stream << "bnez " << dest_reg << "," << statement_label << std::endl;
    }

  stream << end_label << ":" << std::endl;
  context.popContinueLabel ();
  context.popBreakLabel ();
}

void
For::Print (std::ostream &stream) const
{
  stream << "for(";
  if (initializer_ != nullptr)
    {
      initializer_->Print (stream);
    }

  stream << ";";

  if (condition_ != nullptr)
    {
      condition_->Print (stream);
    }
  stream << ";";

  if (iteration_expression_ != nullptr)
    {
      iteration_expression_->Print (stream);
    }
  stream << "){" << std::endl;

  if (statement_ != nullptr)
    {
      statement_->Print (stream);
    }

  stream << "}" << std::endl;
}
} // namespace ast

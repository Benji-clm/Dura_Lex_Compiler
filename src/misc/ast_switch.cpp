#include "misc/ast_switch.hpp"

namespace ast
{
void
Switch::EmitRISC (std::ostream &stream, Context &context,
                  std::string &dest_reg) const
{
  context.enterSwitch ();

  TypeSpecifier type = TypeSpecifier::INT;

  std::string expr_reg = context.allocate_reg (type);
  expression_->EmitRISC (stream, context, expr_reg);

  statement_->getCases (context);
  cases_map_ labels_map = context.getCurrentCaseMap ();

  std::string tmp_reg = context.allocate_reg (type);
  for (const auto &[value, label] : labels_map)
    {
      stream << "li " << tmp_reg << "," << value << std::endl;
      stream << "beq " << expr_reg << "," << tmp_reg << "," << label
             << std::endl;
    }
  context.deallocate_reg (tmp_reg);

  std::string end_label = context.makeUniqueLabel (".L");
  context.setEndSwitch (end_label);
  context.pushBreakLabel (end_label);

  if (statement_->isDefault ())
    {
      std::string default_label = context.makeUniqueLabel (".L");
      context.setDefaultSwitch (default_label);
      stream << "j " << default_label << std::endl;
    }
  else
    {
      stream << "j " << end_label << std::endl;
    }

  statement_->EmitRISC (stream, context, dest_reg);

  stream << end_label << ":" << std::endl;

  context.deallocate_reg (expr_reg);
  context.exitSwitch ();
  context.popBreakLabel ();
}

void
Switch::Print (std::ostream &stream) const
{
  stream << "switch(";
  expression_->Print (stream);
  stream << ")\n{" << std::endl;

  statement_->Print (stream);

  stream << "}" << std::endl;
}
}

/*
switch is being compiled as:

expression
conditions
labels:
    statement
    (break = jump to the end)
end_label
*/

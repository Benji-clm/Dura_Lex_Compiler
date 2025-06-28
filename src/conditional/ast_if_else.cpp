#include "conditional/ast_if_else.hpp"

namespace ast
{
void
IfElse::EmitRISC (std::ostream &stream, Context &context,
                  std::string &dest_reg) const
{
  std::string end_label = context.makeUniqueLabel (".L");
  std::string else_label;

  TypeSpecifier type = condition_->getType (context);
  std::string cond_reg = context.allocate_reg (type);

  condition_->EmitRISC (stream, context, cond_reg);

  //   technically we do not require an if label, however might help asm
  //   readability?
  if (else_statement_ != nullptr)
    {
      else_label = context.makeUniqueLabel (".L");
      stream << "beq " << cond_reg << ", zero, " << else_label << std::endl;
    }
  else
    {
      stream << "beq " << cond_reg << ", zero, " << end_label << std::endl;
    }

  context.deallocate_reg (cond_reg);

  if_statement_->EmitRISC (stream, context, dest_reg);

  if (else_statement_ != nullptr)
    {
      stream << "j " << end_label << std::endl;
      stream << else_label << ":" << std::endl;
      else_statement_->EmitRISC (stream, context, dest_reg);
    }

  stream << end_label << ":" << std::endl;
}

void
IfElse::Print (std::ostream &stream) const
{
  stream << "if(";
  condition_->Print (stream);
  stream << "){" << std::endl;
  if_statement_->Print (stream);
  stream << "}" << std::endl;
  if (else_statement_ != nullptr)
    {
      stream << "else{" << std::endl;
      else_statement_->Print (stream);
      stream << "}" << std::endl;
    }
}
}

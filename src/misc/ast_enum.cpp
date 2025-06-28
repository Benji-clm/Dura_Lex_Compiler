#include "misc/ast_enum.hpp"

namespace ast
{
void
Enum::EmitRISC (std::ostream &stream, Context &context,
                std::string &dest_reg) const
{

  if (identifier_ != "")
    {
      std::string id = identifier_;
      context.addEnumType (id);
    }

  if (enumerator_ != nullptr)
    {
      enumerator_->EmitRISC (stream, context, dest_reg);
    }
}

void
Enum::Print (std::ostream &stream) const
{
  stream << "enum " << identifier_ << "{" << std::endl;
  if (enumerator_ != nullptr)
    {
      enumerator_->Print (stream);
    }
  stream << "}" << std::endl;
}

void
EnumValue::EmitRISC (std::ostream &, Context &context, std::string &) const
{
  int val = 0;
  if (value_ != nullptr)
    {
      val = value_->EvalConstExpr (context);
      context.addEnumVal (identifier_, val);
    }
  else
    {
      context.addEnumVal (identifier_, val);
    }
}

void
EnumValue::Print (std::ostream &stream) const
{
  stream << identifier_ << " ";
  if (value_ != nullptr)
    {
      value_->Print (stream);
    }
}
}

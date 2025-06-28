#include "ast_constant.hpp"
#include <bit>
#include <cstring>
#include <iomanip>

namespace ast
{

void
IntConstant::EmitRISC (std::ostream &stream, Context &,
                       std::string &dest_reg) const
{

  stream << "li " << dest_reg << "," << value_ << std::endl;
}

void
IntConstant::Print (std::ostream &stream) const
{
  stream << value_;
}

void
FloatDoubleConstant::EmitRISC (std::ostream &stream, Context &context,
                               std::string &dest_reg) const
{
  TypeSpecifier tmp_type = TypeSpecifier::INT;
  std::string tmp_reg = context.allocate_arg_reg (tmp_type);

  if (context.getCurrentType () == TypeSpecifier::FLOAT)
    {
      float val = value_;
      uint32_t ieee754_value;
      std::memcpy (&ieee754_value, &val, sizeof (ieee754_value));

      std::string label = context.makeUniqueConstLabel (".LC", ieee754_value);
      stream << "lui " << tmp_reg << ",%hi(" << label << ")" << std::endl;
      stream << "flw " << dest_reg << ",%lo(" << label << ")(" << tmp_reg
             << ")" << std::endl;
    }
  else if (context.getCurrentType () == TypeSpecifier::DOUBLE)
    {

      uint64_t bits;
      std::memcpy (&bits, &value_, sizeof (uint64_t));

      // Split into two 32-bit halves:
      uint32_t lower_part = static_cast<uint32_t> (bits & 0xFFFFFFFF);
      uint32_t upper_part = static_cast<uint32_t> ((bits >> 32) & 0xFFFFFFFF);

      //   stream << "lower " << lower_part << std::endl;
      //   stream << "lower " << upper_part << std::endl;

      //   this is the most criminal code I've written so far, but I guess it
      //   does the job

      std::string label
          = context.makeUniqueDoubleConstLabel (".LC", lower_part, upper_part);
      stream << "lui " << tmp_reg << ",%hi(" << label << ")" << std::endl;
      stream << "fld " << dest_reg << ",%lo(" << label << ")(" << tmp_reg
             << ")" << std::endl;
    }
  else
    {
      throw std::runtime_error (
          "the hell is going on here (FloatDoubleConstant)");
    }

  context.deallocate_reg (tmp_reg);
}

void
FloatDoubleConstant::Print (std::ostream &stream) const
{
  stream << value_;
}

} // namespace ast

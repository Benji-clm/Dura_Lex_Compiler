#include "operation/ast_pre_increment.hpp"

namespace ast
{
void
PreIncrement::EmitRISC (std::ostream &stream, Context &context,
                        std::string &dest_reg) const
{
  // Get the original value into dest_reg
  unary_expression_->EmitRISC (stream, context, dest_reg);
  std::string identifier_ = unary_expression_->getIdentifier ();
  TypeSpecifier type = context.getType (identifier_);

  // Allocate a temp register to hold the incremented value
  std::string inc_reg = context.allocate_reg (type);

  // Copy the original value to the temp register
  if (type == TypeSpecifier::FLOAT)
    {
      stream << "fmv.s " << inc_reg << "," << dest_reg << std::endl;
    }
  else if (type == TypeSpecifier::DOUBLE)
    {
      stream << "fmv.d " << inc_reg << "," << dest_reg << std::endl;
    }
  else
    {
      stream << "mv " << inc_reg << "," << dest_reg << std::endl;
    }

  if (context.get_is_pointer (identifier_))
    {
      int bytes = context.getBytes (type);
      switch (incr_type_)
        {
        case IncrType::INC_OP:
          stream << "addi " << inc_reg << "," << inc_reg << "," << bytes
                 << std::endl;
          break;
        case IncrType::DEC_OP:
          stream << "addi " << inc_reg << "," << inc_reg << "," << -bytes
                 << std::endl;
          break;
        default:
          throw std::runtime_error (
              "PreIncrement() - unrecognised IncrType operator");
        }
    }
  else
    {
      switch (type)
        {
        case TypeSpecifier::FLOAT:
          {
            TypeSpecifier tmp_type = TypeSpecifier::INT;
            std::string tmp_reg = context.allocate_arg_reg (tmp_type);

            tmp_type = TypeSpecifier::FLOAT;
            std::string tmp_float_reg = context.allocate_arg_reg (tmp_type);

            // Load constant 1.0f
            std::string label
                = context.makeUniqueConstLabel (".LC", 1065353216);
            stream << "lui " << tmp_reg << ",%hi(" << label << ")"
                   << std::endl;
            stream << "flw " << tmp_float_reg << ",%lo(" << label << ")("
                   << tmp_reg << ")" << std::endl;

            switch (incr_type_)
              {
              case IncrType::INC_OP:
                stream << "fadd.s " << inc_reg << "," << inc_reg << ","
                       << tmp_float_reg << std::endl;
                break;
              case IncrType::DEC_OP:
                stream << "fsub.s " << inc_reg << "," << inc_reg << ","
                       << tmp_float_reg << std::endl;
                break;
              default:
                throw std::runtime_error (
                    "PreIncrement() - unrecognised IncrType operator");
              }

            context.deallocate_reg (tmp_reg);
            context.deallocate_reg (tmp_float_reg);
            break;
          }
        case TypeSpecifier::DOUBLE:
          {
            TypeSpecifier tmp_type = TypeSpecifier::INT;
            std::string tmp_reg = context.allocate_arg_reg (tmp_type);

            tmp_type = TypeSpecifier::DOUBLE;
            std::string tmp_float_reg = context.allocate_arg_reg (tmp_type);

            // Load constant 1.0f
            std::string label
                = context.makeUniqueDoubleConstLabel (".LC", 0, 1072693248);
            stream << "lui " << tmp_reg << ",%hi(" << label << ")"
                   << std::endl;
            stream << "fld " << tmp_float_reg << ",%lo(" << label << ")("
                   << tmp_reg << ")" << std::endl;

            switch (incr_type_)
              {
              case IncrType::INC_OP:
                stream << "fadd.d " << inc_reg << "," << inc_reg << ","
                       << tmp_float_reg << std::endl;
                break;
              case IncrType::DEC_OP:
                stream << "fsub.d " << inc_reg << "," << inc_reg << ","
                       << tmp_float_reg << std::endl;
                break;
              default:
                throw std::runtime_error (
                    "PreIncrement() - unrecognised IncrType operator");
              }

            context.deallocate_reg (tmp_reg);
            context.deallocate_reg (tmp_float_reg);
            break;
          }
        default:
        case TypeSpecifier::INT:
          {
            switch (incr_type_)
              {
              case IncrType::INC_OP:
                stream << "addi " << inc_reg << "," << inc_reg << ",1"
                       << std::endl;
                break;
              case IncrType::DEC_OP:
                stream << "addi " << inc_reg << "," << inc_reg << ",-1"
                       << std::endl;
                break;
              default:
                throw std::runtime_error (
                    "PreIncrement() - unrecognised IncrType operator");
              }
            break;
          }
        }
    }

  context.store (stream, identifier_, inc_reg);

  if (type == TypeSpecifier::FLOAT)
    {
      stream << "fmv.s " << dest_reg << ", " << inc_reg << std::endl;
    }
  else if (type == TypeSpecifier::DOUBLE)
    {
      stream << "fmv.d " << dest_reg << ", " << inc_reg << std::endl;
    }
  else
    {
      // int/pointer
      stream << "mv " << dest_reg << ", " << inc_reg << std::endl;
    }

  context.deallocate_reg (inc_reg);
}

void
PreIncrement::Print (std::ostream &stream) const
{
  stream << "++";
  unary_expression_->Print (stream);
}

int
PreIncrement::EvalConstExpr (Context &context) const
{
  if (!unary_expression_)
    {
      throw std::runtime_error ("EvalConstExpr() - Null operand detected");
    }

  int expr = unary_expression_->EvalConstExpr (context);
  switch (incr_type_)
    {
    case IncrType::INC_OP:
      return expr + 1;
    case IncrType::DEC_OP:
      return expr - 1;
    default:
      throw std::runtime_error (
          "PreIncrement::EvalConstExpr() - unexpected operator");
    }

  /*
  Cannot use ++ and -- as the change won't be reflected outside here
  */
}
}

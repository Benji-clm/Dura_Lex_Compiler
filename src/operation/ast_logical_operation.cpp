#include "operation/ast_logical_operation.hpp"

namespace ast
{

void
LogicalOperation::EmitRISC (std::ostream &stream, Context &context,
                            std::string &dest_reg) const
{
  TypeSpecifier type = left_->getType (context);
  stream << "# Logical Operation type: " << type << std::endl;

  switch (type)
    {
    case TypeSpecifier::FLOAT:
      {
        std::string left_fp = context.allocate_reg (type);
        left_->EmitRISC (stream, context, left_fp);
        std::string right_fp = context.allocate_reg (type);
        right_->EmitRISC (stream, context, right_fp);

        TypeSpecifier result_type = TypeSpecifier::INT;
        std::string result_int = context.allocate_reg (result_type);

        switch (logical_operator_)
          {
          case LogicalOpType::L_OP:

            stream << "flt.s " << result_int << ", " << left_fp << ", "
                   << right_fp << std::endl;
            break;
          case LogicalOpType::G_OP:

            stream << "flt.s " << result_int << ", " << right_fp << ", "
                   << left_fp << std::endl;
            break;
          case LogicalOpType::LE_OP:
            stream << "fle.s " << result_int << ", " << left_fp << ", "
                   << right_fp << std::endl;
            break;
          case LogicalOpType::GE_OP:
            stream << "fle.s " << result_int << ", " << right_fp << ", "
                   << left_fp << std::endl;
            break;
          case LogicalOpType::EQ_OP:
            stream << "feq.s " << result_int << ", " << left_fp << ", "
                   << right_fp << std::endl;
            break;
          case LogicalOpType::NE_OP:
            stream << "feq.s " << result_int << ", " << left_fp << ", "
                   << right_fp << std::endl;
            stream << "xori " << result_int << ", " << result_int << ", 1"
                   << std::endl;
            break;
          case LogicalOpType::AND_OP:
            {
              // remember for float -> True if non-zero, else false
              std::string false_label = context.makeUniqueLabel (".L");
              std::string end_label = context.makeUniqueLabel (".L");

              // a floating point register containg "zero" to do the
              // comparaison
              std::string zero_reg = context.allocate_reg (type);

              // load the zero
              stream << "fmv.w.x " << zero_reg << ",zero" << std::endl;

              // compare left to zero
              stream << "feq.s " << result_int << "," << left_fp << ","
                     << zero_reg << std::endl;
              // if equal, then float is zero, hence its considered a zero, and
              // we should not evaluate the condition and jump to the false
              // label

              stream << "bne " << result_int << ",zero," << false_label
                     << std::endl;
              //    if result_int is not equal to zero, we do not evaluate
              //    condition, jump to end

              //    same for right
              stream << "feq.s " << result_int << "," << right_fp << ","
                     << zero_reg << std::endl;
              stream << "bne " << result_int << ",zero," << false_label
                     << std::endl;

              stream << "li " << result_int << ", 1" << std::endl;
              stream << "j " << end_label << std::endl;

              stream << false_label << ":" << std::endl;
              stream << "li " << result_int << ", 0" << std::endl;

              stream << end_label << ":" << std::endl;

              context.deallocate_reg (zero_reg);
              break;
            }
          case LogicalOpType::OR_OP:
            {
              // remember for float -> True if non-zero, else false
              std::string false_label = context.makeUniqueLabel (".L");
              std::string true_label = context.makeUniqueLabel (".L");
              std::string end_label = context.makeUniqueLabel (".L");

              std::string zero_reg = context.allocate_reg (type);

              stream << "fmv.w.x " << zero_reg << ",zero" << std::endl;

              stream << "feq.s " << result_int << "," << left_fp << ","
                     << zero_reg << std::endl;

              stream << "beq " << result_int << ",zero," << true_label
                     << std::endl;
              //    here no need to evaluate second one if true, its or hence
              //    already true

              //    same for right
              stream << "feq.s " << result_int << "," << right_fp << ","
                     << zero_reg << std::endl;
              stream << "bne " << result_int << ",zero," << false_label
                     << std::endl;

              stream << true_label << ":" << std::endl;
              stream << "li " << result_int << ", 1" << std::endl;
              stream << "j " << end_label << std::endl;

              stream << false_label << ":" << std::endl;
              stream << "li " << result_int << ", 0" << std::endl;

              stream << end_label << ":" << std::endl;

              context.deallocate_reg (zero_reg);
              break;
            }
          default:
            throw std::runtime_error (
                "LogicalOperation for FLOAT - unexpected operator");
          }

        stream << "addi " << dest_reg << ", " << result_int << ", 0"
               << std::endl;

        context.deallocate_reg (result_int);
        context.deallocate_reg (right_fp);
        context.deallocate_reg (left_fp);

        break;
      }
    case TypeSpecifier::DOUBLE:
      {
        context.setCurrentType (TypeSpecifier::DOUBLE);
        std::string left_dp = context.allocate_reg (type);
        left_->EmitRISC (stream, context, left_dp);
        std::string right_dp = context.allocate_reg (type);
        right_->EmitRISC (stream, context, right_dp);

        TypeSpecifier result_type = TypeSpecifier::INT;
        std::string result_int = context.allocate_reg (result_type);

        switch (logical_operator_)
          {
          case LogicalOpType::L_OP:
            stream << "flt.d " << result_int << ", " << left_dp << ", "
                   << right_dp << std::endl;
            break;
          case LogicalOpType::G_OP:
            stream << "flt.d " << result_int << ", " << right_dp << ", "
                   << left_dp << std::endl;
            break;
          case LogicalOpType::LE_OP:
            stream << "fle.d " << result_int << ", " << left_dp << ", "
                   << right_dp << std::endl;
            break;
          case LogicalOpType::GE_OP:
            stream << "fle.d " << result_int << ", " << right_dp << ", "
                   << left_dp << std::endl;
            break;
          case LogicalOpType::EQ_OP:
            stream << "feq.d " << result_int << ", " << left_dp << ", "
                   << right_dp << std::endl;
            break;
          case LogicalOpType::NE_OP:
            stream << "feq.d " << result_int << ", " << left_dp << ", "
                   << right_dp << std::endl;
            stream << "xori " << result_int << ", " << result_int << ", 1"
                   << std::endl;
            break;
          case LogicalOpType::AND_OP:
            {
              std::string false_label = context.makeUniqueLabel (".L");
              std::string end_label = context.makeUniqueLabel (".L");

              std::string zero_reg = context.allocate_reg (type);

              stream << "fcvt.d.w " << zero_reg << ",x0" << std::endl;

              stream << "feq.d " << result_int << "," << left_dp << ","
                     << zero_reg << std::endl;

              stream << "bne " << result_int << ",zero," << false_label
                     << std::endl;

              stream << "feq.d " << result_int << "," << right_dp << ","
                     << zero_reg << std::endl;
              stream << "bne " << result_int << ",zero," << false_label
                     << std::endl;

              stream << "li " << result_int << ", 1" << std::endl;
              stream << "j " << end_label << std::endl;

              stream << false_label << ":" << std::endl;
              stream << "li " << result_int << ", 0" << std::endl;

              stream << end_label << ":" << std::endl;

              context.deallocate_reg (zero_reg);
              break;
            }
          case LogicalOpType::OR_OP:
            {
              // remember for float -> True if non-zero, else false
              std::string false_label = context.makeUniqueLabel (".L");
              std::string true_label = context.makeUniqueLabel (".L");
              std::string end_label = context.makeUniqueLabel (".L");

              std::string zero_reg = context.allocate_reg (type);

              stream << "fcvt.d.w " << zero_reg << ",zero" << std::endl;

              stream << "feq.d " << result_int << "," << left_dp << ","
                     << zero_reg << std::endl;

              stream << "beq " << result_int << ",zero," << true_label
                     << std::endl;

              stream << "feq.d " << result_int << "," << right_dp << ","
                     << zero_reg << std::endl;
              stream << "bne " << result_int << ",zero," << false_label
                     << std::endl;

              stream << true_label << ":" << std::endl;
              stream << "li " << result_int << ", 1" << std::endl;
              stream << "j " << end_label << std::endl;

              stream << false_label << ":" << std::endl;
              stream << "li " << result_int << ", 0" << std::endl;

              stream << end_label << ":" << std::endl;

              context.deallocate_reg (zero_reg);
              break;
            }
          default:
            throw std::runtime_error (
                "LogicalOperation for DOUBLE - unexpected operator");
          }
        stream << "addi " << dest_reg << ", " << result_int << ", 0"
               << std::endl;
        context.deallocate_reg (result_int);
        context.deallocate_reg (right_dp);
        context.deallocate_reg (left_dp);
        break;
      }
    case TypeSpecifier::INT:
    default:
      {
        std::string left_reg = context.allocate_reg (type);
        left_->EmitRISC (stream, context, left_reg);
        right_->EmitRISC (stream, context, dest_reg);

        switch (logical_operator_)
          {
          case LogicalOpType::L_OP:
            stream << "slt " << dest_reg << ", " << left_reg << ", "
                   << dest_reg << std::endl;
            stream << "andi " << dest_reg << ", " << dest_reg << ", 0xff"
                   << std::endl;
            break;
          case LogicalOpType::G_OP:
            stream << "sgt " << dest_reg << ", " << left_reg << ", "
                   << dest_reg << std::endl;
            stream << "andi " << dest_reg << ", " << dest_reg << ", 0xff"
                   << std::endl;
            break;
          case LogicalOpType::LE_OP:
            stream << "sgt " << dest_reg << ", " << left_reg << ", "
                   << dest_reg << std::endl;
            stream << "xori " << dest_reg << ", " << dest_reg << ", 1"
                   << std::endl;
            stream << "andi " << dest_reg << ", " << dest_reg << ", 0xff"
                   << std::endl;
            break;
          case LogicalOpType::GE_OP:
            stream << "slt " << dest_reg << ", " << left_reg << ", "
                   << dest_reg << std::endl;
            stream << "xori " << dest_reg << ", " << dest_reg << ", 1"
                   << std::endl;
            stream << "andi " << dest_reg << ", " << dest_reg << ", 0xff"
                   << std::endl;
            break;
          case LogicalOpType::EQ_OP:
            stream << "sub " << dest_reg << ", " << left_reg << ", "
                   << dest_reg << std::endl;
            stream << "seqz " << dest_reg << ", " << dest_reg << std::endl;
            stream << "andi " << dest_reg << ", " << dest_reg << ", 0xff"
                   << std::endl;
            break;
          case LogicalOpType::NE_OP:
            stream << "sub " << dest_reg << ", " << left_reg << ", "
                   << dest_reg << std::endl;
            stream << "snez " << dest_reg << ", " << dest_reg << std::endl;
            stream << "andi " << dest_reg << ", " << dest_reg << ", 0xff"
                   << std::endl;
            break;
          case LogicalOpType::AND_EXP:
            stream << "and " << dest_reg << ", " << left_reg << ", "
                   << dest_reg << std::endl;
            break;
          case LogicalOpType::AND_OP:
            {
              std::string false_label = context.makeUniqueLabel (".L");
              std::string end_label = context.makeUniqueLabel (".L");
              stream << "beq " << left_reg << ", zero, " << false_label
                     << std::endl;
              right_->EmitRISC (stream, context, dest_reg);
              stream << "beq " << dest_reg << ", zero, " << false_label
                     << std::endl;
              stream << "li " << dest_reg << ", 1" << std::endl;
              stream << "j " << end_label << std::endl;
              stream << false_label << ":" << std::endl;
              stream << "li " << dest_reg << ", 0" << std::endl;
              stream << end_label << ":" << std::endl;
              break;
            }
          case LogicalOpType::OR_OP:
            {
              std::string true_label = context.makeUniqueLabel ("L");
              std::string end_label = context.makeUniqueLabel ("L");
              stream << "bnez " << left_reg << ", " << true_label << std::endl;
              right_->EmitRISC (stream, context, dest_reg);
              stream << "snez " << dest_reg << ", " << dest_reg << std::endl;
              stream << "j " << end_label << std::endl;
              stream << true_label << ":" << std::endl;
              stream << "li " << dest_reg << ", 1" << std::endl;
              stream << end_label << ":" << std::endl;
              break;
            }
          case LogicalOpType::OR_EXP:
            stream << "or " << dest_reg << ", " << left_reg << ", " << dest_reg
                   << std::endl;
            break;
          case LogicalOpType::XOR_OP:
            stream << "xor " << dest_reg << ", " << left_reg << ", "
                   << dest_reg << std::endl;
            break;
          default:
            throw std::runtime_error (
                "LogicalOperation() - unexpected logical operator");
          }
        context.deallocate_reg (left_reg);
        break;
      }
    }
}

void
LogicalOperation::Print (std::ostream &stream) const
{
  left_->Print (stream);
  stream << " " << static_cast<int> (logical_operator_) << " ";
  right_->Print (stream);
}

int
LogicalOperation::EvalConstExpr (Context &context) const
{
  if (!left_ || !right_)
    {
      throw std::runtime_error ("EvalConstExpr() - Null operand detected");
    }

  int l = left_->EvalConstExpr (context);
  int r = right_->EvalConstExpr (context);

  switch (logical_operator_)
    {
    // Relational operators => return 0 or 1
    case LogicalOpType::L_OP: // <
      return (l < r) ? 1 : 0;
    case LogicalOpType::G_OP: // >
      return (l > r) ? 1 : 0;
    case LogicalOpType::LE_OP: // <=
      return (l <= r) ? 1 : 0;
    case LogicalOpType::GE_OP: // >=
      return (l >= r) ? 1 : 0;
    case LogicalOpType::EQ_OP: // ==
      return (l == r) ? 1 : 0;
    case LogicalOpType::NE_OP: // !=
      return (l != r) ? 1 : 0;

    // Bitwise operators
    case LogicalOpType::AND_EXP: // '&'
      return l & r;
    case LogicalOpType::XOR_OP: // '^'
      return l ^ r;
    case LogicalOpType::OR_EXP: // '|'
      return l | r;

    // Logical operators => also return 0 or 1
    case LogicalOpType::AND_OP: // '&&'
      return ((l != 0) && (r != 0)) ? 1 : 0;
    case LogicalOpType::OR_OP: // '||'
      return ((l != 0) || (r != 0)) ? 1 : 0;

    default:
      throw std::runtime_error (
          "LogicalOperation::EvalConstExpr() - Unexpected operator");
    }
}

} // namespace ast

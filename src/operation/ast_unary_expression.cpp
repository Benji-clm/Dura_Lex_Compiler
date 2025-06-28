#include "operation/ast_unary_expression.hpp"

namespace ast
{
void
UnaryExpression::EmitRISC (std::ostream &stream, Context &context,
                           std::string &dest_reg) const
{

  TypeSpecifier type = cast_expression->getType (context);
  std::string neg_op = "neg";
  // default value just in case (will be overwritten anyways)

  switch (unary_op)
    {
    case UnaryOp::ADDRESSOF:
      cast_expression->EmitADDRESS (stream, context, dest_reg);
      break;

    case UnaryOp::INDIRECTION:
      context.setIsCurrentPointer (true);
      cast_expression->EmitRISC (stream, context, dest_reg);
      if (!context.isLeftOp ())
        {
          std::string load_instr = context.get_load_instr (type);
          stream << load_instr << " " << dest_reg << ",0(" << dest_reg << ")"
                 << std::endl;
        }
      context.setIsCurrentPointer (false);
      break;

    case UnaryOp::U_PLUS:
      cast_expression->EmitRISC (stream, context, dest_reg);
      break;

    case UnaryOp::U_MINUS:
      cast_expression->EmitRISC (stream, context, dest_reg);

      switch (type)
        {
        case TypeSpecifier::FLOAT:
          neg_op = "fneg.s";
          break;
        case TypeSpecifier::DOUBLE:
          neg_op = "fneg.d";
          break;
        case TypeSpecifier::INT:
        default:
          neg_op = "neg";
          break;
        }

      stream << neg_op << " " << dest_reg << "," << dest_reg << std::endl;
      break;

    case UnaryOp::BIT_NOT:
      cast_expression->EmitRISC (stream, context, dest_reg);
      if (type == TypeSpecifier::INT)
        {
          stream << "not " << dest_reg << "," << dest_reg << std::endl;
        }
      else
        {
          throw std::runtime_error (
              "Bro... Really? You're really trying to use a Bitwise Not on a "
              "non-integer... IN C90???? - nah you're on your own now. "
              "(UnaryExpression EmitRISC() error)");
        }
      break;

    case UnaryOp::LOG_NOT:
      cast_expression->EmitRISC (stream, context, dest_reg);
      if (type == TypeSpecifier::INT)
        {
          stream << "seqz " << dest_reg << "," << dest_reg << std::endl;
        }
      else
        {
          throw std::runtime_error (
              "Bro, not again... We've already talked about it, you can't "
              "just use integer only operation on non-integers. "
              "UnaryExpression LOG_NOT EmitRISC() error");
        }
      break;

    default:
      throw std::runtime_error (
          "UnaryExpression - Print() - unrecognized Unary Operator");
    }
}

void
UnaryExpression::Print (std::ostream &stream) const
{
  switch (unary_op)
    {
    case UnaryOp::ADDRESSOF:
      stream << "&";
      break;
    case UnaryOp::INDIRECTION:
      stream << "*";
      break;
    case UnaryOp::U_PLUS:
      stream << "+";
      break;
    case UnaryOp::U_MINUS:
      stream << "-";
      break;
    case UnaryOp::BIT_NOT:
      stream << "~";
      break;
    case UnaryOp::LOG_NOT:
      stream << "!";
      break;
    default:
      throw std::runtime_error (
          "UnaryExpression - Print() - unrecognized Unary Operator");
    }
  cast_expression->Print (stream);
}

int
UnaryExpression::EvalConstExpr (Context &context) const
{
  if (!cast_expression)
    {
      throw std::runtime_error (
          "UnaryExpression::EvalConstExpr() - Null operand detected");
    }

  int val = cast_expression->EvalConstExpr (context);

  switch (unary_op)
    {
    case UnaryOp::ADDRESSOF:
      // Not valid in a constant expression in C.
      throw std::runtime_error ("EvalConstExpr() - ADDRESSOF (&) not allowed "
                                "in a constant expression");

    case UnaryOp::INDIRECTION:
      // Also not valid in a constant expression in C.
      throw std::runtime_error ("EvalConstExpr() - INDIRECTION (*) not "
                                "allowed in a constant expression");

    case UnaryOp::U_PLUS:
      // what is +x -_-, honnestly I still don't understand why U_PLUS exists
      return val;

    case UnaryOp::U_MINUS:
      // negation negation negation
      return -val;

    case UnaryOp::BIT_NOT:
      //    ._.
      // ~x ~|~       yes thats a dancing man
      return ~val;

    case UnaryOp::LOG_NOT:
      return (val == 0) ? 1 : 0;

    default:
      throw std::runtime_error (
          "EvalConstExpr() - Unrecognized unary operator");
    }
}

}

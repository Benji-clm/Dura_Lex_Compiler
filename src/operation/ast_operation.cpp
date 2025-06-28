#include "operation/ast_operation.hpp"
#include <cmath>

namespace ast
{

void
Operation::EmitRISC (std::ostream &stream, Context &context,
                     std::string &dest_reg) const
{
  // Emit code for the left-hand operand into dest_reg.
  left_->EmitRISC (stream, context, dest_reg);

  std::string l_identifier = left_->getIdentifier ();
  bool is_left_pointer = context.get_is_pointer (l_identifier);

  TypeSpecifier left_type = left_->getType (context);

  int bytes = 4;
  std::string load_instr = "lw";
  std::string store_instr = "sw";

  if (!is_left_pointer)
    {
      stream << "# I guess float are somehow seen as pointers??? "
             << is_left_pointer << std::endl;
      bytes = context.getBytes (left_type);
      load_instr = context.get_load_instr (left_type);
      store_instr = context.get_store_instr (left_type);
    }

  int loc = context.push_stack (bytes);

  stream << store_instr << " " << dest_reg << "," << loc << "(s0)"
         << std::endl;

  // Allocate a register for the right-hand operand.
  TypeSpecifier type = right_->getType (context);
  std::string right_reg = context.allocate_reg (type);
  right_->EmitRISC (stream, context, right_reg);

  stream << load_instr << " " << dest_reg << "," << loc << "(s0)" << std::endl;

  context.pop_stack (bytes);

  if (is_left_pointer)
    {
      std::string r_identifier = right_->getIdentifier ();
      bool is_right_pointer = context.get_is_pointer (r_identifier);
      bytes = context.getBytes (left_type);

      switch (operator_)
        {
        case OpType::ADD:
          //   assuming user is not dumb and he has a constant as right op?
          stream << "slli " << right_reg << "," << right_reg << ","
                 << std::log2 (bytes) << std::endl;
          stream << "add " << dest_reg << "," << dest_reg << "," << right_reg
                 << std::endl;
          break;
        case OpType::SUB:
          if (is_right_pointer)
            {
              stream << "sub " << dest_reg << "," << dest_reg << ","
                     << right_reg << std::endl;
              if (bytes != 1)
                {
                  stream << "srai " << dest_reg << "," << dest_reg << ","
                         << std::log2 (bytes) << std::endl;
                }
            }
          else
            {
              stream << "slli " << right_reg << "," << right_reg << ","
                     << std::log2 (bytes) << std::endl;
              stream << "sub " << dest_reg << "," << dest_reg << ","
                     << right_reg << std::endl;
            }
          break;
        default:
          throw std::runtime_error ("Invalid Pointer Operation");
        }
    }
  else if (left_type == TypeSpecifier::FLOAT)
    {
      // Floating-point operations use different instructions.
      switch (operator_)
        {
        case OpType::ADD:
          // fadd.s rd, rs1, rs2  => rd = rs1 + rs2
          stream << "fadd.s " << right_reg << ", " << dest_reg << ", "
                 << right_reg << std::endl;
          // Optionally move the result back to dest_reg.
          stream << "fmv.s " << dest_reg << ", " << right_reg << std::endl;
          break;
        case OpType::SUB:
          stream << "fsub.s " << right_reg << ", " << dest_reg << ", "
                 << right_reg << std::endl;
          stream << "fmv.s " << dest_reg << ", " << right_reg << std::endl;
          break;
        case OpType::MUL:
          stream << "fmul.s " << right_reg << ", " << dest_reg << ", "
                 << right_reg << std::endl;
          stream << "fmv.s " << dest_reg << ", " << right_reg << std::endl;
          break;
        case OpType::DIV:
          stream << "fdiv.s " << right_reg << ", " << dest_reg << ", "
                 << right_reg << std::endl;
          stream << "fmv.s " << dest_reg << ", " << right_reg << std::endl;
          break;
        case OpType::LEFT_SHIFT:
        case OpType::RIGHT_SHIFT:
        case OpType::REM:
          // C90 does not support a remainder and bitwise shifts operation
          // for floats.
          throw std::runtime_error (
              "Operation::EmitRISC() - REM not supported for FLOAT type");
        default:
          throw std::runtime_error (
              "Operation::EmitRISC() - unexpected operator");
        }
    }
  else if (left_type == TypeSpecifier::DOUBLE)
    {
      switch (operator_)
        {
        case OpType::ADD:
          stream << "fadd.d " << dest_reg << ", " << dest_reg << ", "
                 << right_reg << std::endl;
          break;
        case OpType::SUB:
          stream << "fsub.d " << dest_reg << ", " << dest_reg << ", "
                 << right_reg << std::endl;
          break;
        case OpType::MUL:
          stream << "fmul.d " << dest_reg << ", " << dest_reg << ", "
                 << right_reg << std::endl;
          break;
        case OpType::DIV:
          stream << "fdiv.d " << dest_reg << ", " << dest_reg << ", "
                 << right_reg << std::endl;
          break;
        case OpType::LEFT_SHIFT:
        case OpType::RIGHT_SHIFT:
        case OpType::REM:
          // similary as for floats, not supported in C90.
          throw std::runtime_error (
              "Operation::EmitRISC() - REM not supported for DOUBLE type");
        default:
          throw std::runtime_error (
              "Operation::EmitRISC() - unexpected operator");
        }
    }

  else
    {
      // Integer operations.
      switch (operator_)
        {
        case OpType::ADD:
          stream << "add " << dest_reg << ", " << dest_reg << ", " << right_reg
                 << std::endl;
          break;
        case OpType::SUB:
          stream << "sub " << dest_reg << ", " << dest_reg << ", " << right_reg
                 << std::endl;
          break;
        case OpType::MUL:
          stream << "mul " << dest_reg << ", " << dest_reg << ", " << right_reg
                 << std::endl;
          break;
        case OpType::DIV:
          stream << "div " << dest_reg << ", " << dest_reg << ", " << right_reg
                 << std::endl;
          break;
        case OpType::REM:
          stream << "rem " << dest_reg << ", " << dest_reg << ", " << right_reg
                 << std::endl;
          break;
        case OpType::LEFT_SHIFT:
          stream << "sll " << dest_reg << "," << dest_reg << "," << right_reg
                 << std::endl;
          break;
        case OpType::RIGHT_SHIFT:
          stream << "sra " << dest_reg << "," << dest_reg << "," << right_reg
                 << std::endl;
          break;
        default:
          throw std::runtime_error (
              "Operation::EmitRISC() - unexpected operator");
        }
    }
  context.deallocate_reg (right_reg);
}

void
Operation::Print (std::ostream &stream) const
{
  left_->Print (stream);
  stream << operator_;
  right_->Print (stream);
}

int
Operation::EvalConstExpr (Context &context) const
{
  if (!left_ || !right_)
    {
      throw std::runtime_error ("EvalConstExpr() - Null operand detected");
    }

  int l = left_->EvalConstExpr (context);
  int r = right_->EvalConstExpr (context);

  switch (operator_)
    {
    case OpType::ADD:
      return l + r;
    case OpType::SUB:
      return l - r;
    case OpType::MUL:
      return l * r;
    case OpType::DIV:
      if (r == 0)
        {
          throw std::runtime_error ("EvalConstExpr() - Division by zero");
        }
      return l / r;
    case OpType::REM:
      if (r == 0)
        {
          throw std::runtime_error ("EvalConstExpr() - Modulo by zero");
        }
      return l % r;
    case OpType::LEFT_SHIFT:
      if (l < 0 || r < 0)
        {
          throw std::runtime_error (
              "EvalConstExpr() - Undefined shift behavior on negative values");
        }
      return l << r;
    case OpType::RIGHT_SHIFT:
      if (l < 0 || r < 0)
        {
          throw std::runtime_error (
              "EvalConstExpr() - Undefined shift behavior on negative values");
        }
      return l >> r;
    default:
      throw std::runtime_error (
          "Operation::EvalConstExpr() - unexpected operator");
    }
}

} // namespace ast

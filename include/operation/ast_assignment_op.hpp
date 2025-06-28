#pragma once

#include <stdexcept>
#include <string_view>

namespace ast
{
enum class AssignmentOp
{
  ASSIGN,
  ADD_ASSIGN,
  SUB_ASSIGN,
  MUL_ASSIGN,
  DIV_ASSIGN,
  MOD_ASSIGN,
  RIGHT_ASSIGN,
  LEFT_ASSIGN,
  AND_ASSIGN,
  XOR_ASSIGN,
  OR_ASSIGN
};

template <typename LogStream>
LogStream &
operator<< (LogStream &ls, const AssignmentOp &operator_)
{
  const auto OperatorToString = [&operator_] {
    switch (operator_)
      {
      case AssignmentOp::ASSIGN:
        return "assign";
      case AssignmentOp::ADD_ASSIGN:
        return "add_assign";
      case AssignmentOp::SUB_ASSIGN:
        return "sub_assign";
      case AssignmentOp::MUL_ASSIGN:
        return "mul_assign";
      case AssignmentOp::DIV_ASSIGN:
        return "div_assign";
      case AssignmentOp::MOD_ASSIGN:
        return "mod_assign";
      case AssignmentOp::RIGHT_ASSIGN:
        return "right_assign";
      case AssignmentOp::LEFT_ASSIGN:
        return "left_assign";
      case AssignmentOp::AND_ASSIGN:
        return "and_assign";
      case AssignmentOp::XOR_ASSIGN:
        return "xor_assign";
      case AssignmentOp::OR_ASSIGN:
        return "or_assign";
      default:
        throw std::runtime_error ("Unexpected assigment operator");
      };
  };
  return ls << OperatorToString ();
}
}

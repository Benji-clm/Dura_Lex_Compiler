#pragma once

#include <stdexcept>
#include <string_view>

namespace ast
{
enum class OpType
{
  ADD,
  SUB,
  MUL,
  DIV,
  REM,
  LEFT_SHIFT,
  RIGHT_SHIFT
};

enum class LogicalOpType
{
  LE_OP,
  GE_OP,
  L_OP,
  G_OP,
  EQ_OP,
  NE_OP,
  AND_EXP, // '&'
  AND_OP,  // '&&'
  XOR_OP,
  OR_EXP, // '|'
  OR_OP   // '||'
};

enum class IncrType
{
  INC_OP,
  DEC_OP
};

template <typename LogStream>
LogStream &
operator<< (LogStream &ls, const OpType &operator_)
{
  const auto OperatorToString = [&operator_] {
    switch (operator_)
      {
      case OpType::ADD:
        return "add";
      case OpType::SUB:
        return "sub";
      case OpType::MUL:
        return "mul";
      case OpType::DIV:
        return "div";
      case OpType::REM:
        return "rem";
      case OpType::LEFT_SHIFT:
        return "left_shift";
      case OpType::RIGHT_SHIFT:
        return "right_shift";
      default:
        throw std::runtime_error ("Unexpected arithmetic operator");
      };
  };
  return ls << OperatorToString ();
}

template <typename LogStream>
LogStream &
operator<< (LogStream &ls, const LogicalOpType &logical_operator_)
{
  const auto OperatorToString = [&logical_operator_] {
    switch (logical_operator_)
      {
      case LogicalOpType::LE_OP:
        return "le_op";
      case LogicalOpType::GE_OP:
        return "ge_op";
      case LogicalOpType::L_OP:
        return "l_op";
      case LogicalOpType::G_OP:
        return "g_op";
      case LogicalOpType::EQ_OP:
        return "eq_op";
      case LogicalOpType::NE_OP:
        return "ne_op";
      case LogicalOpType::AND_EXP:
        return "and_exp";
      case LogicalOpType::AND_OP:
        return "and_op";
      case LogicalOpType::XOR_OP:
        return "xor_op";
      case LogicalOpType::OR_EXP:
        return "or_exp";
      case LogicalOpType::OR_OP:
        return "or_op";
      default:
        throw std::runtime_error ("Unexpected logical operator");
      };
  };
  return ls << OperatorToString ();
}

template <typename LogStream>
LogStream &
operator<< (LogStream &ls, const IncrType &incr_type_)
{
  const auto OperatorToString = [&incr_type_] {
    switch (incr_type_)
      {
      case IncrType::INC_OP:
        return "inc_op";
      case IncrType::DEC_OP:
        return "dec_op";
      default:
        throw std::runtime_error ("Unexpected increment type");
      };
  };
  return ls << OperatorToString ();
}
}

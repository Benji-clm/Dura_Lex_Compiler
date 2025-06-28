#pragma once

#include "ast_node.hpp"

namespace ast
{
enum class UnaryOp
{
  ADDRESSOF,
  INDIRECTION, // (known as dereference)
  U_PLUS,
  U_MINUS,
  BIT_NOT,
  LOG_NOT
};

class UnaryExpression : public Node
{
private:
  UnaryOp unary_op;
  NodePtr cast_expression;

public:
  UnaryExpression (UnaryOp unary_op_, NodePtr cast_expression_)
      : unary_op (unary_op_), cast_expression (std::move (cast_expression_))
  {
  }

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;
  void Print (std::ostream &stream) const override;

  std::string
  getIdentifier () const override
  {
    return cast_expression->getIdentifier ();
  }

  bool
  isPointer () const override
  {
    if (unary_op == UnaryOp::INDIRECTION)
      {
        return true;
      }
    else
      {
        return false;
      }
  }

  TypeSpecifier
  getType (Context &context) const override
  {
    return cast_expression->getType (context);
  }

  int EvalConstExpr (Context &context) const override;
};
}

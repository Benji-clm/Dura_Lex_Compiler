#pragma once

#include "ast_node.hpp"
#include "ast_operator_type.hpp"

namespace ast
{
class PreIncrement : public Node
{
private:
  IncrType incr_type_;
  NodePtr unary_expression_;

public:
  PreIncrement (IncrType incr_type, NodePtr unary_expression)
      : incr_type_ (incr_type),
        unary_expression_ (std::move (unary_expression))
  {
  }

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;
  void Print (std::ostream &stream) const override;

  int EvalConstExpr (Context &context) const override;
};
}

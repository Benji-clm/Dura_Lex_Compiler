#pragma once

#include "ast_node.hpp"
#include "ast_operator_type.hpp"

namespace ast
{
class Increment : public Node
{
private:
  NodePtr expression_;
  IncrType incr_type_;

public:
  Increment (NodePtr expression, IncrType incr_type)
      : expression_ (std::move (expression)), incr_type_ (incr_type)
  {
  }

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;
  void Print (std::ostream &stream) const override;

  int EvalConstExpr (Context &context) const override;
};
}

#pragma once

#include "ast_node.hpp"
#include "ast_operator_type.hpp"

namespace ast
{
class Operation : public Node
{
private:
  NodePtr left_;
  OpType operator_;
  NodePtr right_;

public:
  Operation (NodePtr left, OpType op, NodePtr right)
      : left_ (std::move (left)), operator_ (op), right_ (std::move (right))
  {
  }
  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;
  void Print (std::ostream &stream) const override;

  TypeSpecifier
  getType (Context &context) const override
  {
    return left_->getType (context);
  }

  int EvalConstExpr (Context &context) const override;
};
}

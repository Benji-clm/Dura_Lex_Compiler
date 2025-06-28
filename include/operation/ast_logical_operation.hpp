#pragma once

#include "ast_node.hpp"
#include "ast_operator_type.hpp"

namespace ast
{
class LogicalOperation : public Node
{
private:
  NodePtr left_;
  LogicalOpType logical_operator_;
  NodePtr right_;

public:
  LogicalOperation (NodePtr left, LogicalOpType logical_operator,
                    NodePtr right)
      : left_ (std::move (left)), logical_operator_ (logical_operator),
        right_ (std::move (right))
  {
  }
  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;
  void Print (std::ostream &stream) const override;

  int EvalConstExpr (Context &context) const override;
};
}

#pragma once

#include "ast_node.hpp"

namespace ast
{
class IfElse : public Node
{
private:
  NodePtr condition_;
  NodePtr if_statement_;
  NodePtr else_statement_;

public:
  IfElse (NodePtr condition, NodePtr if_statement, NodePtr else_statement)
      : condition_ (std::move (condition)),
        if_statement_ (std::move (if_statement)),
        else_statement_ (std::move (else_statement))
  {
  }

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;
  void Print (std::ostream &stream) const override;
};
}

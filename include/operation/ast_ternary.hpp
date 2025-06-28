#pragma once

#include "ast_node.hpp"

namespace ast
{
class Ternary : public Node
{
private:
  NodePtr condition_;
  NodePtr expressionTrue_;
  NodePtr expressionFalse_;

public:
  Ternary (NodePtr condition, NodePtr expressionTrue, NodePtr expressionFalse)
      : condition_ (std::move (condition)),
        expressionTrue_ (std::move (expressionTrue)),
        expressionFalse_ (std::move (expressionFalse))
  {
  }

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;

  void Print (std::ostream &stream) const override;

  TypeSpecifier
  getType (Context &context) const override
  {
    return condition_->getType (context);
  }
};
}

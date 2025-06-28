#pragma once

#include "ast_node.hpp"

namespace ast
{
class Switch : public Node
{
private:
  NodePtr expression_;
  NodePtr statement_;

public:
  Switch (NodePtr expression, NodePtr statement)
      : expression_ (std::move (expression)),
        statement_ (std::move (statement))
  {
  }

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;
  void Print (std::ostream &stream) const override;

  TypeSpecifier
  getType (Context &context) const override
  {
    return expression_->getType (context);
  }
};
}

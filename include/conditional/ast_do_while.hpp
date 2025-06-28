#pragma once

#include "ast_node.hpp"

namespace ast
{
class DoWhile : public Node
{
private:
  NodePtr statement_;
  NodePtr expression_;

public:
  DoWhile (NodePtr statement, NodePtr expression)
      : statement_ (std::move (statement)),
        expression_ (std::move (expression))
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

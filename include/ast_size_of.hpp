#pragma once

#include "ast_node.hpp"

namespace ast
{
class SizeOf : public Node
{
private:
  NodePtr unary_expression_;

public:
  SizeOf (NodePtr unary_expression)
      : unary_expression_ (std::move (unary_expression))
  {
  }

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;
  void Print (std::ostream &stream) const override;

  TypeSpecifier
  getType (Context &context) const override
  {
    return unary_expression_->getType (context);
  }
};
}

#pragma once

#include "ast_node.hpp"

namespace ast
{
class FunctionCall : public Node
{
private:
  NodePtr identifier_;
  NodePtr argument_expression_list_;

public:
  FunctionCall (NodePtr identifier, NodePtr argument_expression_list)
      : identifier_ (std::move (identifier)),
        argument_expression_list_ (std::move (argument_expression_list)) {};

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;

  void Print (std::ostream &stream) const override;
};
}

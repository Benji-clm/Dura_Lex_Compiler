#pragma once

#include "ast_node.hpp"

namespace ast
{
class FunctionArgument : public Node
{
private:
  NodePtr argument_;

public:
  FunctionArgument (NodePtr argument) : argument_ (std::move (argument)) {};

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;

  void Print (std::ostream &stream) const override;
};
}

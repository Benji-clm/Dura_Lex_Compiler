#pragma once

#include "ast_node.hpp"
#include "variant"

namespace ast
{
class ArrayInit : public Node
{
private:
  NodePtr initalizer_;

public:
  ArrayInit (NodePtr initalizer) : initalizer_ (std::move (initalizer)) {}

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;
  void Print (std::ostream &stream) const override;
};
}

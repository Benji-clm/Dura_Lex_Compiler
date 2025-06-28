#pragma once

#include "ast_node.hpp"

namespace ast
{
class While : public Node
{
private:
  NodePtr condition_;
  NodePtr statement_;

public:
  While (NodePtr condition, NodePtr statement)
      : condition_ (std::move (condition)), statement_ (std::move (statement))
  {
  }

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;
  void Print (std::ostream &stream) const override;
};
}

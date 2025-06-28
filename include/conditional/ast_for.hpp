#pragma once

#include "ast_node.hpp"

namespace ast
{
class For : public Node
{
private:
  NodePtr initializer_;
  NodePtr condition_;
  NodePtr iteration_expression_;
  NodePtr statement_;

public:
  For (NodePtr initializer, NodePtr condition, NodePtr iteration_expression,
       NodePtr statement)
      : initializer_ (std::move (initializer)),
        condition_ (std::move (condition)),
        iteration_expression_ (std::move (iteration_expression)),
        statement_ (std::move (statement))
  {
  }

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;
  void Print (std::ostream &stream) const override;
};
} // namespace ast

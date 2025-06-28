#pragma once

#include "ast_node.hpp"

namespace ast
{

class DirectDeclarator : public Node
{
private:
  NodePtr identifier_;

public:
  DirectDeclarator (NodePtr identifier)
      : identifier_ (std::move (identifier)) {};

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;
  void Print (std::ostream &stream) const override;
  std::string
  getIdentifier () const override
  {
    return identifier_->getIdentifier ();
  }
};

} // namespace ast

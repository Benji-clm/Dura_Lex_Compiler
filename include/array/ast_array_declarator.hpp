#pragma once

#include "ast_node.hpp"

namespace ast
{
class ArrayDeclarator : public Node
{
private:
  NodePtr identifier_;
  NodePtr size_array_;

public:
  ArrayDeclarator (NodePtr identifier, NodePtr size_array)
      : identifier_ (std::move (identifier)),
        size_array_ (std::move (size_array))
  {
  }

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;
  void Print (std::ostream &stream) const override;

  std::string
  getIdentifier () const override
  {
    return identifier_->getIdentifier ();
  }

  bool
  isFunctionDeclarator () const override
  {
    return false;
  }

  bool
  isArray () const override
  {
    return true;
  }
};
}

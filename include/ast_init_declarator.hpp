#pragma once

#include "ast_node.hpp"

namespace ast
{

class InitDeclarator : public Node
{
private:
  NodePtr declarator;
  NodePtr initializer;

public:
  InitDeclarator (NodePtr declarator_, NodePtr initializer_)
      : declarator (std::move (declarator_)),
        initializer (std::move (initializer_))
  {
  }

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;
  void Print (std::ostream &stream) const override;

  std::string
  getIdentifier () const override
  {
    return declarator->getIdentifier ();
  }
};
}

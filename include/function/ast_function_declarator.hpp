#pragma once

#include "ast_node.hpp"

namespace ast
{
class FunctionDeclarator : public Node
{
private:
  NodePtr direct_declarator;
  NodePtr parameter_list;

public:
  FunctionDeclarator (NodePtr direct_declarator_, NodePtr parameter_list_)
      : direct_declarator (std::move (direct_declarator_)),
        parameter_list (std::move (parameter_list_))
  {
  }

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;
  void Print (std::ostream &stream) const override;

  std::string
  getIdentifier () const override
  {
    return direct_declarator->getIdentifier ();
  }

  bool
  isFunctionDeclarator () const override
  {
    return true;
  }
};

} // namespace ast

#pragma once

#include "ast_node.hpp"

namespace ast
{
class Declaration : public Node
{
private:
  NodePtr declaration_specifier;
  NodePtr init_declarator_list;

public:
  Declaration (NodePtr declaration_specifier_, NodePtr init_declarator_list_)
      : declaration_specifier (std::move (declaration_specifier_)),
        init_declarator_list (std::move (init_declarator_list_))
  {
  }

  virtual ~Declaration () {}

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;
  void Print (std::ostream &stream) const override;

  TypeSpecifier
  getType (Context &context) const override
  {
    return declaration_specifier->getType (context);
  }

  std::string
  getIdentifier () const override
  {
    return init_declarator_list->getIdentifier ();
  }
};
}

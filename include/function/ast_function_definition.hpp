#pragma once

#include "ast_node.hpp"
#include "ast_type_specifier.hpp"

namespace ast
{

class FunctionDefinition : public Node
{
private:
  NodePtr declaration_specifiers_;
  NodePtr declarator_;
  NodePtr compound_statement_;

public:
  FunctionDefinition (NodePtr declaration_specifiers, NodePtr declarator,
                      NodePtr compound_statement)
      : declaration_specifiers_ (std::move (declaration_specifiers)),
        declarator_ (std::move (declarator)),
        compound_statement_ (std::move (compound_statement)) {};

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;
  void Print (std::ostream &stream) const override;

  TypeSpecifier
  getType (Context &context) const override
  {
    return declaration_specifiers_->getType (context);
  }
};

} // namespace ast

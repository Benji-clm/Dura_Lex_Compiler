#pragma once

#include "ast_node.hpp"
#include "ast_type_specifier.hpp"

namespace ast
{
class FunctionParameter : public Node
{
private:
  NodePtr declaration_specifier_;
  NodePtr declarator_;

public:
  FunctionParameter (NodePtr declaration_specifier, NodePtr declarator)
      : declaration_specifier_ (std::move (declaration_specifier)),
        declarator_ (std::move (declarator))
  {
  }

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;
  void Print (std::ostream &stream) const override;

  TypeSpecifier
  getType (Context &context) const override
  {
    return declaration_specifier_->getType (context);
  }
};

}

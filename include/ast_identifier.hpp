#pragma once

#include "ast_node.hpp"

namespace ast
{

class Identifier : public Node
{
private:
  std::string identifier_;

public:
  Identifier (std::string identifier)
      : identifier_ (std::move (identifier)) {};

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;
  void Print (std::ostream &stream) const override;

  std::string getIdentifier () const override;

  TypeSpecifier
  getType (Context &context) const override
  {
    return context.getType (identifier_);
  }

  bool
  isFunctionDeclarator () const override
  {
    return false;
  }

  bool
  isArray () const override
  {
    return false;
  }

  int
  EvalConstExpr (Context &context) const override
  {
    return context.findEnumVal (identifier_);
  }

  void EmitADDRESS (std::ostream &stream, Context &context,
                    std::string &dest_reg) const override;
};

} // namespace ast

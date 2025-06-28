#pragma once

#include "ast_node.hpp"

namespace ast
{
class String : public Node
{
private:
  std::string string_;

public:
  String (std::string string) : string_ (std::move (string)) {}

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;

  void Print (std::ostream &stream) const override;

  TypeSpecifier
  getType (Context &) const override
  {
    return TypeSpecifier::CHAR;
  }

  std::string
  getIdentifier () const override
  {
    return string_;
  }
};

}

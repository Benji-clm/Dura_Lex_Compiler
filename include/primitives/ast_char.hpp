#pragma once

#include "ast_node.hpp"

namespace ast
{
class CharConstant : public Node
{
private:
  int char_;

public:
  CharConstant (int char_val) : char_ (char_val) {}

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;

  void Print (std::ostream &stream) const override;

  int
  getValue () const override
  {
    return char_;
  };

  TypeSpecifier
  getType (Context &) const override
  {
    return TypeSpecifier::CHAR;
  }

  //   std::variant<int, float, double, char>
  //   GetConst () const override
  //   {
  //     return char_;
  //   }

  int
  EvalConstExpr (Context &) const override
  {
    return char_;
  }
};
}

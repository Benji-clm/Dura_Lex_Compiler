#pragma once

#include "ast_node.hpp"

namespace ast
{

class IntConstant : public Node
{
private:
  int value_;

public:
  IntConstant (int value) : value_ (value) {}

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;
  void Print (std::ostream &stream) const override;

  int
  getValue () const override
  {
    return value_;
  };

  TypeSpecifier
  getType (Context &) const override
  {
    return TypeSpecifier::INT;
  }

  std::variant<int, float, double>
  GetConst () const override
  {
    return value_;
  }

  int
  EvalConstExpr (Context &) const override
  {
    return value_;
  }
};

// Note that FloatConstant also does doubles (it is not possible to
// differentiate within the lexer between a float and a double)
class FloatDoubleConstant : public Node
{
private:
  double value_;

public:
  FloatDoubleConstant (double value) : value_ (value) {}

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;
  void Print (std::ostream &stream) const override;

  int
  getValue () const override
  {
    return value_;
  };
  TypeSpecifier
  getType (Context &) const override
  {
    return TypeSpecifier::INT;
  }
  std::variant<int, float, double>
  GetConst () const override
  {
    return value_;
  }
};

} // namespace ast

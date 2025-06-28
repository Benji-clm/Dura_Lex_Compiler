#pragma once

#include "ast_node.hpp"

namespace ast
{
class Case : public Node
{
private:
  NodePtr constant_expression_;
  NodePtr statement_;

public:
  Case (NodePtr constant_expression, NodePtr statement)
      : constant_expression_ (std::move (constant_expression)),
        statement_ (std::move (statement))
  {
  }

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;
  void Print (std::ostream &stream) const override;

  TypeSpecifier
  getType (Context &context) const override
  {
    return statement_->getType (context);
  }

  int
  EvalConstExpr (Context &context) const override
  {
    return constant_expression_->EvalConstExpr (context);
  }

  void getCases (Context &context) const override;

  bool
  isDefault () const override
  {
    return statement_->isDefault ();
  }
};

class DefaultCase : public Node
{
private:
  NodePtr statement_;

public:
  DefaultCase (NodePtr statement) : statement_ (std::move (statement)) {}

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;
  void Print (std::ostream &stream) const override;

  TypeSpecifier
  getType (Context &context) const override
  {
    return statement_->getType (context);
  }

  bool
  isDefault () const override
  {
    return true;
  }
};
}

#pragma once

#include "ast_node.hpp"

namespace ast
{
class Scope : public Node
{
private:
  NodePtr declaration_list;
  NodePtr statement_list;

public:
  Scope (NodePtr declaration_list_, NodePtr statement_list_)
      : declaration_list (std::move (declaration_list_)),
        statement_list (std::move (statement_list_))
  {
  }

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;
  void Print (std::ostream &stream) const override;

  std::vector<std::variant<int, float, double> >
  GetConstList () const override
  {
    return statement_list->GetConstList ();
  };

  void
  getCases (Context &context) const override
  {
    statement_list->getCases (context);
  }

  bool
  isDefault () const override
  {
    return statement_list->isDefault ();
  }
};
}

#pragma once

#include "ast_node.hpp"

namespace ast
{
class ArrayIndex : public Node
{
private:
  NodePtr identifier_;
  NodePtr index_;

public:
  ArrayIndex (NodePtr identifier, NodePtr index)
      : identifier_ (std::move (identifier)), index_ (std::move (index))
  {
  }

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;
  void Print (std::ostream &stream) const override;

  std::string
  getIdentifier () const override
  {
    return identifier_->getIdentifier ();
  }

  void EmitADDRESS (std::ostream &, Context &, std::string &) const override;
};
}

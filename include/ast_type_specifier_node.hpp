#pragma once

#include "ast_node.hpp"

namespace ast
{
class TypeSpecifierNode : public Node
{
private:
  TypeSpecifier type_;

public:
  TypeSpecifierNode (TypeSpecifier type) : type_ (type) {}

  TypeSpecifier
  getType (Context &) const override
  {
    return type_;
  }

  virtual void
  EmitRISC (std::ostream &, Context &, std::string &) const override
  {
    // Simple type nodes don't typically need to emit anything themselves
    // TODO: the sizeof bullshit
  }

  virtual void
  Print (std::ostream &stream) const override
  {
    stream << "TypeSpecifier: " << static_cast<int> (type_) << "\n";
  }
};
}

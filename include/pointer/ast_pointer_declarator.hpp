#pragma once

#include "ast_context.hpp"
#include "ast_node.hpp"
#include "ast_type_specifier.hpp"

#include <iostream>
#include <string>

namespace ast
{

class Declarator : public Node
{
public:
  virtual ~Declarator () {}
  virtual std::string getIdentifier () const = 0;
};

class PointerDeclarator : public Declarator
{
private:
  NodePtr pointer;           // Represents the '*' operator (if needed)
  NodePtr direct_declarator; // The direct declarator (e.g. the identifier)
public:
  PointerDeclarator (NodePtr _pointer, NodePtr _direct_declarator)
      : pointer (std::move (_pointer)),
        direct_declarator (std::move (_direct_declarator))
  {
  }

  std::string
  getIdentifier () const override
  {
    return direct_declarator->getIdentifier ();
  }

  void
  Print (std::ostream &stream) const override
  {
    if (pointer)
      pointer->Print (stream);
    if (direct_declarator)
      direct_declarator->Print (stream);
  }

  void
  EmitRISC (std::ostream &stream, Context &context,
            std::string &dest_reg) const override
  {
    context.setIsCurrentPointer (true);
    direct_declarator->EmitRISC (stream, context, dest_reg);
    context.setIsCurrentPointer (false);
  }

  bool
  isPointer () const override
  {
    return true;
  }
};

} // namespace ast

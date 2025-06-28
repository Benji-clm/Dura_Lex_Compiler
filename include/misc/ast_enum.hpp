#pragma once

#include "ast_node.hpp"

namespace ast
{

class Enum : public Node
{
private:
  std::string identifier_;
  NodePtr enumerator_;

public:
  Enum (std::string identifier, NodePtr enumerator)
      : identifier_ (std::move (identifier)),
        enumerator_ (std::move (enumerator))
  {
  }

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;
  void Print (std::ostream &stream) const override;

  TypeSpecifier
  getType (Context &) const override
  {
    return TypeSpecifier::INT;
  }

  std::string
  getIdentifier () const override
  {
    return identifier_;
  }
};

class EnumValue : public Node
{
private:
  std::string identifier_;
  NodePtr value_;

public:
  EnumValue (std::string identifier, NodePtr value)
      : identifier_ (identifier), value_ (std::move (value))
  {
  }

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;
  void Print (std::ostream &stream) const override;

  TypeSpecifier
  getType (Context &) const override
  {
    return TypeSpecifier::INT;
  }

  std::string
  getIdentifier () const override
  {
    return identifier_;
  }
};

}

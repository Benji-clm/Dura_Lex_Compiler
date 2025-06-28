#pragma once

#include "ast_node.hpp"

namespace ast
{

class TypeDef : public Node
{
private:
  NodePtr typeName_;
  NodePtr baseType_;

public:
  TypeDef (NodePtr baseType, NodePtr typeName)
      : typeName_ (std::move (typeName)), baseType_ (std::move (baseType))
  {
  }

  void
  EmitRISC (std::ostream &, Context &, std::string &) const override
  {
    // Do nothing - typedef doesn't generate code
  }

  void
  Print (std::ostream &stream) const override
  {
    stream << "typedef ";
    baseType_->Print (stream);
    stream << " ";
    typeName_->Print (stream);
  }
};

class TypeDefNode : public Node
{
private:
  std::string type_def_name_;

public:
  TypeDefNode (std::string type_def_name)
      : type_def_name_ (std::move (type_def_name))
  {
  }

  void
  EmitRISC (std::ostream &, Context &, std::string &) const override
  {
    // Do nothing - typedef doesn't generate code
  }

  void
  Print (std::ostream &stream) const override
  {
    stream << type_def_name_ << std::endl;
  }

  TypeSpecifier
  getType (Context &)
  {
    if (type_def_name_ == "int")
      return TypeSpecifier::INT;
    if (type_def_name_ == "float")
      return TypeSpecifier::FLOAT;
    if (type_def_name_ == "char")
      return TypeSpecifier::CHAR;
    if (type_def_name_ == "double")
      return TypeSpecifier::DOUBLE;
    if (type_def_name_ == "void")
      return TypeSpecifier::VOID;
  }
};
}

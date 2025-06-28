#pragma once

#include "../ast_node.hpp"

namespace ast
{
class Assignment : public Node
{
private:
  NodePtr unary_expression;
  AssignmentOp Assign_op;
  NodePtr assignment_expression;

public:
  Assignment (NodePtr unary_expression_, AssignmentOp Assign_op_,
              NodePtr assignment_expression_)
      : unary_expression (std::move (unary_expression_)),
        Assign_op (Assign_op_),
        assignment_expression (std::move (assignment_expression_))
  {
  }

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;
  void Print (std::ostream &stream) const override;
  std::string
  getIdentifier () const override
  {
    return unary_expression->getIdentifier ();
  }
  TypeSpecifier
  getType (Context &context) const override
  {
    return unary_expression->getType (context);
  }
};
}

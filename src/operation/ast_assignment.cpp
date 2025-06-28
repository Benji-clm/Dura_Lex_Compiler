#include "operation/ast_assignment.hpp"
#include <cmath>
namespace ast
{

void
Assignment::EmitRISC (std::ostream &stream, Context &context,
                      std::string &dest_reg) const
{
  if (Assign_op == AssignmentOp::ASSIGN)
    {
      context.setLeftOp (true);
      unary_expression->EmitRISC (stream, context, dest_reg);
      context.setLeftOp (false);
    }
  else
    {
      unary_expression->EmitRISC (stream, context, dest_reg);
    }
  TypeSpecifier type = unary_expression->getType (context);

  std::string right_reg = context.allocate_reg (type);
  assignment_expression->EmitRISC (stream, context, right_reg);

  std::string identifier = unary_expression->getIdentifier ();

  switch (Assign_op)
    {
    case AssignmentOp::ASSIGN:
      if (context.isArray (identifier))
        {
          context.store (stream, identifier, right_reg, dest_reg);
        }
      else if (unary_expression->isPointer ())
        {
          stream << "sw " << right_reg << ",0(" << dest_reg << ")"
                 << std::endl;
        }
      else
        {
          context.store (stream, identifier, right_reg);
        }
      break;
    default:
      std::string tmp_reg = context.allocate_reg (type);
      if (context.isArray (identifier))
        {
          context.load (stream, identifier, tmp_reg, dest_reg);
        }
      switch (Assign_op)
        {
        case AssignmentOp::ADD_ASSIGN:
          if (context.isArray (identifier))
            {

              stream << "add " << tmp_reg << "," << tmp_reg << "," << right_reg
                     << std::endl;
              context.store (stream, identifier, tmp_reg, dest_reg);
            }
          else if (context.get_is_pointer (identifier))
            {

              int bytes = context.getBytes (type);
              stream << "slli " << right_reg << "," << right_reg << ","
                     << std::log2 (bytes) << std::endl;
              stream << "add " << right_reg << "," << dest_reg << ","
                     << right_reg << std::endl;
              context.store (stream, identifier, right_reg);
            }
          else
            {

              stream << "add " << dest_reg << "," << dest_reg << ","
                     << right_reg << std::endl;
              context.store (stream, identifier, dest_reg);
            }
          break;
        case AssignmentOp::SUB_ASSIGN:
          if (context.isArray (identifier))
            {

              stream << "sub " << tmp_reg << "," << tmp_reg << "," << right_reg
                     << std::endl;
              context.store (stream, identifier, tmp_reg, dest_reg);
            }
          else if (context.get_is_pointer (identifier))
            {

              int bytes = context.getBytes (type);
              stream << "slli " << right_reg << "," << right_reg << ","
                     << std::log2 (bytes) << std::endl;
              stream << "sub " << right_reg << "," << dest_reg << ","
                     << right_reg << std::endl;
              context.store (stream, identifier, right_reg);
            }
          else
            {
              stream << "sub " << dest_reg << "," << dest_reg << ","
                     << right_reg << std::endl;
              context.store (stream, identifier, dest_reg);
            }
          break;
        case AssignmentOp::MUL_ASSIGN:
          if (context.isArray (identifier))
            {
              stream << "mul " << tmp_reg << "," << tmp_reg << "," << right_reg
                     << std::endl;
              context.store (stream, identifier, tmp_reg, dest_reg);
            }
          else if (context.get_is_pointer (identifier))
            {

              int bytes = context.getBytes (type);
              stream << "slli " << right_reg << "," << right_reg << ","
                     << std::log2 (bytes) << std::endl;
              stream << "mul " << right_reg << "," << dest_reg << ","
                     << right_reg << std::endl;
              stream << "sw " << right_reg << ",0(" << dest_reg << ")"
                     << std::endl;
            }
          else
            {

              stream << "mul " << dest_reg << "," << dest_reg << ","
                     << right_reg << std::endl;
              context.store (stream, identifier, dest_reg);
            }
          break;
        case AssignmentOp::DIV_ASSIGN:
          if (context.isArray (identifier))
            {

              stream << "div " << tmp_reg << "," << tmp_reg << "," << right_reg
                     << std::endl;
              context.store (stream, identifier, tmp_reg, dest_reg);
            }
          else if (context.get_is_pointer (identifier))
            {

              int bytes = context.getBytes (type);
              stream << "slli " << right_reg << "," << right_reg << ","
                     << std::log2 (bytes) << std::endl;
              stream << "div " << right_reg << "," << dest_reg << ","
                     << right_reg << std::endl;
              stream << "sw " << right_reg << ",0(" << dest_reg << ")"
                     << std::endl;
            }
          else
            {

              stream << "div " << dest_reg << "," << dest_reg << ","
                     << right_reg << std::endl;
              context.store (stream, identifier, dest_reg);
            }
          break;
        case AssignmentOp::MOD_ASSIGN:
          if (context.isArray (identifier))
            {
              stream << "rem " << tmp_reg << "," << tmp_reg << "," << right_reg
                     << std::endl;
              context.store (stream, identifier, tmp_reg, dest_reg);
            }
          else if (context.get_is_pointer (identifier))
            {

              int bytes = context.getBytes (type);
              stream << "slli " << right_reg << "," << right_reg << ","
                     << std::log2 (bytes) << std::endl;
              stream << "rem " << right_reg << "," << dest_reg << ","
                     << right_reg << std::endl;
              stream << "sw " << right_reg << ",0(" << dest_reg << ")"
                     << std::endl;
            }
          else
            {
              stream << "rem " << dest_reg << "," << dest_reg << ","
                     << right_reg << std::endl;
              context.store (stream, identifier, dest_reg);
            }
          break;
        case AssignmentOp::RIGHT_ASSIGN:
          if (context.isArray (identifier))
            {
              stream << "sra " << tmp_reg << "," << tmp_reg << "," << right_reg
                     << std::endl;
              context.store (stream, identifier, tmp_reg, dest_reg);
            }
          else
            {
              stream << "sra " << dest_reg << "," << dest_reg << ","
                     << right_reg << std::endl;
              context.store (stream, identifier, dest_reg);
            }
          break;
        case AssignmentOp::LEFT_ASSIGN:
          if (context.isArray (identifier))
            {
              stream << "sll " << tmp_reg << "," << tmp_reg << "," << right_reg
                     << std::endl;
              context.store (stream, identifier, tmp_reg, dest_reg);
            }
          else
            {
              stream << "sll " << dest_reg << "," << dest_reg << ","
                     << right_reg << std::endl;
              context.store (stream, identifier, dest_reg);
            }
          break;
        case AssignmentOp::AND_ASSIGN:
          if (context.isArray (identifier))
            {
              stream << "and " << tmp_reg << "," << tmp_reg << "," << right_reg
                     << std::endl;
              context.store (stream, identifier, tmp_reg, dest_reg);
            }
          else
            {
              stream << "and " << dest_reg << "," << dest_reg << ","
                     << right_reg << std::endl;
              context.store (stream, identifier, dest_reg);
            }
          break;
        case AssignmentOp::XOR_ASSIGN:
          if (context.isArray (identifier))
            {
              stream << "xor " << tmp_reg << "," << tmp_reg << "," << right_reg
                     << std::endl;
              context.store (stream, identifier, tmp_reg, dest_reg);
            }
          else
            {
              stream << "xor " << dest_reg << "," << dest_reg << ","
                     << right_reg << std::endl;
              context.store (stream, identifier, dest_reg);
            }
          break;
        case AssignmentOp::OR_ASSIGN:
          if (context.isArray (identifier))
            {
              stream << "or " << tmp_reg << "," << tmp_reg << "," << right_reg
                     << std::endl;
              context.store (stream, identifier, tmp_reg, dest_reg);
            }
          else
            {
              stream << "or " << dest_reg << "," << dest_reg << ","
                     << right_reg << std::endl;
              context.store (stream, identifier, dest_reg);
            }
          break;
        default:
          throw std::runtime_error (
              "ast_assignment.cpp - Undefined Assignment Operator");
          break;
        }
      context.deallocate_reg (tmp_reg);
    }
  // case AssignmentOp::MUL_ASSIGN:
}

void
Assignment::Print (std::ostream &stream) const
{
  unary_expression->Print (stream);
  stream << "=" << std::endl;
  assignment_expression->Print (stream);
}
}

/*
Assign types:

">>="			 {return(RIGHT_ASSIGN);}
"<<="      {return(LEFT_ASSIGN);}
"+="			 {return(ADD_ASSIGN);}
"-="       {return(SUB_ASSIGN);}
"*="       {return(MUL_ASSIGN);}
"/="			 {return(DIV_ASSIGN);}
"%="			 {return(MOD_ASSIGN);}
"&="       {return(AND_ASSIGN);}
"^="			 {return(XOR_ASSIGN);}
"|="       {return(OR_ASSIGN);}
*/

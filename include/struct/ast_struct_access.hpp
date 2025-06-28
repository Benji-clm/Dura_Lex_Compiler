#pragma once

#include "../ast_node.hpp"
#include "../ast_context.hpp"

namespace ast {

class StructAccess : public Node
{
private:
    NodePtr postfix_expression;
    NodePtr identifier;

public:
    StructAccess(NodePtr _postfix_expression, NodePtr _identifier)
        : postfix_expression(std::move(_postfix_expression)), identifier(std::move(_identifier))
 {}

    void Print(std::ostream &stream) const override;
    void EmitRISC(std::ostream &stream, Context &context, std::string &dest_reg) const override;
    TypeSpecifier getType(Context &context) const override;
    std::string getIdentifier() const override
    {
        return postfix_expression->getIdentifier()
             + "."
             + identifier->getIdentifier();
    }
};

} // namespace ast

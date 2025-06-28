#pragma once

#include "../ast_node.hpp"
#include "../ast_context.hpp"

namespace ast {

    class StructDeclaration : public Node
    {
    private:
        NodePtr specifier_qualifier_list;
        NodePtr struct_declarator_list;

    public:
        StructDeclaration(NodePtr specifier_qualifier_list, NodePtr _struct_declarator_list)
            : specifier_qualifier_list(std::move(specifier_qualifier_list)), struct_declarator_list(std::move(_struct_declarator_list))
        {}


        void EmitRISC(std::ostream &stream, Context &context, std::string &dest_reg) const override;
        void Print(std::ostream &stream) const override;
        TypeSpecifier getType(Context &) const override { return TypeSpecifier::STRUCT; }
    };
} // namespace ast

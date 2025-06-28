#pragma once

#include "../ast_node.hpp"
#include "../ast_context.hpp"

namespace ast
{

class StructDefinition: public Node
{
private:
    std::string identifier;
    NodePtr struct_declaration_list;
public:
     StructDefinition(std::string _identifier, NodePtr _struct_declaration_list)
     : identifier(_identifier),
          struct_declaration_list(std::move(_struct_declaration_list))
    {}

    void EmitRISC(std::ostream &stream, Context &context, std::string &dest_reg) const override;
    void Print(std::ostream &stream) const override;
};


} // namespace ast

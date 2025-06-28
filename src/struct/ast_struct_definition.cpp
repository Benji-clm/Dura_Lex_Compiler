#include "struct/ast_struct_definition.hpp"

namespace ast {

void StructDefinition::EmitRISC(std::ostream &stream, Context &context, std::string &dest_reg) const
{

    context.clearStructMembers();

    struct_declaration_list->EmitRISC(stream, context, dest_reg);

    context.defineStruct(identifier, TypeSpecifier::STRUCT);
}

void StructDefinition::Print(std::ostream &stream) const
{
    stream << "struct " << identifier << " {\n";
    struct_declaration_list->Print(stream);
    stream << "};\n";
}

} // namespace ast

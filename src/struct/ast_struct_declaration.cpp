#include "struct/ast_struct_declaration.hpp"

namespace ast {

    void StructDeclaration::Print(std::ostream &stream) const
    {
        specifier_qualifier_list->Print(stream);
        stream << " ";
        struct_declarator_list->Print(stream);
        stream << ";" << std::endl;
    }


    void StructDeclaration::EmitRISC(std::ostream &stream, Context &context, std::string &dest_reg) const
    {
        TypeSpecifier type = specifier_qualifier_list->getType(context);


        context.setCurrentType(type);

        struct_declarator_list->EmitRISC(stream, context, dest_reg);
    }


} // namespace ast

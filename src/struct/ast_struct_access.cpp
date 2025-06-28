#include "struct/ast_struct_access.hpp"

namespace ast {

void StructAccess::Print(std::ostream &stream) const
{
    postfix_expression->Print(stream);
    stream << ".";
    identifier->Print(stream);
}
void StructAccess::EmitRISC(std::ostream &stream, Context &context, std::string &dest_reg) const
{
    std::string full_identifier = getIdentifier();
    TypeSpecifier type = getType(context);

    if (!context.has_stack_entry(full_identifier)) {
        context.allocate_stack(full_identifier, type);
    }

    int stack_loc = context.getLoc(full_identifier);

    switch (type)
    {
        case TypeSpecifier::INT:
        case TypeSpecifier::UNSIGNED_INT:
            stream << "lw " << dest_reg << ", " << stack_loc << "(s0)" << std::endl;
            break;

        case TypeSpecifier::FLOAT:
            stream << "flw " << dest_reg << ", " << stack_loc << "(s0)" << std::endl;
            break;

        case TypeSpecifier::DOUBLE:
            stream << "fld " << dest_reg << ", " << stack_loc << "(s0)" << std::endl;
            break;

        default:
            stream << "lw " << dest_reg << ", " << stack_loc << "(s0)" << std::endl;
            break;
    }
}


TypeSpecifier StructAccess::getType(Context &context) const
{
    std::string identifier = getIdentifier();
    return context.getType(identifier);

}

} // namespace ast

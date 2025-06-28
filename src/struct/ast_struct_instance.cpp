#include "struct/ast_struct_instance.hpp"

namespace ast {

void StructInstance::Print(std::ostream &stream) const
    {
        stream << "struct " << identifier;
    }

    void StructInstance::EmitRISC(std::ostream &, Context &context, std::string &) const
    {

        if (context.hasStruct(identifier))
        {
            auto instance = context.getStructInstance(identifier);
        }
        else
        {
            throw std::runtime_error("Struct '" + identifier + "' is not defined.");
        }
    }


}

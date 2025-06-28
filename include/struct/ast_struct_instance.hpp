#pragma once
#include "../ast_node.hpp"
#include "../ast_context.hpp"

namespace ast
{

class StructInstance : public Node
{
public:
    StructInstance(std::string _identifier) : identifier(_identifier) {}
    void EmitRISC(std::ostream &stream, Context &context, std::string &dest_reg) const override;
    void Print(std::ostream &stream) const override;

private:
    std::string identifier;
};

}

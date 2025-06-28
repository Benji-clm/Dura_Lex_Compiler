#include "ast_node.hpp"
#include "ast_context.hpp"
#include <iostream>
#include <memory>
#include <vector>

namespace ast {

void NodeList::PushBack(NodePtr item)
{
    std::cerr << "[DEBUG] Pushing node at address " << item.get() << "\n";
    nodes_.push_back(std::move(item));
}

void NodeList::EmitRISC(std::ostream &stream, Context &context, std::string &dest_reg) const
{
    std::cerr << "[DEBUG] NodeList::EmitRISC called; number of nodes: " << nodes_.size() << "\n";
    for (const auto &node : nodes_) {
        if (node == nullptr) {
            std::cerr << "[DEBUG] Skipping null node in EmitRISC\n";
            continue;
        }
        std::cerr << "[DEBUG] Emitting RISC for node at address " << node.get() << "\n";
        node->EmitRISC(stream, context, dest_reg);
    }
}

void NodeList::Print(std::ostream &stream) const
{
    std::cerr << "[DEBUG] NodeList::Print called; number of nodes: " << nodes_.size() << "\n";
    for (const auto &node : nodes_) {
        if (node == nullptr) {
            std::cerr << "[DEBUG] Skipping null node in Print\n";
            continue;
        }
        std::cerr << "[DEBUG] Printing node at address " << node.get() << "\n";
        node->Print(stream);
        stream << "\n";
    }
}



} // namespace ast

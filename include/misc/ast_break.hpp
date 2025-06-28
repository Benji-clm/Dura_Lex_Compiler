#pragma once

#include "ast_node.hpp"

namespace ast
{
class Break : public Node
{
private:
public:
  Break () {}

  void EmitRISC (std::ostream &stream, Context &context,
                 std::string &dest_reg) const override;
  void Print (std::ostream &stream) const override;
};
}

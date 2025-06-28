#pragma once

#include "ast_node.hpp"

namespace ast
{
class Continue : public Node
{
private:
  // has no member, simply a keyword

public:
  Continue () {};

  void
  EmitRISC (std::ostream &stream, Context &context,
            std::string &) const override
  {
    std::string jump_label = context.getContinueLabel ();
    stream << "j " << jump_label << std::endl;
  }

  void
  Print (std::ostream &stream) const override
  {
    stream << "continue" << std::endl;
  }
};
}

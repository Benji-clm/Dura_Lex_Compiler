#include "array/ast_array_init.hpp"
#include <variant>
#include <vector>

namespace ast
{
void
ArrayInit::EmitRISC (std::ostream &stream, Context &context,
                     std::string &dest_reg) const
{
  TypeSpecifier type = context.getCurrentType ();
  std::vector<std::variant<int, float, double> > consts;
  consts = initalizer_->GetConstList ();

  int bytes = context.getBytes (type);
  int base = context.getArrayLocBase ();

  int size = static_cast<int> (consts.size ());

  for (int i = 0; i < size; i++)
    {
      int loc = base + bytes * i;

      std::string store_instr = context.get_store_instr (type);

      switch (type)
        {
        case TypeSpecifier::CHAR:
        case TypeSpecifier::INT:
          stream << "li " << dest_reg << "," << std::get<int> (consts[i])
                 << std::endl;
          stream << store_instr << " " << dest_reg << "," << loc << "(s0)"
                 << std::endl;
          break;
        case TypeSpecifier::FLOAT:
          stream << "li " << dest_reg << "," << std::get<float> (consts[i])
                 << std::endl;
          stream << store_instr << " " << dest_reg << "," << loc << "(s0)"
                 << std::endl;
          break;
        case TypeSpecifier::DOUBLE:
          stream << "li " << dest_reg << "," << std::get<double> (consts[i])
                 << std::endl;
          stream << store_instr << " " << dest_reg << "," << loc << "(s0)"
                 << std::endl;
          break;
        default:
          throw std::runtime_error ("ArrayInit - Bitchass bitch you tryna "
                                    "load the wrong gang into that array");
        }
    }
}

void
ArrayInit::Print (std::ostream &stream) const
{
  initalizer_->Print (stream);
  stream << ",";
}
}

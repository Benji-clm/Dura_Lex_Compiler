#include "ast_init_declarator.hpp"

namespace ast
{
void
InitDeclarator::EmitRISC (std::ostream &stream, Context &context,
                          std::string &dest_reg) const
{

  declarator->EmitRISC (stream, context, dest_reg);

  if (declarator->isFunctionDeclarator ())
    {
      return;
    }

  std::string identifier_ = declarator->getIdentifier ();

  if (context.isScopeGlobal ())
    {
      if (!declarator->isArray ())
        {
          context.addGlobalVar (identifier_);
        }

      stream << identifier_ << ":" << std::endl;
      if (initializer != nullptr)
        {
          stream << ".word " << initializer->getValue () << std::endl;
        }
      else
        {
          stream << ".zero 4" << std::endl;
        }
    }
  else
    {
      TypeSpecifier type = context.getCurrentType ();
      if (declarator->isPointer ())
        {
          context.setPointerInit (true);
        }
      else
        {
          context.setPointerInit (false);
        }

      // if its an array we declare and allocate within array declaration
      // class.
      if (!declarator->isArray ())
        {
          context.allocate_stack (identifier_, type);
        }
      if (initializer != nullptr)
        {
          initializer->EmitRISC (stream, context, dest_reg);
          if (!declarator->isArray ())
            {
              context.store (stream, identifier_, dest_reg);
            }
        }
    }
}

void
InitDeclarator::Print (std::ostream &stream) const
{
  //   stream << "InitDeclarator" << std::endl;
  declarator->Print (stream);
  if (initializer != nullptr)
    {
      initializer->Print (stream);
    }
}
}
// clang-format off
/*
|    Section    |                        Use Case                              |
|---------------|--------------------------------------------------------------|
| **`.bss`**    | Uninitialized global/static variables (default for `int x;`) |
| **`.data`**   | Initialized global/static variables (`int x = 5;`)           |
| **`.sdata`**  | Small initialized global/static variables (≤ 8 bytes)        |
| **`.sbss`**   | Small uninitialized global/static variables (≤ 8 bytes)      |
| **`.rodata`** | Read-only data (`const int x = 5;`)                          |
*/
// clang-format on

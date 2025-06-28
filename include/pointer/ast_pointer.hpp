#pragma once

#include "ast_context.hpp"
#include "ast_node.hpp"

#include <iostream>
#include <memory>
#include <string>

namespace ast
{

// This node represents a pointer type or a pointer operator (e.g. "*" in an
// expression). It holds a pointer to another Node, which might be a type node
// or an expression.
class Pointer : public Node
{
private:
  // You could alternatively use NodePtr (std::unique_ptr<const Node>) for
  // automatic memory management.
  Node *pointer;

public:
  // Constructor: takes ownership of the pointer.
  Pointer (Node *_pointer) : pointer (_pointer) {}

  // Destructor: free the held pointer.
  virtual ~Pointer () { delete pointer; }

  // Print the pointer operator and the underlying node.
  void
  Print (std::ostream &stream) const override
  {
    stream << "*";
    if (pointer != nullptr)
      {
        pointer->Print (stream);
      }
  }

  // EmitRISC: In this simple example, pointer types do not generate runtime
  // code. If you need to generate code (for example for dereference or
  // address-of operators), you would implement that here.
  void
  EmitRISC (std::ostream &stream, Context &context,
            std::string &dest_reg) const override
  {
    (void)stream;   // Mark as unused
    (void)context;  // Mark as unused
    (void)dest_reg; // Mark as unused
  }
};

} // namespace ast

/* clang-format off */
// ========================= Valid Pointer Operations =========================
//
// Assignment ( = )
//   ptr = &x;              // Assigns the address of 'x' to the pointer 'ptr'.
//
// Dereferencing ( * )
//   value = *ptr;          // Accesses the value stored at the pointer’s address.
//
// Address-of ( & )
//   ptr = &x;              // Retrieves the address of a variable 'x'.
//
// Pointer Addition ( ptr + n )
//   ptr = arr + 2;         // Moves the pointer forward by 'n' elements
//                          // (automatically scaled by element size).
//
// Pointer Subtraction ( ptr - n )
//   ptr = arr + 5 - 2;     // Moves the pointer backward by 'n' elements.
//
// Pointer Difference ( ptr1 - ptr2 )
//   int diff = ptr2 - ptr1; // Computes the number of elements between two pointers
//                          // in the same array.
//
// Pointer Comparison ( ==, !=, >, <, >=, <= )
//   if (ptr1 == ptr2) { ... } // Compares pointer addresses.
//
// Pointer Increment ( ++ )
//   ptr++;                // Moves pointer to the next element (ptr = ptr + 1).
//
// Pointer Decrement ( -- )
//   ptr--;                // Moves pointer to the previous element (ptr = ptr - 1).
//
// Compound Addition ( += )
//   ptr += 3;             // Moves pointer forward by '3' elements.
//
// Compound Subtraction ( -= )
//   ptr -= 2;             // Moves pointer backward by '2' elements.
//
// ============================================================================
/* clang-format on */

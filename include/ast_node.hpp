#pragma once

#include <iostream>
#include <memory>
#include <optional>
#include <variant>
#include <vector>

#include "ast_context.hpp"

namespace ast
{

class Node
{
public:
  virtual ~Node ()
  {
    std::cerr << "[DEBUG] Destroying Node at address " << this << "\n";
  }

  virtual void EmitRISC (std::ostream &stream, Context &context,
                         std::string &dest_reg) const
      = 0;
  virtual void Print (std::ostream &stream) const = 0;

  virtual void
  EmitADDRESS (std::ostream &, Context &, std::string &) const
  {
  }

  virtual std::string
  getIdentifier () const
  {
    return {};
  };

  virtual int
  getValue () const
  {
    return 0;
  };

  virtual int
  EvalConstExpr (Context &) const
  {
    return 0;
  };

  virtual TypeSpecifier
  getType (Context &) const
  {
    return {};
  };

  virtual std::string
  get_index_register () const
  {
    return {};
  }

  virtual bool
  isFunctionDeclarator () const
  {
    return false;
  }

  virtual bool
  isArray () const
  {
    return false;
  }

  virtual std::variant<int, float, double>
  GetConst () const
  {
    return 0;
  };

  virtual std::vector<std::variant<int, float, double> >
  GetConstList () const
  {
    return {};
  };

  virtual void getCases (Context &) const {};

  virtual bool
  isDefault () const
  {
    return false;
  }

  virtual bool
  isPointer () const
  {
    return false;
  }
};

// If you don't feel comfortable using std::unique_ptr, you can switch
// NodePtr to be defined as a raw pointer instead here and your project
// should still compile, although you'll need to add destructors to avoid
// leaking memory (and get rid of the now unnecessary std::move-s)
using NodePtr = std::unique_ptr<const Node>;

class NodeList : public Node
{
private:
  std::vector<NodePtr> nodes_;

public:
  NodeList (NodePtr first_node) { nodes_.push_back (std::move (first_node)); }

  void PushBack (NodePtr item);
  virtual void EmitRISC (std::ostream &stream, Context &context,
                         std::string &dest_reg) const override;
  virtual void Print (std::ostream &stream) const override;

  virtual std::vector<std::variant<int, float, double> >
  GetConstList () const override
  {
    std::vector<std::variant<int, float, double> > consts;
    for (const auto &node : nodes_)
      {
        if (node == nullptr)
          {
            continue;
          }
        consts.push_back (node->GetConst ());
      }
    return consts;
  }

  virtual std::variant<int, float, double>
  GetConst () const override
  {
    if (!nodes_.empty ())
      {
        return nodes_[0]->GetConst ();
      }
    return 0;
  };

  virtual void
  getCases (Context &context) const override
  {
    for (const auto &node : nodes_)
      {
        if (node)
          {
            node->getCases (context); // Delegate to each node
          }
      }
  };

  virtual bool
  isDefault () const override
  {
    for (const auto &node : nodes_)
      {
        if (node && node->isDefault ())
          {
            return true;
          }
      }
    return false;
  }

  virtual bool
  isPointer () const override
  {
    for (const auto &node : nodes_)
      {
        if (node && node->isDefault ())
          {
            return true;
          }
      }
    return false;
  }
};

} // namespace ast

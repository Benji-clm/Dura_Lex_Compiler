#pragma once

#include <stdexcept>
#include <string_view>

namespace ast
{

  enum class TypeSpecifier
  {
    INT,
    FLOAT,
    DOUBLE,
    CHAR,
    VOID,
    UNSIGNED_INT,
    UNSIGNED_CHAR,
    UNSIGNED_SHORT,
    UNSIGNED_LONG,
    STRUCT
  };

template <typename LogStream>
LogStream &
operator<< (LogStream &ls, const TypeSpecifier &type)
{
  // Let the lambda deduce its return type.
  const auto TypeToString = [&type] {
    switch (type)
      {
      case TypeSpecifier::INT:
        return "int";
      case TypeSpecifier::FLOAT:
        return "float";
      case TypeSpecifier::DOUBLE:
        return "double";
      case TypeSpecifier::CHAR:
        return "char";
      case TypeSpecifier::VOID:
        return "void";
      case TypeSpecifier::UNSIGNED_INT:
        return "unsigned int";
      case TypeSpecifier::UNSIGNED_CHAR:
        return "unsigned char";
      case TypeSpecifier::UNSIGNED_SHORT:
        return "unsigned short";
      case TypeSpecifier::UNSIGNED_LONG:
        return "unsigned long";
      case TypeSpecifier::STRUCT:
        return "struct";
      default:
        throw std::runtime_error ("Unexpected type specifier");
      }
  };
  return ls << TypeToString ();
}

} // namespace ast

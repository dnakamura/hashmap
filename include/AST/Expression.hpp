#ifndef AST_EXPRESSION_HPP
#define AST_EXPRESSION_HPP

#include <memory>  // for unique_ptr
#include <string_view>
#include "Value.hpp"

namespace ast {
class Expression;
using Node_t = std::unique_ptr<Expression>;
class Expression {
 public:
  enum Kind { VARIABLE, INDEX, ASSIGNMENT, STRING_LIT, INT_LIT };

  constexpr Expression(Kind kind, std::string_view &raw)
      : kind_(kind), raw_(raw) {}
  virtual ~Expression() {}

  virtual Value Eval() = 0;
  constexpr Kind getKind() const { return kind_; }
  std::string_view getRaw() const { return raw_; }

 private:
  const Kind kind_;
  std::string_view raw_;
};
}  // namespace ast
#endif

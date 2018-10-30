#ifndef AST_STRING_LITERAL_HPP
#define AST_STRING_LITERAL_HPP
#include "Expression.hpp"

namespace ast {
class StringLiteral : public Expression {
 public:
  StringLiteral(std::string_view raw, std::string_view value)
      : Expression(Kind::STRING_LIT, raw), value_(value) {}
  static bool classof(const Expression *expr) {
    return expr->getKind() == Kind::STRING_LIT;
  }
  Value Eval() override;

 private:
  std::string_view value_;
};
}  // namespace ast

#endif

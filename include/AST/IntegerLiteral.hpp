#ifndef AST_INTEGER_LITERAL_HPP
#define AST_INTEGER_LITERAL_HPP

#include "Expression.hpp"

namespace ast {
class IntegerLiteral : public Expression {
 public:
  IntegerLiteral(std::string_view str, std::int64_t value)
      : Expression(Kind::INT_LIT, str), value_(value) {}
  static bool classof(const Expression *expr) {
    return expr->getKind() == Kind::INT_LIT;
  }
  Value Eval() override;

 private:
  std::int64_t value_;
};
}  // namespace ast
#endif

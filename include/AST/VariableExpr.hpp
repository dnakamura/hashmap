#ifndef AST_VARIABLE_EXPR_HPP
#define AST_VARIABLE_EXPR_HPP

#include "Expression.hpp"

namespace ast {
class VariableExpr : public Expression {
 public:
  VariableExpr(std::string_view name) : Expression(Kind::VARIABLE, name) {}
  Value Eval() override;
  static bool classof(const Expression *expr) {
    return expr->getKind() == Kind::VARIABLE;
  }
};
}  // namespace ast

#endif

#ifndef AST_ASSIGNMENT_EXPR_HPP
#define AST_ASSIGNMENT_EXPR_HPP

#include "Expression.hpp"

namespace ast {
class AssignmentExpr : public Expression {
 public:
  AssignmentExpr(std::string_view str, Node_t lhs, Node_t rhs)
      : Expression(Kind::ASSIGNMENT, str),
        lhs_(std::move(lhs)),
        rhs_(std::move(rhs)) {}
  static bool classof(const Expression *expr) {
    return expr->getKind() == Kind::ASSIGNMENT;
  }
  Value Eval() override;

 private:
  Node_t lhs_;
  Node_t rhs_;
};
}  // namespace ast
#endif

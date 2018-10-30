#ifndef AST_INDEX_EXPR_HPP
#define AST_INDEX_EXPR_HPP

#include "Expression.hpp"

namespace ast {
class IndexExpr : public Expression {
 public:
  IndexExpr(std::string_view str, Node_t &&var, Node_t &&index)
      : Expression(Kind::INDEX, str),
        variable_(std::move(var)),
        index_(std::move(index)) {}

  static bool classof(const Expression *expr) {
    return expr->getKind() == Kind::INDEX;
  }
  Value Eval() override;

 private:
  Node_t variable_;
  Node_t index_;
  friend class AssignmentExpr;
};
}  // namespace ast
#endif

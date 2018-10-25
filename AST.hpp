#pragma once
#include <exception>
#include <memory>
#include <string>
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

class IndexExpr : public Expression {
 public:
  IndexExpr(std::string_view str, Node_t &&var, Node_t &&index)
      : Expression(Kind::INDEX, str),
        variable_(std::move(var)),
        index_(std::move(index)) {}
  // IndexExpr() : Expression(Kind::INDEX) {}
  static bool classof(const Expression *expr) {
    return expr->getKind() == Kind::INDEX;
  }
  Value Eval() override;

 private:
  Node_t variable_;
  Node_t index_;
  friend class AssignmentExpr;
};

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

class VariableExpr : public Expression {
 public:
  VariableExpr(std::string_view name) : Expression(Kind::VARIABLE, name) {}
  Value Eval() override;
  static bool classof(const Expression *expr) {
    return expr->getKind() == Kind::VARIABLE;
  }
};
}  // namespace ast

class ParseError : public std::exception {
 public:
  ParseError(const char *msg) : msg_(msg) {}
  const char *what() const noexcept override { return msg_; }

 private:
  const char *msg_;
};

class EvalError : public std::exception {
 public:
  EvalError(const char *msg) : msg_(msg) {}
  const char *what() const noexcept override { return msg_; }

 private:
  const char *msg_;
};

std::unique_ptr<ast::Expression> Parse(std::string_view str);

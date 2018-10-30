#pragma once
#include <exception>
#include <memory>

#include "AST/AssignmentExpr.hpp"
#include "AST/Expression.hpp"
#include "AST/IndexExpr.hpp"
#include "AST/IntegerLiteral.hpp"
#include "AST/StringLiteral.hpp"
#include "AST/VariableExpr.hpp"

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

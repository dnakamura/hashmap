#pragma once
#include <exception>
#include <memory>
#include <string>
#include <string_view>
#include "Value.hpp"

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

std::unique_ptr<Expression> Parse(std::string_view str);

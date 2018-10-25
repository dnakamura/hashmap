
#include "AST.hpp"
#include <cassert>
#include <cctype>
#include <charconv>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include "Environ.hpp"
#include "Object.hpp"

using Node_t = std::unique_ptr<Expression>;
namespace {
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

class VariableExpr : public Expression {
 public:
  VariableExpr(std::string_view name) : Expression(Kind::VARIABLE, name) {}
  Value Eval() override;
  static bool classof(const Expression *expr) {
    return expr->getKind() == Kind::VARIABLE;
  }
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

class ParseBuffer {
 public:
  using BufferType = std::string_view;
  using Iterator = decltype(std::declval<BufferType>().cbegin());
  using difference_type = BufferType::difference_type;
  using size_type = BufferType::size_type;

 private:
  BufferType buff_;
  Iterator cur_;
  const Iterator end_;

 public:
  ParseBuffer(BufferType buffer)
      : buff_(buffer), cur_(buff_.cbegin()), end_(buff_.cend()) {}
  // ParseBuffer(const BufferType & buffer) : buff_() {}

  Iterator &Pos() { return cur_; }
  bool IsEOF() const { return cur_ == end_; }

  template <typename ConditionFunc>
  void AdvanceWhile(ConditionFunc f) {
    // static_assert<decltype(f(char) == bool)
    while ((cur_ != end_) && f(*cur_)) {
      ++cur_;
    }
  }

  /// Skip whitespace
  void SkipWS() {
    AdvanceWhile([](char ch) { return std::isspace(ch); });
  }

  std::string_view Substring(const Iterator &begin, size_type sz) {
    difference_type start_offset = begin - buff_.cbegin();
    assert(start_offset >= 0);
    assert(sz + start_offset <= buff_.size());
    return buff_.substr(start_offset, sz);
  }

  std::string_view Substring(const Iterator &begin, const Iterator &end) {
    const difference_type diff = end - begin;
    assert(diff >= 0);
    return Substring(begin, diff);
  }
};
}  // namespace

static std::string_view ParseIdentifier(ParseBuffer &buff);
static Node_t ParseVariable(ParseBuffer &buff);
static Node_t ParseStringLiteral(ParseBuffer &buff);
static Node_t ParseIntegerLiteral(ParseBuffer &buff);
static Node_t ParseIndexExpression(ParseBuffer &buff, Node_t lhs);
static Node_t ParseExpression(ParseBuffer &buff);

static std::string_view ParseIdentifier(ParseBuffer &buff) {
  assert(std::isalpha(*buff.Pos()));
  ParseBuffer::Iterator begin = buff.Pos();
  // Advance past the first char
  buff.Pos()++;
  buff.AdvanceWhile([](char ch) { return std::isalnum(ch); });
  return buff.Substring(begin, buff.Pos());
}

static Node_t ParseVariable(ParseBuffer &buff) {
  return std::make_unique<VariableExpr>(ParseIdentifier(buff));
}

static Node_t ParseStringLiteral(ParseBuffer &buff) {
  assert(*buff.Pos() == '"');
  ParseBuffer::Iterator begin = buff.Pos();
  ++buff.Pos();
  buff.AdvanceWhile([](char ch) { return ch != '"'; });
  if (buff.IsEOF()) {
    throw ParseError("Unexpected end of file");
  }
  assert(*buff.Pos() == '"');
  ++buff.Pos();
  std::string_view raw = buff.Substring(begin, buff.Pos());
  std::string_view sValue = raw.substr(1, raw.size() - 2);
  return std::make_unique<StringLiteral>(raw, sValue);
}

static Node_t ParseIntegerLiteral(ParseBuffer &buff) {
  assert(std::isdigit(*buff.Pos()));
  auto begin = buff.Pos();
  buff.AdvanceWhile([](char ch) { return std::isdigit(ch); });
  std::string_view str = buff.Substring(begin, buff.Pos());

  std::int64_t intValue = 0;

  const char *endPtr = &*str.cbegin() + str.size();
  auto rc = std::from_chars(&*str.cbegin(), endPtr, intValue);
  if (rc.ptr != endPtr) {
    throw ParseError("Failed parsing integer");
  }

  return std::make_unique<IntegerLiteral>(str, intValue);
}

static Node_t ParseIndexExpression(ParseBuffer &buff, Node_t lhs) {
  using namespace std;
  assert(*buff.Pos() == '[');
  ++buff.Pos();
  buff.SkipWS();
  Node_t expr = ParseExpression(buff);
  buff.SkipWS();
  if (buff.IsEOF()) {
    throw ParseError("Unexpected end of file");
  }
  assert(*buff.Pos() == ']');
  ++buff.Pos();
  return std::make_unique<IndexExpr>("dummy"sv, std::move(lhs),
                                     std::move(expr));
}

static Node_t ParseExpression(ParseBuffer &buff) {
  buff.SkipWS();
  if (buff.IsEOF()) {
    throw ParseError("Unexpected end of file");
  }

  Node_t expr = nullptr;

  if (std::isalpha(*buff.Pos())) {
    expr = ParseVariable(buff);
  } else if (std::isdigit(*buff.Pos())) {
    expr = ParseIntegerLiteral(buff);
  } else if (*buff.Pos() == '"') {
    expr = ParseStringLiteral(buff);
  } else {
    throw ParseError("Unexpected tokens");
  }
  buff.SkipWS();
  if (buff.IsEOF()) {
    return expr;
  }
  if (*buff.Pos() == '[') {
    return ParseIndexExpression(buff, std::move(expr));
  }
  return expr;
}

// parse a basic expression (no assigments)

static Node_t ParseAssignementExpression(ParseBuffer &buff, Node_t lhs) {
  assert(!buff.IsEOF());
  assert(*buff.Pos() == '=');
  ++buff.Pos();
  buff.SkipWS();
  using namespace std;
  return std::make_unique<AssignmentExpr>("assignDummy"sv, std::move(lhs),
                                          ParseExpression(buff));
}

// PArse a single statement ((at the moment that means only 1 AssignmentExpr
// which must be at the top level
Node_t Parse(std::string_view str) {
  ParseBuffer buff(str);
  Node_t expr = ParseExpression(buff);
  buff.SkipWS();
  if (!buff.IsEOF() && *buff.Pos() == '=') {
    expr = ParseAssignementExpression(buff, std::move(expr));
  }
  buff.SkipWS();

  if (!buff.IsEOF()) {
    throw ParseError("Unexpected tokens");
  }
  return expr;
}

Value IntegerLiteral::Eval() { return value_; }

Value StringLiteral::Eval() { return StringObject::Allocate(value_); }

Value VariableExpr::Eval() {
  using namespace std;
  // We handle the special case of variable named "newtable"
  if (getRaw() == "newtable"sv) {
    HashTable *newTable = new HashTable();
    newTable->Initialize(HashTable::DEFAULT_SIZE);
    return newTable;
  }
  Value key = StringObject::Allocate(getRaw());
  Value *value = globalVariables.Get(key);
  Value retval{nullptr};
  if (value != nullptr) {
    retval = *value;
  }
  // TODO this makes no sense if we are using a gc
  // std::free(key.AsObject());
  return retval;
}

Value AssignmentExpr::Eval() {
  auto lhsKind = lhs_->getKind();
  HashTable *tbl = nullptr;
  Value idx = nullptr;
  if (lhsKind == Kind::INDEX) {
    IndexExpr *indexExpr = static_cast<IndexExpr *>(lhs_.get());

    // we need to set the value in the table
    Value temp = indexExpr->variable_->Eval();
    if (!temp.IsObject()) {
      throw EvalError("TODO: msg");
    }
    if (temp.AsObject()->kind() != Object::Kind::HASHTABLE) {
      throw EvalError("TODO: msg2");
    }
    tbl = static_cast<HashTable *>(temp.AsObject());

    idx = indexExpr->index_->Eval();

  } else if (lhsKind == Kind::VARIABLE) {
    tbl = &globalVariables;
    idx = StringObject::Allocate(lhs_->getRaw());
  } else {
    throw EvalError("Must assign to variable or into table");
  }

  assert(tbl != nullptr);
  assert(tbl->kind() == Object::Kind::HASHTABLE);
  Value value = rhs_->Eval();
  tbl->Set(idx, value);

  // Dont return the value, just to quiet the output from repl
  // return value;
  return nullptr;
}

static Value AssignIndex(IndexExpr *indx, Value value) { return 1; }

Value IndexExpr::Eval() {
  Value object = variable_->Eval();

  if (!object.IsObject()) {
    throw EvalError("Attempt to index a non-object");
  }
  if (object.IsNill() || object.AsObject() == nullptr) {
    throw EvalError("Attempt to index null");
  }
  if (object.AsObject()->kind() != Object::Kind::HASHTABLE) {
    throw EvalError("Can only index on hashtables");
  }
  // TODO need to create ref to object
  Value index = index_->Eval();
  if (index.IsNill()) {
    throw EvalError("Attempting to use nil as an index");
  }

  HashTable *hash = static_cast<HashTable *>(object.AsObject());
  Value *result = hash->Get(index);
  if (result == nullptr) return nullptr;
  return *result;
}

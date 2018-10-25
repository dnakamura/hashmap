#pragma once
#include <cassert>
#include <cassert>
#include <cstddef>
#include <iostream>
#include "Hash.hpp"

#include "Object.hpp"

class Value {
 public:
  enum class Kind {
    NILL = 0,
    OBJECT,
    INTEGER,
    VALUE_REF,  // TODO This is a super hack
  };

  constexpr Value() : kind_(Kind::NILL), i_(0) {}
  constexpr Value(Object* obj) : kind_(Kind::OBJECT), object_(obj) {}
  constexpr Value(std::int64_t i) : kind_(Kind::INTEGER), i_(i) {}
  constexpr Value(std::nullptr_t) : kind_(Kind::NILL), object_(nullptr) {}

  constexpr Kind kind() const { return kind_; }
  constexpr bool IsInt() const { return kind_ == Kind::INTEGER; }
  constexpr bool IsNill() const {
    return kind_ == Kind::NILL; /*TODO also maybe check if we are an object
                                   which is nill*/
  }
  constexpr bool IsObject() const { return kind_ == Kind::OBJECT; }

  Object* AsObject() const {
    assert(kind_ == Kind::OBJECT);
    return object_;
  }

  std::int64_t AsInteger() const {
    assert(kind_ == Kind::INTEGER);
    return i_;
  }

  constexpr HashValue Hash() const {
    switch (kind_) {
      case Kind::INTEGER:
        return i_;
      case Kind::OBJECT:
        return object_->Hash();
      case Kind::NILL:
        return 0;
    }
    assert(false);
    return 0;
  }

  constexpr bool operator==(const Value& other) const;

 private:
  union {
    Object* object_;
    std::int64_t i_;
  };

  Kind kind_;

  friend std::ostream& operator<<(std::ostream& os, const Value& v);
};

// TODO this is a real hackjob
// object comparison code should be refactored into object
constexpr bool Value::operator==(const Value& other) const {
  if (other.kind_ != kind_) return false;
  switch (kind_) {
    case Kind::NILL:
      return true;
    case Kind::INTEGER:
      return i_ == other.i_;
    case Kind::OBJECT: {
      if (object_ == other.object_) return true;
      if (object_ != nullptr && other.object_ != nullptr) {
        if ((object_->kind() == Object::Kind::STRING) &&
            (other.object_->kind() == Object::Kind::STRING)) {
          return object_->Hash() == other.object_->Hash();
        } else {
          return false;
        }
      } else {
        return false;
      }
    }
    default:
      assert(false);
  }
  return false;  // should be unreachable
}

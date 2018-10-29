#pragma once

#include <cstring>
#include <functional>
#include <string>
#include <string_view>
#include "Hash.hpp"

//#include "Environ.hpp"

constexpr size_t align2(size_t size, size_t alignment) {
  return (size + alignment - 1) & ~(alignment - 1);
}

class Object {
  static HashValue objectId;

 public:
  enum class Kind : std::uint8_t { HASHTABLE, REF_ARRAY, STRING };

  constexpr HashValue Hash() const { return hash_; }

  constexpr Kind kind() const {
    return static_cast<Kind>((header_ >> 24) & 0xFF);
  }

 protected:
  Object(Kind kind)
      : header_(static_cast<std::uint64_t>(kind) << 24), hash_(objectId++) {}
  Object(Kind kind, std::uint64_t hash)
      : header_(static_cast<std::uint64_t>(kind) << 24), hash_(hash) {}

 private:
  std::uint64_t header_;
  std::uint64_t hash_;
};

class StringObject;
StringObject* AllocateString(std::string_view str);
class StringObject : public Object {
  StringObject(std::size_t size, std::size_t hash)
      : Object(Kind::STRING, hash), size_(size) {}

 public:
  static inline StringObject* Allocate(std::string_view str);
  std::string_view ToStringView() {
    const char* strData = reinterpret_cast<char*>(
        reinterpret_cast<uint8_t*>(this) + sizeof(StringObject));
    return {strData, size_};
  }
  constexpr size_t size() const {
    return align2(sizeof(StringObject) + size_, 16);
  }

 private:
  const std::size_t size_;
};

std::size_t Size(const Object* obj);

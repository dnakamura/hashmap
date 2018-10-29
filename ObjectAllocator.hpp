#pragma once
#include <OMR/GC/Allocator.hpp>
#include "Object.hpp"

extern OMR::GC::Context* ctx;

static inline size_t align(size_t size, size_t alignment) {
  return (size + alignment - 1) & ~(alignment - 1);
}

StringObject* StringObject::Allocate(std::string_view str) {
  std::size_t size = align(sizeof(StringObject) + str.size(), 16);

  std::size_t hash = std::hash<std::string_view>{}(str);
  const std::size_t strSize = str.size();
  StringObject* ptr = static_cast<StringObject*>(
      OMR::GC::allocateNonZero<StringObject>(*ctx, size, [=](StringObject* p) {
        new (p) StringObject(strSize, hash);
      }));

  std::uint8_t* data =
      reinterpret_cast<std::uint8_t*>(ptr) + sizeof(StringObject);

  std::memcpy(data, &*str.cbegin(), str.size());
  return ptr;
}

RefArray* RefArray::Allocate(std::size_t sz) {
  const size_t allocSize =
      align(sizeof(RefArray) + sizeof(HashTable::Entry) * sz, 16);
  return OMR::GC::allocate<RefArray>(
      *ctx, allocSize, [=](RefArray* p) { new (p) RefArray(sz); });
}
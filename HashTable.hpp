#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP
#include <cassert>
#include <cstdint>
#include <iostream>
#include "Hash.hpp"
#include "Object.hpp"
#include "Value.hpp"



int main();
class RefArray;
class HashTable : public Object {
 public:
  static constexpr std::uint64_t DEFAULT_SIZE = 16;
  inline HashTable();

  // TODO should maybe be protected/private?
  void Initialize(std::size_t sz);
  struct Entry;
  // struct Key;

  Value* Get(Value key);
  void Set(Value key, Value value);

 private:
  void Grow(std::size_t newSize);
  bool FindSlot(const Value& key, std::size_t* idx) const;

  /* Data memebers */
  // TODO should we maybe enforce a power of 2 sizing?
  std::uint64_t size_ = 0;
  std::uint64_t remainingCapacity_ = 0;
  std::size_t nextFreeSlot_;
  RefArray* backingArray_ = nullptr;
  friend std::ostream& operator<<(std::ostream& os, const Value& v);
  friend class OMRClient::GC::ObjectScanner;
  friend int main();
};

struct HashTable::Entry {
  Value key;
  std::intptr_t next;
  Value value;
};

/* TODO this may be bogus when we start actually gc allocating*/
HashTable::HashTable()
    : Object(Kind::HASHTABLE),
      size_(0),
      // entries_(nullptr)
      backingArray_(nullptr) {}

// TODO this isnt really a ref array, its more of a hash table backing array
class RefArray : public Object {
  RefArray(size_t sz) : Object(Kind::REF_ARRAY), size_(sz) {}
  RefArray(const RefArray&) = delete;
  RefArray& operator=(const RefArray&) = delete;

 public:
  inline static RefArray* Allocate(std::size_t sz);

  size_t FindEmptyCell(size_t start);
  void Relocate(size_t from, size_t to);

  constexpr size_t size() const {
    return sizeof(RefArray) + sizeof(HashTable::Entry) * size_;
  }
  constexpr size_t count() const { return size_;}
  // private:
  std::size_t size_;
  HashTable::Entry entries_[];
  friend class OMRClient::GC::ObjectScanner;
  
};

void DumpHashTable(std::ostream& os, RefArray& refarr);



#endif

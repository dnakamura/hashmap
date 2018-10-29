// Hashtable.cpp : This file contains the 'main' function. Program execution
// begins and ends there.
//

#include "HashTable.hpp"
#include "ObjectAllocator.hpp"
#include <iomanip>
#include <iostream>
#include <OMR/GC/StackRoot.hpp>
HashValue Object::objectId = 0;

std::size_t Size(const Object* obj) {
  switch (obj->kind()) {
    case Object::Kind::HASHTABLE:
      return align(sizeof(HashTable),16);
    case Object::Kind::REF_ARRAY:
      return align(static_cast<const RefArray*>(obj)->size(),16);
    case Object::Kind::STRING:
      return align(static_cast<const StringObject*>(obj)->size(),16);
  }
  assert(false);

  return 16;
}
void HashTable::Initialize(std::size_t sz) {
  // Note: we throw away the old backing array so this is wildly unsafe'=
  OMR::GC::StackRoot<HashTable> self(*ctx, this);
  RefArray * temp = RefArray::Allocate(sz);
  self->backingArray_ = temp;
  self->size_ = sz;
  self->remainingCapacity_ = sz;  // TODO: maybe sz * 0.75?
}


void HashTable::Grow(std::size_t newSize) {
  assert(newSize > size_);
  const std::size_t oldSize = size_;
  OMR::GC::StackRoot<RefArray> old(*ctx);
  old = backingArray_;
  
  OMR::GC::StackRoot<HashTable> self(*ctx);
  self = this;
  Initialize(newSize);

  for (std::size_t i = 0; i < oldSize; ++i) {
    if (!old->entries_[i].key.IsNill()) {
      self->Set(old->entries_[i].key, old->entries_[i].value);
    }
  }
}

Value* HashTable::Get(Value key) {
  std::size_t idx = size_;

  if (!FindSlot(key, &idx)) {
    return nullptr;
  }
  assert(idx < size_);
  return &backingArray_->entries_[idx].value;
}

void HashTable::Set(Value key, Value value) {
  assert(size_ > 0);
  std::size_t idx = 0;
  if (FindSlot(key, &idx)) {
    // Key already exists in the table
    backingArray_->entries_[idx].value = value;
    return;
  }
  // key not in the table
  if (remainingCapacity_ == 0) {
    // grow the table
    Grow(size_ * 2);
    assert(false);
  }

  --remainingCapacity_;
  // Is the slot we want free
  if (backingArray_->entries_[idx].key.IsNill()) {
    backingArray_->entries_[idx] = {key, 0, value};
    return;
  }

  // otherwise get a new free slot
  size_t newIdx = backingArray_->FindEmptyCell(idx);
  backingArray_->entries_[newIdx] = {key, 0, value};
  backingArray_->entries_[idx].next = newIdx - idx;
}

void DumpHashTable(std::ostream& os, RefArray& refarr) {
  for (std::size_t i = 0; i < refarr.size_; ++i) {
    if (refarr.entries_[i].key.IsNill()) {
      continue;
    }
    os << "\t[" << refarr.entries_[i].key << "] = " << refarr.entries_[i].value
       << "\n";
  }
}

std::ostream& operator<<(std::ostream& os, const Value& v) {
  switch (v.kind_) {
    case Value::Kind::NILL:
      os << "(NILL)";
      break;
    case Value::Kind::INTEGER:
      os << "INT<" << v.AsInteger() << ">";
      break;
    case Value::Kind::OBJECT:
      switch (v.AsObject()->kind()) {
        case Object::Kind::HASHTABLE: {
          os << "HASHTABLE<" << v.AsObject()
             << ">\n";  // TODO needs to be better
          RefArray* backingArray =
              static_cast<HashTable*>(v.AsObject())->backingArray_;
          assert(backingArray != nullptr);
          DumpHashTable(os, *backingArray);
          break;
        }
        case Object::Kind::REF_ARRAY:
          os << "<REF_ARRAY> ";  // probably shouldnt be happening
          break;
        case Object::Kind::STRING:
          os << std::quoted(
              static_cast<StringObject*>(v.AsObject())->ToStringView());
          break;
        default:
          assert(false);
      }
      break;
    default:
      os << "INVALID!";
  }
  return os;
}

// bool FindSlot(const Value& key, std::size_t* idx) const;
bool HashTable::FindSlot(const Value& key, std::size_t* index) const {
  std::size_t idx = key.Hash() % size_;
  Entry* entry = &backingArray_->entries_[idx];  // todo read barrier

  assert(!key.IsNill());

  // todo read barrier?
  if (entry->key.IsNill()) {
    *index = idx;
    return false;
  }

  // something is in the slot. Is it supposed to be here, or an overflow value
  size_t otherHash = entry->key.Hash() % size_;
  if (otherHash != idx) {
    // THis is an overflow value, we need to relocate
    // TODO: need to refactor. No sense in relocating if  we are only doing a
    // lookup
    size_t new_index = backingArray_->FindEmptyCell(idx);
    backingArray_->Relocate(idx, new_index);
    *index = idx;
    return false;
  }

  // Something is in the slot, with the correct hash value
  // Walk the chain until we find our key
  // TODO this shitty loop unrolling is dumb.
  // there must be a better way to refactor this
  if (entry->key == key) {
    *index = idx;
    return true;
  }
  while (backingArray_->entries_[idx].next != 0) {
    idx += backingArray_->entries_[idx].next;
    assert(idx < size_);

    if (backingArray_->entries_[idx].key == key) {
      *index = idx;
      return true;
    }
  }
  //*index = backingArray_->FindEmptyCell(idx);
  *index = idx;
  return false;
}

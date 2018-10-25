#include "HashTable.hpp"

size_t RefArray::FindEmptyCell(size_t start) {
  const size_t sz = size_;
  assert(start < sz);
  // Look aboce
  for (size_t i = start + 1; i < sz; ++i) {
    if (entries_[i].key.IsNill()) {
      return i;
    }
  }

  assert(start > 1);
  for (size_t i = start; i > 0; --i) {
    if (entries_[i - 1].key.IsNill()) {
      return i;
    }
  }
  assert(false);
  return size();
}

void RefArray::Relocate(size_t from, size_t to) {
  size_t idx = entries_[from].key.Hash() % size_;
  assert(entries_[idx].next != 0);
  while (entries_[idx].next + idx != from) {
    assert(entries_[idx].next != 0);
    idx += entries_[idx].next;
    assert(idx < size_);
    assert(!entries_[idx].key.IsNill());
  }
  assert(entries_[to].key.IsNill());
  entries_[to] = entries_[from];
  entries_[idx].next = to - idx;

  // zero out the old entry. probably not technically required
  // since we will be overwriting immidiately anyway
  entries_[from].key = nullptr;
  entries_[from].value = nullptr;
}

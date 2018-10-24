#pragma once

#include "Hash.hpp"
#include <cstdint>
#include <cassert>
#include <iostream>
#include "Object.hpp"
#include "Value.hpp"







class RefArray;
class HashTable : public Object {
public:
	static constexpr std::uint64_t DEFAULT_SIZE = 16;
	inline HashTable();

	//TODO should maybe be protected/private?
	inline void Initialize(std::size_t sz);
	struct Entry;
	//struct Key;

	Value* Get(Value key);
	void Set(Value key, Value value);
	
private:
	void Grow(std::size_t newSize);
	bool FindSlot(const Value& key, std::size_t* idx) const;
	

	/* Data memebers */
	//TODO should we maybe enforce a power of 2 sizing?
	std::uint64_t size_ = 0;
	std::uint64_t remainingCapacity_ = 0;
	std::size_t nextFreeSlot_;
	RefArray* backingArray_ = nullptr;
	friend std::ostream& operator<<(std::ostream& os, const Value& v);
};



struct HashTable::Entry {
	Value key;
	std::intptr_t next;
	Value value;
		
};

/* TODO this may be bogus when we start actually gc allocating*/
HashTable::HashTable() :
	Object(Kind::HASHTABLE),
	size_(0),
	//entries_(nullptr)
	backingArray_(nullptr)
{

}


//TODO this isnt really a ref array, its more of a hash table backing array
class RefArray : public Object {
	RefArray(size_t sz) : Object(Kind::REF_ARRAY), size_(sz) {}
	RefArray(const RefArray &) = delete;
	RefArray& operator=(const RefArray&) = delete;

public:
	static RefArray* Allocate(std::size_t sz) {
		RefArray* arr = static_cast<RefArray*>(calloc(1,sizeof(RefArray) + sizeof(HashTable::Entry)*sz));
		new (arr) RefArray(sz);
		return arr;
	}

	size_t FindEmptyCell(size_t start);
	void Relocate(size_t from, size_t to);

	constexpr size_t size() const { return sizeof(RefArray) + sizeof(HashTable::Entry) * size_; }
//private:
	std::size_t size_;
	HashTable::Entry entries_[];
};



void DumpHashTable(std::ostream& os, RefArray &refarr);

void HashTable::Initialize(std::size_t sz) {
	//Note: we throw away the old backing array so this is wildly unsafe
	backingArray_ = RefArray::Allocate(sz);
	size_ = sz;
	remainingCapacity_ = sz;//TODO: maybe sz * 0.75?
}

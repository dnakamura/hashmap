#pragma once

#include "Hash.hpp"
#include <string>
#include <string_view>
#include <functional>
#include <cstring>

class Object {
	static HashValue objectId;
public:
	enum class Kind : std::uint8_t {
		HASHTABLE,
		REF_ARRAY,
		STRING
	};

	 constexpr HashValue Hash() const {
		 return hash_;
	}

	constexpr Kind kind() const { return static_cast<Kind>(header_ & 0xFF); }
protected:
	Object(Kind kind)
		: header_(static_cast<std::uint64_t>(kind)),
		hash_(objectId++)
	{}
	Object(Kind kind, std::uint64_t hash)
		: header_(static_cast<std::uint64_t>(kind)),
		hash_(hash) {}

#if 0
	// note this is a hack for lazy-hashing on strings
	void setHash(std::uint64_t hash) {
		hash_ = hash;
	}
#endif

private:
	std::uint64_t header_;
	std::uint64_t hash_;
};


class StringObject : public Object {
	StringObject(std::size_t size, std::size_t hash): Object(Kind::STRING, hash), size_(size) {}
public:
	static StringObject* Allocate(std::string_view str) {
		std::size_t size = sizeof(StringObject) + str.size();
		//TODO this all needs to change when dealing with a gc
		StringObject *ptr = static_cast<StringObject*>(std::malloc(size));
		std::size_t hash = std::hash<std::string_view>{}(str);

		new (ptr) StringObject(str.size(), hash);
		std::uint8_t *data = reinterpret_cast<std::uint8_t*>(ptr) + sizeof(StringObject);

		std::memcpy(data, &*str.cbegin(), str.size());
		return ptr;

	}

	std::string ToString() {
		using namespace std;
		return ""s;
	}
	std::string_view ToStringView() {
		const char*  strData = reinterpret_cast<char*>(reinterpret_cast<uint8_t*>(this) + sizeof(StringObject));
		return { strData, size_ };
	}
private:
	const std::size_t size_;
};


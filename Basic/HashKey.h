#ifndef _HASHKEY_H_
#define _HASHKEY_H_
#include <string>
#include <vector>

namespace HashKey
{
	class EitherKey {
		friend class EitherKeyHash;
	public:
		enum class KeyType
		{
			None = 0,
			Integer,
			String
		};
		EitherKey();
		EitherKey(const EitherKey& other);
		EitherKey(int64_t key);
		EitherKey(const std::string& key);
		EitherKey(const char* key);
		~EitherKey();

		bool operator==(const EitherKey& theOther) const;
		EitherKey& operator=(const EitherKey& theOther);

		std::string uni_string() const;
		std::string to_string() const;
		void from_formatted_string(const std::string& formattedString);
		bool is_integer() const;
		int64_t get_integer() const;
		bool is_string() const;
		const std::string& get_string() const;
		bool is_empty() const;

	private:
		int64_t intValue;
		std::string stringValue;
		KeyType keyType;
	};

	class EitherKeyHash {
	public:
		size_t operator()(const EitherKey& me) const;
	};

	class DualKey {
		friend class DualKeyHash;
	public:
		DualKey(const EitherKey& _first, const EitherKey& _second);
		DualKey(const DualKey& other);
		~DualKey();
		
		bool operator==(const DualKey& theOther) const;
		DualKey& operator=(const DualKey& theOther);
		
		const EitherKey& get_first() const;
		const EitherKey& get_second() const;
	
	private:
		EitherKey first;
		EitherKey second;
	};

	class DualKeyHash {
	public:
		size_t operator()(const DualKey& theOther) const;
	};
}

inline HashKey::EitherKey::EitherKey()
{
	keyType = KeyType::None;
}

inline HashKey::EitherKey::EitherKey(const EitherKey& other)
{
	keyType = other.keyType;
	if (keyType == KeyType::Integer)
	{
		intValue = other.intValue;
	}
	else
	{
		stringValue = other.stringValue;
	}
}

inline HashKey::EitherKey::EitherKey(int64_t newValue)
{
	keyType = KeyType::Integer;
	intValue = newValue;
}

inline HashKey::EitherKey::EitherKey(const std::string& key)
{
	keyType = KeyType::String;
	stringValue = key;
}

inline HashKey::EitherKey::EitherKey(const char* key)
{
	keyType = KeyType::String;
	stringValue = key;
}

inline HashKey::EitherKey::~EitherKey()
{
}

inline bool HashKey::EitherKey::operator==(const EitherKey& theOther) const
{
	if (keyType != theOther.keyType)
	{
		return false;
	}
	switch (keyType)
	{
	case KeyType::Integer:
		return intValue == theOther.intValue;
	case KeyType::String:
		return !((stringValue).compare(theOther.stringValue));
	default:
		break;
	}
	return false;
}

inline size_t HashKey::EitherKeyHash::operator()(const EitherKey& me) const
{
	if (HashKey::EitherKey::KeyType::String == me.keyType)
	{
		std::hash<std::string> hasher;
		return hasher(me.stringValue);
	}
	else
	{
		std::hash<int64_t> hasher;
		return hasher(me.intValue);
	}
}

inline HashKey::EitherKey& HashKey::EitherKey::operator=(const EitherKey& theOther)
{
	keyType = theOther.keyType;
	if (keyType == KeyType::String)
	{
		stringValue = theOther.stringValue;
	}
	else
	{
		intValue = theOther.intValue;
	}
	return *this;
}

inline std::string HashKey::EitherKey::uni_string() const {
	switch (keyType)
	{
	case KeyType::Integer:
		return std::to_string(intValue);
	case KeyType::String:
		return stringValue;
	default:
		return "Unknown";
	}
}

inline std::string HashKey::EitherKey::to_string() const
{
	std::string retVal;
	switch (keyType)
	{
	case KeyType::Integer:
		retVal = "i:";
		retVal += std::to_string(intValue);
		break;
	case KeyType::String:
		retVal = "s:";
		retVal += stringValue;
		break;
	default:
		return "Unknown";
	}
	return retVal;
}

inline void HashKey::EitherKey::from_formatted_string(const std::string& formattedString) {
	if(formattedString.find("i:") == 0) {
		intValue = stol(formattedString.substr(2, std::string::npos));
		keyType = KeyType::Integer;
	}
	else if(formattedString.find("s:") == 0) {
		stringValue = formattedString.substr(2, std::string::npos);
		keyType = KeyType::String;
	}
	else{
		keyType = KeyType::None;
	}
}

inline bool HashKey::EitherKey::is_integer() const
{
	return keyType == KeyType::Integer;
}

inline int64_t HashKey::EitherKey::get_integer() const
{
	return intValue;
}

inline bool HashKey::EitherKey::is_string() const
{
	return keyType == KeyType::String;
}

inline const std::string& HashKey::EitherKey::get_string() const
{
	return stringValue;
}

inline bool HashKey::EitherKey::is_empty() const {
	return keyType == KeyType::None;
}

inline HashKey::DualKey::DualKey(const EitherKey& _first, const EitherKey& _second) : first(_first), second(_second) {

}

inline HashKey::DualKey::DualKey(const DualKey& other) : first(other.first), second(other.second) {

}

inline HashKey::DualKey::~DualKey() {

}
		
inline bool HashKey::DualKey::operator==(const DualKey& theOther) const {
	if(first == theOther.first && second == theOther.second) {
		return true;
	}
	return false;
}

inline size_t HashKey::DualKeyHash::operator()(const DualKey& theOther) const {
	return EitherKeyHash()(theOther.first) + EitherKeyHash()(theOther.second);
}

inline HashKey::DualKey& HashKey::DualKey::operator=(const DualKey& theOther) {
	first = theOther.first;
	second = theOther.second;
	return *this;
}
		
inline const HashKey::EitherKey& HashKey::DualKey::get_first() const {
	return first;
}

inline const HashKey::EitherKey& HashKey::DualKey::get_second() const {
	return second;
}

#endif

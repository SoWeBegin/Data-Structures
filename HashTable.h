#ifndef HASHTABLE_IMPLEMENTATION
#define HASHTABLE_IMPLEMENTATION

#include <algorithm>
#include <cstddef>
#include <list>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <vector>

namespace container {
	template<typename Key, typename Type, typename Hash = std::hash<Key>>
	class HashTable {
	private:
		using hash_table = std::vector<std::list<std::pair<const Key, Type>>>;
		std::size_t m_size{};				  	    // Total elements inserted - not size of the vector. 
		Hash m_hash;					  	    // Keep track of the constructed hash through e.g the constructors, so we can use it for the hashing functions in the STL
							       	  	    // Grow factor must be defined before m_bucket_count, otherwise we can't initialize the latter properly (initialization happens from top-bottom)
		inline static const double grow_factor = 2.0;     	    // The size of the table has to be a bit bigger than the total elements, to avoid too many collisions
		inline static const double m_max_load_factor = 1.0; 	    // Whenever the load factor is > than 0.75 we'll need to rehash
		std::size_t m_bucket_count{};			 	    // size of vector (= total buckets)
		std::vector<std::list<std::pair<const Key, Type>>> m_table; // Actual hash table - each vector's element is composed off by a list - each list contains Key-Value pairs
		// static used since all classes will share the same value (which is const), but also to make sure we can use the implicitly-declared move constructor


	public:
		using key_type = Key;
		using mapped_type = Type;
		using value_type = std::pair<const Key, mapped_type>;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using hasher = Hash;
		using reference = value_type&;
		using const_reference = const reference;
		using pointer = value_type*;
		using const_pointer = const pointer;

		// Default constructor
		constexpr HashTable(const hasher& hash = Hash())
			: m_size{ 0 }
			, m_hash{ hash }
			, m_bucket_count{ 0 }

		{}

		// Init-list constructor.
		// Each item in the list is a key-value pair: thus we can directly use the hash functions through the Key, and use said key with the hash-function, which returns a value of type std::size_t. This is used
		// for fast-indexing on our vector (which itself contains a hash table)
		constexpr explicit HashTable(std::initializer_list<value_type> list, const hasher& hash = Hash())
			: m_size{ list.size() }
			, m_hash{ hash }
			, m_bucket_count{ static_cast<size_type>(list.size() * grow_factor) }
			, m_table(static_cast<size_type>(list.size()* grow_factor)) // Note: () needed here to disambiguate ({} would call vector's initializer_list constructor)
		{
			for (auto current : list) {
				insert(current);
			}
		}

		// By principle we don't need any copy/move constructors/assignment operators, since we own no resources.
		// But since the implicit move constructor/operator= does not reset our m_size variable, we'll define them nonetheless.
		// We could just use the implicit ones, too.
		constexpr HashTable(const HashTable& other)
			: m_bucket_count{ other.m_bucket_count }
			, m_hash{ other.m_hash }
			, m_size{ other.m_size }
			, m_table{ other.m_table }
		{}

		constexpr HashTable(HashTable&& other) noexcept
			: HashTable() {
			other.swap(*this);
		}

		constexpr HashTable& operator=(HashTable&& other) noexcept {
			HashTable temp;
			other.swap(*this);
			temp.swap(other);
			return *this;
		}

		constexpr HashTable& operator=(const HashTable& other) {
			HashTable temp(other);
			temp.swap(*this);
			return *this;
		}

		constexpr HashTable& operator=(std::initializer_list<value_type> list) {
			HashTable temp{ list };
			temp.swap(*this);
			return *this;
		}


		// Capacity related
		constexpr bool empty() const noexcept {
			return m_size == 0;
		}

		constexpr size_type size() const noexcept {
			return m_size;
		}

		// Modifiers
		constexpr void clear() noexcept {
			m_table.clear();
			m_size = 0;
			m_bucket_count = 0;
		}

	private:
		constexpr bool key_found(const std::list<std::pair<const Key, mapped_type>>& bucket, const value_type& value) {
			for (const auto& element : bucket) {
				if (element.first == value.first) {
					return true;
				}
			}
			return false;
		}

	public:
		constexpr bool insert(const value_type& value) {
			size_type index{ m_hash(value.first) % m_bucket_count };
			auto& bucket{ m_table.at(index) };
			if (key_found(bucket, value)) return false;
			bucket.push_front(value);
			++m_size;
			if (calculate_load_factor() > m_max_load_factor) {
				rehash();
			}
			return true;
		}

		constexpr bool insert(value_type&& value) {
			size_type index{ m_hash(value.first) % m_bucket_count };
			auto& bucket{ m_table.at(index) };
			if (key_found(bucket, value)) return false;
			bucket.push_front(std::move(value));
			++m_size;
			if (calculate_load_factor() > m_max_load_factor) {
				rehash();
			}
			return true;
		}

		constexpr void insert(std::initializer_list<value_type> list) {
			for (auto elem : list) {
				insert(elem);
			}
		}

		template<typename Val>
		constexpr bool insert_or_assign(const Key& key, Val&& value) {
			size_type index{ m_hash(key) % m_bucket_count };
			auto& bucket{ m_table.at(index) };
			// Search for the key. If it exists, assign value to that key. Otherwise, insert the new value (through std::pair<key, std::forward<Val>(value))
			for (auto& element : bucket) {
				if (element.first == key) {
					element.second = value;
					return false;
				}
			}
			return insert(std::pair<Key, mapped_type>(key, std::forward<Val>(value)));
		}

		template<typename...Args>
		constexpr bool emplace(Args&&...args) {
			size_type index{ m_hash(std::pair<Key,Type>(std::forward<Args>(args)...).first) % m_bucket_count };
			auto& bucket{ m_table.at(index) };
			if (key_found(bucket, std::pair<Key, Type>(std::forward<Args>(args)...))) return false;
			bucket.emplace_front(std::forward<Args>(args)...);
			++m_size;
			if (calculate_load_factor() > m_max_load_factor) {
				rehash();
			}
			return true;
		}

		constexpr bool remove_by_key(const Key& key) {
			for (size_type index{ 0 }; auto & current_list : m_table) {
				for (auto& current_pair : current_list) {
					if (current_pair.first == key) {
						m_table.at(index).remove(current_pair);
						--m_size;
						return true;
					}
				}
				++index;
			}
			return false;
		}

		constexpr bool remove_by_value(const Type& value) {
			for (size_type index{ 0 }; auto & current_list : m_table) {
				for (auto& current_pair : current_list) {
					if (current_pair.second == value) {
						m_table.at(index).remove(current_pair);
						--m_size;
						return true;
					}
				}
				++index;
			}
			return false;
		}

		// Lookup functions
		constexpr Type& at(const Key& key) {
			size_type index{ m_hash(key) % m_bucket_count };
			auto& current_list{ m_table.at(index) };
			for (auto& current : current_list) {
				if (current.first == key)
					return current.second;
			}
			throw std::out_of_range("Specified key is not associated with any element");
		}

		constexpr const Type& at(const Key& key) const {
			size_type index{ m_hash(key) % m_size };
			auto& current_list{ m_table.at(index) };
			for (const auto& current : current_list) {
				if (current.first == key)
					return current.second;
			}
			throw std::out_of_range("Specified key is not associated with any element");
		}


		constexpr Type& operator[](const Key& key) {
			size_type index{ m_hash(key) % m_bucket_count };
			auto& current_list{ m_table.at(index) };
			for (auto& current : current_list) {
				if (current.first == key)
					return current.second;
			}
			return current_list.front().second;
		}

		constexpr const Type& operator[](const Key& key) const {
			size_type index{ m_hash(key) % m_bucket_count };
			auto& current_list{ m_table.at(index) };
			for (const auto& current : current_list) {
				if (current.first == key)
					return current.second;
			}
			return current_list.front().second;
		}

		constexpr size_type count(const Key& key) const {
			for (const auto& list : m_table) {
				for (const auto& pair : list) {
					if (pair.first == key) return 1;
				}
			}
			return 0;
		}

		constexpr bool contains_key(const Key& key) const {
			return count(key) == 1 ? true : false;
		}

		constexpr bool contains_value(const Type& value) const {
			for (size_type index{ 0 }; auto & current_list : m_table) {
				for (auto& current_pair : current_list) {
					if (current_pair.second == value) {
						return true;
					}
				}
			}
			return false;
		}

		// Bucket interface
		constexpr size_type bucket_count() const noexcept {
			return m_bucket_count;
		}

		constexpr size_type max_bucket_count() const noexcept {
			return m_table.max_size();
		}

		constexpr size_type bucket_size(size_type index) const noexcept {
			return m_table.at(index).size();
		}

		constexpr std::ptrdiff_t bucket(const Key& key) const {
			for (size_type index{ 0 }; const auto & current_list : m_table) {
				for (auto& current_pair : current_list) {
					if (current_pair.first == key) {
						return index;
					}
				}
			}
			return -1;
		}

		// Hash related
		constexpr double load_factor() const noexcept {
			return calculate_load_factor();
		}

		constexpr double max_load_factor() const noexcept {
			return m_max_load_factor;
		}

		constexpr void max_load_factor(double new_factor) {
			m_max_load_factor = new_factor;
		}

		constexpr void reserve(size_type count) {
			m_bucket_count = count;
			rehash(count);
		}

	private:
		constexpr double calculate_load_factor() const noexcept {
			return static_cast<double>(m_size) / m_bucket_count;
		}

		constexpr void rehash(size_type n) {
			hash_table temp{ m_table };	
			m_table.clear();		
			m_size = 0;			
			m_bucket_count = n; 		
			m_table.resize(m_bucket_count);
			for (const auto& current_bucket : temp) {
				for (const auto& current_pair : current_bucket) {
					insert(current_pair);
				}
			}
		}

	public:
		constexpr void rehash() {
			hash_table temp{ m_table }; // Copy the contents of the current hash table
			m_table.clear(); // Remove all elements from our table
			m_size = 0;	 // Reset the size (total elements in the table). The insert function will increase it on each insertion.
			m_bucket_count = static_cast<size_type>(m_bucket_count * grow_factor); // Double the total number of buckets
			m_table.resize(m_bucket_count);
			for (const auto& current_bucket : temp) {
				for (const auto& current_pair : current_bucket) {
					insert(current_pair);
				}
			}
		}

	public:
		constexpr const hash_table get_table() const noexcept {
			return m_table;
		}

		constexpr void swap(HashTable& other) noexcept {
			std::swap(m_size, other.m_size);
			std::swap(m_bucket_count, other.m_bucket_count);
			std::swap(m_hash, other.m_hash);
			std::swap(m_table, other.m_table);
		}
	};
}


#endif

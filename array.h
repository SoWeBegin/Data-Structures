#ifndef ARRAY_CONTAINER
#define ARRAY_CONTAINER

#include <algorithm>
#include <concepts>
#include <exception>
#include <iterator>
#include <stdexcept>

namespace container {
	template<typename T, std::size_t arr_size>
	class array
	{
		// Aliases
		using iterator			 = T*;
		using const_iterator		 = const T*;
		using reverse_iterator		 = std::reverse_iterator<iterator>;
		using const_reverse_iterator 	 = std::reverse_iterator<const_iterator>;
		using reference			 = T&;
		using const_reference		 = const T&;
		using size_type			 = std::size_t;
		using value_type		 = T;
		using pointer			 = T*;
		using const_pointer		 = const T*;
		static_assert(arr_size != 0, "error: array size cannot be 0");

	public:
		T m_array[arr_size];

		// Access operators (No bound checking)
		constexpr reference operator[](size_type index)			 { return m_array[index]; }
		constexpr const_reference operator[](size_type index) const	 { return m_array[index]; }
		constexpr reference back() noexcept				 { return *(end() - 1); }
		constexpr const_reference back() const noexcept			 { return *(end() - 1); }
		constexpr pointer data() noexcept				 { return m_array; }
		constexpr const_pointer data() const noexcept			 { return m_array; }
		constexpr reference front() noexcept				 { return *(data()); }
		constexpr const_reference front() const noexcept		 { return *(data()); }

		// Access operators (With bound checking)	
		constexpr reference at(size_type index)
		{
			return (index < arr_size) ? m_array[index] : throw std::out_of_range("Error: Index out of range");
		}
		constexpr const_reference at(size_type index) const
		{
			return (index < arr_size) ? m_array[index] : throw std::out_of_range("Error: Index out of range");
		}

		// Size related
		constexpr size_type size() const noexcept	 { return arr_size; }
		constexpr size_type max_size() const noexcept	 { return arr_size; }

		// Miscellaneous
		void fill(const size_type& value) noexcept	 { for (auto& current : m_array) current = value; }
		void swap(array& other) noexcept		 { std::swap(m_array, other.m_array); }


		// Iterators
		constexpr iterator begin() noexcept			{ return m_array; }
		constexpr const_iterator begin() const noexcept		{ return m_array; }
		constexpr const_iterator cbegin() const noexcept        { return m_array; }
		constexpr iterator end()   noexcept		        { return m_array + arr_size; }
		constexpr const_iterator end() const noexcept	        { return m_array + arr_size; }
		constexpr const_iterator cend() const noexcept	        { return m_array + arr_size; }
		constexpr auto rbegin() noexcept		        { return reverse_iterator(end()); }
		constexpr const auto rbegin() const noexcept	        { return const_reverse_iterator(end()); }
		constexpr const auto crbegin() const noexcept	        { return const_reverse_iterator(end()); }
		constexpr auto rend() noexcept				{ return reverse_iterator(begin()); }
		constexpr const auto rend() const noexcept		{ return reverse_iterator(begin()); }
		constexpr const auto crend() const noexcept		{ return reverse_iterator(begin()); }
	};

	// Deduction guide. Checks if the Arguments are of the same type as T.
	template<typename T, std::same_as<T>...Args>
	array(T, Args...)->array<T, sizeof...(Args) + 1>;
}

#endif

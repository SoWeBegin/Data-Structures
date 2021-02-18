  
#ifndef ARRAY_2D
#define ARRAY_2D

#include <algorithm>
#include <cassert>
#include <exception>
#include <initializer_list>
#include <utility>
#include <stdexcept>

namespace container {
	template<typename T, std::size_t Rows, std::size_t Columns>

	class array_2d
	{
		static_assert(Rows != 0, "error: first array size cannot be 0");
		static_assert(Columns != 0, "error: second array size cannot be 0");

	private:
		T m_array2d[Rows][Columns]{};

	public:
		// Aliases
		using size_type = std::size_t;
		using nested_init_list_type = std::initializer_list<std::initializer_list<T>>;
		using pointer = T*;
		using const_pointer = const T*;
		using reference = T&;
		using const_reference = const T&;
		using reverse_iterator = std::reverse_iterator<pointer>;
		using const_reverse_iterator = std::reverse_iterator<const_pointer>;

		// Avoids having to type 3 braces, allows 2 braces only
		// Note: 2 braces are always needed even for one single element
		explicit array_2d(nested_init_list_type array2d_list) {
			assert(array2d_list.size() <= Rows && "Wrong number of rows [1st index] inserted");
			size_type row{ 0 };
			size_type column{ 0 };
			for (auto& currentBrace : array2d_list) {
				assert(currentBrace.size() <= Columns && "Wrong number of columns [2nd index] inserted");
				for (auto& currentValue : currentBrace) {
					m_array2d[row][column++] = std::move(currentValue);
				}
				++row;
				column = 0;
			}
		}

		// Accessors (No bound checking)
		constexpr pointer operator[](size_type index)		  { return m_array2d[index]; }
		constexpr const_pointer operator[](size_type index) const { return m_array2d[index]; }
		constexpr reference back() noexcept 			  { return m_array2d[Rows - 1][Columns - 1]; }
		constexpr const_reference back() const noexcept		  { return m_array2d[Rows - 1][Columns - 1]; }
		constexpr pointer data() noexcept 			  { return m_array2d[0]; }
		constexpr const_pointer data() const noexcept 		  { return m_array2d[0]; }
		constexpr reference front() noexcept 			  { return *(data()); }
		constexpr const_reference front() const noexcept 	  { return *(data()); }


		// Accessors (With bound checking)
		constexpr reference at(size_type rowIndex, size_type columnIndex)
		{ return (rowIndex < Rows && columnIndex < Columns) ? m_array2d[rowIndex][columnIndex] : throw std::out_of_range("Error: Index out of range"); }
		
		constexpr const_reference at(size_type rowIndex, size_type columnIndex) const
		{ return (rowIndex < Rows && columnIndex < Columns) ? m_array2d[rowIndex][columnIndex] : throw std::out_of_range("Error: Index out of range"); }

		// Size related
		constexpr size_type size() const noexcept 	 { return Rows * Columns; }
		constexpr size_type row_size() const noexcept    { return Rows; }
		constexpr size_type column_size() const noexcept { return Columns; }

		// Miscellaneous
		void fill(const size_type& value) noexcept 	 { std::fill(&m_array2d[0][0], &m_array2d[Rows-1][Columns], value); }
		void swap(array_2d& other) noexcept 		 { std::swap(m_array2d, other.m_array2d); }

		// Iterators - Missing reverse iterators
		constexpr auto begin() noexcept			 { return std::begin(m_array2d); }
		constexpr const auto begin() const noexcept 	 { return std::begin(m_array2d); }
		constexpr const auto cbegin() const noexcept 	 { return std::begin(m_array2d); }

		constexpr auto end() noexcept 		  	 { return std::end(m_array2d); }
		constexpr const auto end() const noexcept  	 { return std::end(m_array2d); }
		constexpr const auto cend() const noexcept 	 { return std::end(m_array2d); }
	};
}

#endif

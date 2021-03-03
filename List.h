#ifndef BIDIRECTIONAL_LIST
#define BIDIRECTIONAL_LIST

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <concepts>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>

namespace container {
	template<typename T>
	class List {
	private:
		constexpr void deallocate(List& other) noexcept {
			if (other.m_head) {
				Node* current_node = other.m_head;
				while (current_node != nullptr) {
					Node* next_node = current_node->next;
					delete current_node;
					current_node = next_node;
				}
			}
			other.m_head = nullptr;
			other.m_tail = nullptr;
			other.m_size = 0;
		}

	private:
		struct Node {
			T data;
			Node* next;
			Node* prev;
			Node() = default;
			template<typename...Args>
			constexpr explicit Node(Args&&... args)
				: data{ std::forward<Args>(args)... }
			{}
		};

		Node* m_head;
		Node* m_tail;
		std::size_t m_size;

		/* Bidirectional iterator */
		template<typename T>
		class BidirectionalIterator {
		private:
			Node* m_iterator;

		public:
			using value_type = T;
			using reference = T&;
			using pointer = value_type*;
			using iterator_category = std::bidirectional_iterator_tag;
			using difference_type = std::ptrdiff_t;

			constexpr BidirectionalIterator(Node* bidir_iter = nullptr) : m_iterator{ bidir_iter } {}

			constexpr Node* getNodeAddress() const noexcept { return m_iterator; }
			constexpr Node* getNodeNextAddress() const noexcept { return m_iterator->next; }
			constexpr Node* getNodePreviousAddress() const noexcept { return m_iterator->prev; }
			constexpr reference operator*() const noexcept { return m_iterator->data; }
			constexpr pointer operator->() const noexcept { return m_iterator; }
			constexpr BidirectionalIterator& operator++() noexcept {
				m_iterator = m_iterator->next;
				return *this;
			}
			constexpr BidirectionalIterator operator++(int) noexcept {
				BidirectionalIterator tmp(*this);
				m_iterator = m_iterator->next;
				return tmp;
			}
			constexpr BidirectionalIterator& operator--() noexcept {
				m_iterator = m_iterator->prev;
				return *this;
			}
			constexpr BidirectionalIterator operator--(int) noexcept {
				BidirectionalIterator tmp(*this);
				m_iterator = m_iterator->prev;
				return tmp;
			}
			constexpr friend bool operator== (const BidirectionalIterator& first, const BidirectionalIterator& second) noexcept {
				return (first.m_iterator == second.m_iterator);
			}
			constexpr friend bool operator!=(const BidirectionalIterator& first, const BidirectionalIterator& second)  noexcept {
				return !(first.m_iterator == second.m_iterator);
			}
		};

	public:
		// Aliases
		using value_type = T;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using const_reference = const value_type&;
		using reference = value_type&;
		using const_reference = const value_type&;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using iterator = BidirectionalIterator<value_type>;
		using const_iterator = BidirectionalIterator<const_reference>;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		constexpr List() noexcept
			: m_head{ nullptr }, m_tail{ nullptr }, m_size{ 0 }
		{}

		constexpr explicit List(size_type count, const_reference value)
			: m_size{ count } {
			assert(count != 0 && "count is 0");
			Node* current_node = new Node(value);
			m_head = current_node;
			m_head->prev = nullptr;
			Node* tmp = m_head;

			for (size_type index{ 0 }; index < count - 1; ++index) {
				current_node->next = new Node(value);
				current_node = current_node->next;
				current_node->prev = tmp;
				tmp = tmp->next;
			}
			m_tail = current_node;
			m_tail->next = nullptr;
		}

		constexpr explicit List(size_type count)
			: List(count, T())
		{}

		template<typename input_iter>
		constexpr List(input_iter first, input_iter last)
			: m_size{ static_cast<size_type>(std::distance(first, last)) } {
			assert(first != last && "First == last");
			Node* current_node = new Node(*first);
			m_head = current_node;
			m_head->prev = nullptr;
			Node* tmp = m_head;
			for (size_type index{ 0 }; index < m_size - 1; ++index) {
				current_node->next = new Node(*(++first));
				current_node = current_node->next;
				current_node->prev = tmp;
				tmp = tmp->next;
			}
			m_tail = current_node;
			m_tail->next = nullptr;
		}

		constexpr List(const List& other) {
			if (other.m_head) {
				m_size = other.m_size;
				Node* current_node = new Node(other.m_head->data);
				Node* current_other_node = other.m_head;
				m_head = current_node;
				m_head->prev = nullptr;
				Node* tmp = m_head;
				while (current_other_node->next != nullptr) {
					current_node->next = new Node(current_other_node->next->data);
					current_node = current_node->next;
					current_other_node = current_other_node->next;
					current_node->prev = tmp;
					tmp = tmp->next;
				}
				m_tail = current_node;
				m_tail->next = nullptr;
			}
			else { List(); }
		}

		constexpr List(List&& other) noexcept
			: List() {
			other.swap(*this);
		}

		constexpr List(std::initializer_list<T> list)
			: List(std::begin(list), std::end(list))
		{}

		~List() {
			Node* current_node = m_head;
			while (current_node != nullptr) {
				Node* next_node = current_node->next;
				delete current_node;
				current_node = next_node;
			}
			m_head = nullptr;
		}

		constexpr List& operator=(const List& other) {
			List temp(other);
			temp.swap(*this);
			return *this;
		}

		constexpr List& operator=(List&& other) {
			other.swap(*this);
			deallocate(other);
			return *this;
		}

		constexpr List& operator=(std::initializer_list<T> list) {
			List temp{ list };
			temp.swap(*this);
			return *this;
		}

		constexpr void assign(size_type new_size, const_reference value) {
			deallocate(*this);
			List temp_list(new_size, value);
			temp_list.swap(*this);
		}

		template<typename input_iter>
		constexpr void assign(input_iter first, input_iter last) {
			deallocate(*this);
			List temp_list(first, last);
			temp_list.swap(*this);
		}

		constexpr void assign(std::initializer_list<T> list) {
			assign(std::begin(list), std::end(list));
		}

		constexpr reference front() noexcept {
			return m_head->data;
		}

		constexpr const_reference front() const noexcept {
			return m_head->data;
		}

		constexpr reference back() noexcept {
			return m_tail->data;
		}

		constexpr const_reference back() const noexcept {
			return m_tail->data;
		}

		constexpr iterator begin() noexcept {
			return iterator(m_head);
		}

		constexpr const_iterator begin() const noexcept {
			return const_iterator(m_head);
		}

		constexpr const_iterator cbegin() const noexcept {
			return const_iterator(m_head);
		}

		constexpr iterator end() noexcept {
			return iterator(m_tail->next);
		}

		constexpr const_iterator end() const noexcept {
			return const_iterator(m_tail->next);
		}

		constexpr const_iterator cend() const noexcept {
			return const_iterator(m_tail->next);
		}

		constexpr reverse_iterator rbegin() noexcept {
			return reverse_iterator(end());
		}

		constexpr const_reverse_iterator rbegin() const noexcept {
			return const_reverse_iterator(end());
		}

		constexpr const_reverse_iterator crbegin() const noexcept {
			return const_reverse_iterator(end());
		}

		constexpr reverse_iterator rend() noexcept {
			return reverse_iterator(begin());
		}

		constexpr const_reverse_iterator rend() const noexcept {
			return const_reverse_iterator(begin());
		}

		constexpr const_reverse_iterator crend() const noexcept {
			return const_reverse_iterator(begin());
		}

		constexpr bool empty() const noexcept {
			return m_size == 0;
		}

		constexpr size_type size() const noexcept {
			return m_size;
		}

		constexpr size_type max_size() const noexcept {
			return std::numeric_limits<difference_type>::max();
		}

		constexpr void clear() noexcept {
			deallocate(*this);
		}

		template<typename...Args>
		constexpr iterator emplace(iterator pos, Args&&...args) {
			Node* temp = pos.getNodeAddress();
			Node* current_node = new Node(std::forward<Args>(args)...);
			if (temp == m_head) {
				m_head->prev = current_node;
				current_node->next = m_head;
				m_head = current_node;
				m_head->prev = nullptr;
			}
			else if (temp == end()) {
				m_tail->next = current_node;
				current_node->prev = m_tail;
				m_tail = current_node;
				m_tail->next = nullptr;
			}
			else {
				Node* prev_node = pos.getNodePreviousAddress();
				temp->prev = current_node;
				current_node->prev = prev_node;
				prev_node->next = current_node;
				current_node->next = temp;
			}

			m_size += 1;
			return iterator(current_node);
		}

		constexpr iterator insert(iterator pos, const_reference value) {
			return emplace(pos, value);
		}

		constexpr iterator insert(iterator pos, value_type&& value) {
			return emplace(pos, std::move(value));
		}

		constexpr iterator insert(iterator pos, size_type count, const_reference value) {
			iterator temp;
			for (size_type index{ 0 }; index < count; ++index) {
				temp = emplace(pos, value);
			}

			return (count == 0) ? pos : temp;
		}

		constexpr iterator insert(iterator pos, std::initializer_list<T> list) {
			iterator temp;
			for (auto current : list) {
				temp = emplace(pos, current);
			}
			return (list.size() == 0) ? pos : temp;
		}

		constexpr iterator erase(iterator pos) {
			Node* temp = pos.getNodeAddress();
			Node* prev_temp = pos.getNodePreviousAddress();
			Node* next_temp = temp->next;
			if (temp != nullptr) {
				if (temp == m_head) {
					m_head = next_temp;
					m_head->prev = nullptr;
				}
				else if (temp == m_tail) {
					m_tail = prev_temp;
					m_tail->next = nullptr;
				}
				else {
					prev_temp->next = next_temp;
					next_temp->prev = prev_temp;
				}
				--m_size;
				delete temp;
			}
			auto posit = pos;
			return (++posit != nullptr) ? posit : end();
		}

		constexpr iterator erase(iterator first, iterator last) {
			Node* firstNode_temp = first.getNodeAddress();
			Node* firstNode_next = first.getNodeNextAddress();
			Node* lastNode_temp = last.getNodeAddress();

			if (firstNode_temp == m_head && (lastNode_temp == m_tail || lastNode_temp == end())) {
				deallocate(*this);
			}
			else {
				while (firstNode_next != lastNode_temp) {
					Node* temp = firstNode_next->next;
					delete firstNode_next;
					firstNode_next = temp;
					--m_size;
				}
				firstNode_temp->next = lastNode_temp;
			}
			return last;
		}

		constexpr void push_back(const_reference value) {
			insert(end(), value);
		}

		constexpr void push_back(value_type&& value) {
			insert(end(), std::move(value));
		}

		template<typename... Args>
		constexpr reference emplace_back(Args&&...args) {
			auto ref = emplace(end(), std::forward<Args>(args)...);
			return ref.getNodeAddress()->data;
		}

		constexpr void pop_back() {
			Node* temp_tail = m_tail;
			m_tail = m_tail->prev;
			m_tail->next = nullptr;
			delete temp_tail;
			--m_size;
		}

		template<typename...Args>
		constexpr reference emplace_front(Args...args) {
			Node* head_temp = m_head;
			Node* current = new Node(std::forward<Args>(args)...);
			m_head = current;
			m_head->next = head_temp;
			m_head->prev = nullptr;
			++m_size;
			return m_head->data;
		}

		constexpr void push_front(const_reference value) {
			emplace_front(value);
		}

		constexpr void push_front(value_type&& value) {
			emplace_front(std::move(value));
		}

		constexpr void pop_front() {
			Node* head_temp = m_head;
			Node* next_temp = m_head->next;
			m_head = next_temp;
			delete head_temp;
			m_head->prev = nullptr;
			--m_size;
		}

		constexpr void resize(size_type count, const_reference value = T()) {
			if (count < size()) {
				if (count == 0) { deallocate(*this); }
				else {
					for (size_type i{ 0 }; i < count; ++i) {
						pop_back();
					}
				}
			}
			else {
				for (size_type i{ size() }; i < count; ++i) {
					emplace_back(value);
				}
			}
		}

		constexpr void splice(iterator position, List& other) {
			Node* current_pos = position.getNodeAddress();
			if (current_pos == m_tail) {
				m_tail->next = other.m_head;
				other.m_head->prev = m_tail;
				m_tail = other.m_tail;
			}

			else if (current_pos == begin()) {
				Node* temp_head = m_head;
				other.m_tail->next = temp_head;
				m_head = other.m_head;
			}
			else {
				Node* prev_node = position.getNodePreviousAddress();
				Node* temp_head = other.m_head;
				Node* temp_tail = other.m_tail;

				prev_node->next = other.m_head;
				other.m_head->prev = prev_node;
				temp_tail->next = current_pos;
				current_pos->prev = temp_head;
			}
			m_size += other.m_size;
			other.m_size = 0;
			other.m_head = nullptr;
			other.m_tail = nullptr;
		}


	private:
		constexpr Node* _Remove(Node* beforeNode) noexcept {
			if (beforeNode->next == m_tail) {
				Node* temp_tail = m_tail;
				m_tail = beforeNode;
				delete temp_tail;
				m_tail->next = nullptr;
				return nullptr;
			}
			else {
				const auto to_remove = beforeNode->next;
				const auto removed_next = to_remove->next;
				beforeNode->next = removed_next;
				delete to_remove;
				return removed_next;
			}
		}

	public:
		constexpr size_type remove(const_reference toRemove_value) {
			return remove_if([&toRemove_value](auto other) { return other == toRemove_value; });
		}

		template<typename Predicate>
		constexpr size_type remove_if(Predicate pred) {
			Node* before_begin = new Node();
			Node* tmp_before_begin = before_begin;
			before_begin->next = m_head;
			size_type tot_removed{ 0 };

			for (Node* first = m_head; first != nullptr;) {
				if (pred(first->data)) {
					if (first == m_head) {
						first = _Remove(before_begin);
						m_head = first;
					}
					else
						first = _Remove(before_begin);
					++tot_removed;
				}

				else {
					before_begin = first;
					first = first->next;
				}
			}
			m_size -= tot_removed;
			return tot_removed;
		}

		constexpr size_type unique() {
			size_type removed{ 0 };
			for (Node* first = m_head; first != nullptr;) {
				if (first == m_tail) break;
				if (first->data == first->next->data) {
					first = _Remove(first);
					++removed;
				}
				else
					first = first->next;
			}
			m_size -= removed;
			return removed;
		}

		constexpr void swap(List& other) noexcept {
			Node* temp_node = m_head;
			m_head = other.m_head;
			other.m_head = temp_node;
			temp_node = nullptr;

			Node* temp_tail = m_tail;
			m_tail = other.m_tail;
			other.m_tail = temp_tail;
			temp_tail = nullptr;

			std::swap(m_size, other.m_size);
		}

		// Overloaded comparision operators
		constexpr bool operator<=(const List& other) {
			return !(other < *this);
		}

		constexpr bool operator >=(const List& other) {
			return !(*this < other);
		}

		constexpr bool operator== (const List& other) {
			Node* temp = m_head;
			Node* other_temp = other.m_head;
			bool notEqual = false;
			if (m_size != other.size()) return false;
			else {
				while (temp != nullptr && other_temp != nullptr) {
					if (temp->data != other_temp->data) {
						notEqual = true;
						break;
					}
					temp = temp->next;
					other_temp = other_temp->next;
				}
			}
			return notEqual;
		}

		constexpr bool operator!= (const List& other) {
			return !(*this == other);
		}

		constexpr bool operator<(const List& other) {
			return (std::lexicographical_compare(begin(), end(), other.begin(), other.end()));
		}

		constexpr bool operator>(const List& other) {
			return !(std::lexicographical_compare(begin(), end(), other.begin(), other.end()));
		}
	};

#endif

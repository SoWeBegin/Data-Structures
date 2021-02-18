#ifndef FORWARD_LIST
#define FORWARD_LIST

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
	template<typename Type>
	class ForwardList {
	private:
		struct Node {
			Type data;
			Node* next;
			Node() = default;
			template<class... Args>
			constexpr explicit Node(Args&&... args) : data{ std::forward<Args>(args)... } {}
		};

		Node* m_head;
		Node* m_tail;
		std::size_t m_size{};

		/* Forward iterator */
		template<typename T>
		class forward_iterator {
		private:
			Node* m_iterator;

		public:
			using value_type = T;
			using reference = T&;
			using pointer = value_type*;
			using iterator_category = std::forward_iterator_tag;
			using difference_type = std::ptrdiff_t;

			constexpr forward_iterator(Node* forw_iter = nullptr) : m_iterator{ forw_iter } {}

			constexpr Node* getNodeAddress() const noexcept { return m_iterator; }
			constexpr Node* getNodeNextAddress() const noexcept { return m_iterator->next; }
			constexpr reference operator*() const noexcept { return m_iterator->data; }
			constexpr pointer operator->() const noexcept { return m_iterator; }
			constexpr forward_iterator& operator++() noexcept {
				m_iterator = m_iterator->next;
				return *this;
			}
			constexpr forward_iterator operator++(int) noexcept {
				forward_iterator tmp(*this);
				m_iterator = m_iterator->next;
				return tmp;
			}
			constexpr friend bool operator== (const forward_iterator& first, const forward_iterator& second) noexcept { return (first.m_iterator == second.m_iterator); }
			constexpr friend bool operator!=(const forward_iterator& first, const forward_iterator& second) noexcept { return !(first.m_iterator == second.m_iterator); }
		};

		/* Useful functions for internal purposes */
		constexpr void deallocate(ForwardList& other) noexcept {
			if (!other.m_head) { return; }
			Node* current_node = other.m_head;
			while (current_node != nullptr) {
				Node* next_node = current_node->next;
				delete current_node;
				current_node = next_node;
			}
			other.m_head = nullptr;
			other.m_tail = nullptr;
			other.m_size = 0;
		}

	public:
		using value_type = Type;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using reference = value_type&;
		using const_reference = const value_type&;
		using pointer = Type*;
		using const_pointer = const pointer;
		using iterator = forward_iterator<value_type>;
		using const_iterator = forward_iterator<const Type>;

		// Member functions
		constexpr ForwardList() noexcept
			: m_head{ nullptr }
			, m_tail{ nullptr }
			, m_size{ 0 }
		{}

		constexpr explicit ForwardList(size_type count, const_reference value)
			: m_size{ count } {
			assert(count != 0 && "count is 0");
			Node* current_node = new Node(value);
			m_head = current_node;
			for (size_type index{ 0 }; index < count - 1; ++index) {
				current_node->next = new Node(value);
				current_node = current_node->next;
			}
			m_tail = current_node;
			m_tail->next = nullptr;
		}

		// Type must be DefaultConstructible 
		constexpr explicit ForwardList(size_type count)
			: ForwardList(count, Type{})
		{}

		template<std::input_iterator input_iter>
		constexpr ForwardList(input_iter first, input_iter last)
			: m_size{ static_cast<size_type>(std::distance(first, last)) } {
			assert(first != last && "First == last");
			Node* current_node = new Node(*first);
			m_head = current_node;
			for (size_type index{ 0 }; index < m_size - 1; ++index) {
				current_node->next = new Node(*(++first));
				current_node = current_node->next;
			}
			m_tail = current_node;
			m_tail->next = nullptr;
		}

		constexpr ForwardList(const ForwardList& other) {
			if (other.m_head) {
				m_size = other.m_size;
				Node* current_node = new Node(other.m_head->data);
				Node* current_other_node = other.m_head;
				m_head = current_node;
				while (current_other_node->next != nullptr) {
					current_node->next = new Node(current_other_node->next->data);
					current_node = current_node->next;
					current_other_node = current_other_node->next;
				}
				m_tail = current_node;
				m_tail->next = nullptr;
			}
			else {
				ForwardList();
			}
		}

		constexpr ForwardList(ForwardList&& other) noexcept
			: ForwardList() {
			other.swap(*this);
		}

		constexpr ForwardList(std::initializer_list<Type> list) // Use the iterator constructor and std::begin/std::end
			: ForwardList(std::begin(list), std::end(list))
		{}

		~ForwardList() {
			Node* current_node = m_head;
			while (current_node != nullptr) {
				Node* next_node = current_node->next;
				delete current_node;
				current_node = next_node;
			}
			m_head = nullptr;
		}

		constexpr ForwardList& operator=(const ForwardList& other) {
			ForwardList temp_list(other);
			temp_list.swap(*this);
			return *this;
		}

		constexpr ForwardList& operator=(ForwardList&& other) noexcept {
			other.swap(*this);
			deallocate(other);
			return *this;
		}

		constexpr ForwardList& operator=(std::initializer_list<Type> list) {
			ForwardList temp_list{ list };
			temp_list.swap(*this);
			return *this;
		}

		constexpr void assign(size_type new_size, const_reference value) {
			deallocate(*this);
			ForwardList temp_list(new_size, value);
			temp_list.swap(*this);
		}

		constexpr void assign(std::initializer_list<Type> list) {
			deallocate(*this);
			ForwardList temp_list{ list };
			temp_list.swap(*this);
		}

		template<typename input_iter>
		constexpr void assign(input_iter first, input_iter last) {
			deallocate(*this);
			ForwardList temp_list(first, last);
			temp_list.swap(*this);
		}

		// Element access
		constexpr reference front() noexcept {
			return m_head->data;
		}

		constexpr const_reference front() const noexcept {
			return m_head->data;
		}

		// Iterators
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
			if (m_tail == nullptr) { return nullptr; }
			return iterator(m_tail->next);
		}

		constexpr const_iterator end() const noexcept {
			if (m_tail == nullptr) { return nullptr; }
			return const_iterator(m_tail->next);
		}

		constexpr const_iterator cend() const noexcept {
			if (m_tail == nullptr) { return nullptr; }
			return const_iterator(m_tail->next);
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

		//Modifiers
		constexpr void clear() noexcept {
			deallocate(*this);
		}

		template<typename...Args>
		constexpr iterator emplace_after(const iterator position, Args...args) { // Must be O(1)
			Node* temp = position.getNodeAddress();
			Node* current_node = new Node(std::forward<Args>(args)...);
			if (temp == m_tail) {
				m_tail->next = current_node;
				current_node->next = nullptr;
				m_tail = m_tail->next;
			}
			else {
				Node* next_temp = position.getNodeNextAddress();
				temp->next = current_node;
				current_node->next = next_temp;
			}

			m_size += 1;
			return iterator(current_node);
		}

		constexpr iterator insert_after(const iterator position, const_reference value) {
			return emplace_after(position, value);
		}

		constexpr iterator insert_after(const iterator position, Type&& value) {
			return emplace_after(position, std::move(value));
		}

		constexpr iterator insert_after(const iterator position, size_type count, const_reference value) {
			iterator temp;
			for (size_type i{ 0 }; i < count; ++i) {
				temp = emplace_after(position, value);
			}
			return (count == 0) ? position : temp;
		}

		constexpr iterator insert_after(const iterator position, std::initializer_list<Type> list) {
			iterator temp; // nullptr iter
			for (auto current : list) {
				temp = emplace_after(position, current);
			}
			return (list.size() == 0) ? position : temp;
		}

		constexpr iterator erase_after(const iterator position) {
			Node* temp = position.getNodeAddress();
			Node* next_temp = temp->next;
			if (temp != nullptr && next_temp->next == nullptr) {
				delete temp->next;
				temp->next = nullptr;
				m_size -= 1;
			}
			else if (temp != nullptr) {
				temp->next = next_temp->next;
				delete next_temp;
				m_size -= 1;
			}
			auto pos = position;
			return (++pos != nullptr) ? pos : end();
		}

		constexpr iterator erase_after(iterator first, iterator last) {
			Node* firstNode_temp = first.getNodeAddress();
			Node* firstNode_next = first.getNodeNextAddress();
			Node* lastNode_temp = last.getNodeAddress();

			while (firstNode_next != lastNode_temp) {
				Node* temp = firstNode_next->next;
				delete firstNode_next;
				firstNode_next = temp;
				--m_size;
			}
			firstNode_temp->next = lastNode_temp;

			return last;
		}

		constexpr void push_front(const_reference value) {
			emplace_front(value);
		}

		constexpr void push_front(Type&& value) {
			emplace_front(std::move(value));
		}

		template<typename...Args>
		constexpr reference emplace_front(Args...args) {
			Node* head_temp = m_head;
			Node* current = new Node(std::forward<Args>(args)...);
			m_head = current;
			m_head->next = head_temp;
			++m_size;
			return m_head->data;
		}

		constexpr void pop_front() {
			Node* head_temp = m_head;
			Node* next_temp = m_head->next;
			m_head = next_temp;
			delete head_temp;
			--m_size;
		}

		constexpr void resize(size_type count, const_reference value = Type()) {
			if (count < size()) {
				if (count == 0) { deallocate(); }
				else {
					Node* tmp = m_head;
					for (size_type index{ 0 }; index < count - 1; ++index) {
						tmp = tmp->next;
					}
					Node* other_tmp = tmp;
					tmp = tmp->next;
					while (tmp->next != m_tail) {
						Node* tmpp = tmp->next;
						delete tmp;
						tmp = tmpp;
					}
					delete m_tail;
					m_tail = other_tmp;
					m_tail->next = nullptr;
				}
			}

			else {
				Node* temp_tail = m_tail;
				for (size_type index{ 0 }; index < count; ++index) {
					temp_tail->next = new Node(value);
					temp_tail = temp_tail->next;
				}
				m_tail = temp_tail;
				m_tail->next = nullptr;
				temp_tail = nullptr;
			}
			m_size = count;
		}

		constexpr void swap(ForwardList& other) noexcept {
			Node* temp_node;
			temp_node = m_head;
			m_head = other.m_head;
			other.m_head = temp_node;
			temp_node = nullptr;

			Node* temp_tail;
			temp_tail = m_tail;
			m_tail = other.m_tail;
			other.m_tail = temp_tail;
			temp_tail = nullptr;

			std::swap(m_size, other.m_size);
		}

		constexpr void splice_after(const iterator position, ForwardList& other) {
			Node* current_pos = position.getNodeAddress();
			if (current_pos == m_tail) {
				m_tail->next = other.m_head;
				m_tail = other.m_tail;
			}
			else {
				Node* next_node = position.getNodeNextAddress();
				Node* next_next = next_node->next;
				Node* temp_head = other.m_head;
				Node* temp_tail = other.m_tail;

				current_pos->next = temp_head;
				temp_tail->next = next_next;
			}
			m_size += other.m_size;
			other.m_size = 0;
			other.m_head = nullptr;
			other.m_tail = nullptr;
		}

	private:
		constexpr Node* _Remove(Node* beforeNode) noexcept { // Bug->Last element not removed/crash
			const auto to_remove = beforeNode->next;
			const auto removed_next = to_remove->next;
			beforeNode->next = removed_next;

			delete to_remove;
			return removed_next;
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
				if (pred(first->data)) { // Lambda is true, remove the element - m_head might be changed
					if (first == m_head) {
						first = _Remove(before_begin);
						m_head = first;
					}
					else
						first = _Remove(before_begin);
					++tot_removed;
				}

				else { // m_head is not removed
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

		constexpr void reverse() noexcept {
			Node* temp_data = m_head;
			ForwardList temp = ForwardList();
			while (temp_data != nullptr) {
				temp.push_front(temp_data->data);
				temp_data = temp_data->next;
			}
			temp.swap(*this);
		}

	private:
		/*Example taken from geeksforgeeks*/
		constexpr void _Sort(Node* (&head_ref), Node* new_node) {
			Node* current;
			if (head_ref == nullptr || (head_ref)->data >= new_node->data) {
				new_node->next = head_ref;
				head_ref = new_node;
			}
			else {
				current = head_ref;
				while (current->next != nullptr && current->next->data < new_node->data) {
					current = current->next;
				}
				new_node->next = current->next;
				current->next = new_node;
			}
		}

	public:
		constexpr void sort() {
			Node* sorted = nullptr;
			Node* current = m_head;
			while (current != nullptr) {
				Node* next = current->next;
				_Sort(sorted, current);
				current = next;
			}
			m_head = sorted;
		}

		constexpr bool operator<=(const ForwardList<Type>& other) {
			return !(other < *this);
		}

		constexpr bool operator >=(const ForwardList<Type>& other) {
			return !(*this < other);
		}

		constexpr bool operator== (const ForwardList& other) {
			Node* temp = m_head;
			Node* other_temp = other.m_head;
			bool notEqual = false;
			if (m_size != other.size()) return false;
			else {
				while (temp != nullptr && other_temp!=nullptr) {
					if (temp->data != other_temp->data) {
						notEqual = true;
						break;
					}
					temp = temp->next;
					other_temp = other_temp->next;
				}
			}
			return (notEqual) ? false : true;
		}
		
		constexpr bool operator!= (const ForwardList& other) {
			return !(*this == other);
		}

		constexpr bool operator<(const ForwardList& other) {
			return (std::lexicographical_compare(begin(), end(), other.begin(), other.end()));
		}

		constexpr bool operator>(const ForwardList& other) {
			return !(std::lexicographical_compare(begin(), end(), other.begin(), other.end()));
		}
	};
}

#endif

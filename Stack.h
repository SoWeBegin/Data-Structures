#ifndef STACK_CONTAINER
#define STACK_CONTAINER

#include <algorithm>
#include <deque>
#include <utility>
#include <type_traits>
#include <memory>

namespace container {

	template<typename T, class Container = std::deque<T>>
	class Stack {
	private:
		Container container{};

	public:
		using value_type = typename Container::value_type;
		using reference = typename Container::reference;
		using const_reference = typename Container::const_reference;
		using size_type = typename Container::size_type;
		using container_type = typename Container;

		// Constructors
		constexpr Stack() = default;
		constexpr explicit Stack(const Container& cont) : container{ cont } {}
		constexpr explicit Stack(Container&& cont) noexcept : container{ std::move_if_noexcept(cont) } {}

		// Constructors defined only if Container is allocator-aware - SFINAE
		template<typename Allocator, std::enable_if_t<std::uses_allocator<Container, Allocator>::value, int> = 0>
		explicit Stack(const Allocator& alloc) : container{ alloc } {}

		template<typename Allocator, std::enable_if_t<std::uses_allocator<Container, Allocator>::value, int> = 0>
		explicit Stack(const Container& cont, const Allocator& alloc) : container{ cont, alloc } {}

		template<typename Allocator, std::enable_if_t<std::uses_allocator<Container, Allocator>::value, int> = 0>
		explicit Stack(Container&& cont, const Allocator& alloc) : container{ std::move_if_noexcept(cont), alloc } {}

		template<typename Allocator, std::enable_if_t<std::uses_allocator<Container, Allocator>::value, int> = 0>
		Stack(const Stack& other, const Allocator& alloc) : container{ other.container, alloc } {}

		template<typename Allocator, std::enable_if_t<std::uses_allocator<Container, Allocator>::value, int> = 0>
		Stack(Stack&& other, const Allocator& alloc) : container{ std::move_if_noexcept(other.container), alloc } {}


		constexpr reference top() noexcept {
			return container.back();
		}

		constexpr const_reference top() const noexcept {
			return container.back();
		}
		constexpr bool empty() const noexcept {
			return container.empty();
		}

		constexpr size_type size() const noexcept {
			return container.size();
		}

		constexpr void push(const T& value) {
			container.push_back(value);
		}

		constexpr void push(T&& value) {
			container.push_back(std::move(value));
		}

		constexpr void pop() noexcept {
			container.pop_back();
		}

		constexpr void swap(Stack& other) noexcept {
			std::swap(container, other.container);
		}

		template<typename...Args>
		constexpr decltype(auto) emplace(Args&&...args) {
			return container.emplace_back(std::forward<Args>(args)...);
		}
	};

	template<typename T, class Container>
	constexpr bool operator==(const Stack<T, Container>& lhs, const Stack<T, Container>& rhs) noexcept {
		return lhs.container == rhs.container;
	}

	template<typename T, class Container>
	constexpr bool operator!=(const Stack<T, Container>& lhs, const Stack<T, Container>& rhs) {
		return lhs.container != rhs.container;
	}

	template<typename T, class Container>
	constexpr bool operator< (const Stack<T, Container>& lhs, const Stack<T, Container>& rhs) {
		return lhs.container < rhs.container;
	}

	template<typename T, class Container>
	constexpr bool operator> (const Stack<T, Container>& lhs, const Stack<T, Container>& rhs) {
		return lhs.container > rhs.container;
	}

	template<typename T, class Container>
	constexpr bool operator<=(const Stack<T, Container>& lhs, const Stack<T, Container>& rhs) {
		return lhs.container <= rhs.container;
	}

	template<typename T, class Container>
	constexpr bool operator>=(const Stack<T, Container>& lhs, const Stack<T, Container>& rhs) {
		return lhs.container >= rhs.container;
	}
}

#endif

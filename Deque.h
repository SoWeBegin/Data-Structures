#ifndef QUEUE_CONTAINER
#define QUEUE_CONTAINER

#include <algorithm>
#include <deque>
#include <utility>
#include <type_traits>
#include <memory>

namespace container {

	template<typename T, class Container = std::deque<T>>
	class Queue {
	private:
		Container container{};

	public:
		using value_type = typename Container::value_type;
		using reference = typename Container::reference;
		using const_reference = typename Container::const_reference;
		using size_type = typename Container::size_type;
		using container_type = typename Container;

		// Constructors
		constexpr Queue() = default;
		constexpr explicit Queue(const Container& cont) : container{ cont } {}
		constexpr explicit Queue(Container&& cont) noexcept : container{ std::move_if_noexcept(cont) } {}

		// Constructors defined only if Container is allocator-aware - SFINAE
		template<typename Allocator, std::enable_if_t<std::uses_allocator<Container, Allocator>::value, int> = 0>
		explicit Queue(const Allocator& alloc) : container{ alloc } {}

		template<typename Allocator, std::enable_if_t<std::uses_allocator<Container, Allocator>::value, int> = 0>
		explicit Queue(const Container& cont, const Allocator& alloc) : container{ cont, alloc } {}

		template<typename Allocator, std::enable_if_t<std::uses_allocator<Container, Allocator>::value, int> = 0>
		explicit Queue(Container&& cont, const Allocator& alloc) : container{ std::move_if_noexcept(cont), alloc } {}

		template<typename Allocator, std::enable_if_t<std::uses_allocator<Container, Allocator>::value, int> = 0>
		Queue(const Queue& other, const Allocator& alloc) : container{ other.container, alloc } {}

		template<typename Allocator, std::enable_if_t<std::uses_allocator<Container, Allocator>::value, int> = 0>
		Queue(Queue&& other, const Allocator& alloc) : container{ std::move_if_noexcept(other.container), alloc } {}


		constexpr reference front() noexcept {
			return container.front();
		}

		constexpr const_reference front() const noexcept {
			return container.front();
		}

		constexpr reference back() noexcept {
			return container.back();
		}

		constexpr const_reference back() const noexcept {
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
			container.pop_front();
		}

		constexpr void swap(Queue& other) noexcept {
			std::swap(container, other.container);
		}

		template<typename...Args>
		constexpr decltype(auto) emplace(Args&&...args) {
			return container.emplace_back(std::forward<Args>(args)...);
		}
	};

	template<typename T, class Container>
	constexpr bool operator==(const Queue<T, Container>& lhs, const Queue<T, Container>& rhs) noexcept {
		return lhs.container == rhs.container;
	}

	template<typename T, class Container>
	constexpr bool operator!=(const Queue<T, Container>& lhs, const Queue<T, Container>& rhs) {
		return lhs.container != rhs.container;
	}

	template<typename T, class Container>
	constexpr bool operator< (const Queue<T, Container>& lhs, const Queue<T, Container>& rhs) {
		return lhs.container < rhs.container;
	}

	template<typename T, class Container>
	constexpr bool operator> (const Queue<T, Container>& lhs, const Queue<T, Container>& rhs) {
		return lhs.container > rhs.container;
	}

	template<typename T, class Container>
	constexpr bool operator<=(const Queue<T, Container>& lhs, const Queue<T, Container>& rhs) {
		return lhs.container <= rhs.container;
	}

	template<typename T, class Container>
	constexpr bool operator>=(const Queue<T, Container>& lhs, const Queue<T, Container>& rhs) {
		return lhs.container >= rhs.container;
	}
}

#endif

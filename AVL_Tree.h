#ifndef AVL_DATATREE_STRUCTURE
#define AVL_DATATREE_STRUCTURE

#include <algorithm>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <memory>
#include <utility>
// Note: #include <iostream> only needed for the traversal-print functions (in case you pass e.g std::cout as argument)

/* Basic implementation of a simple AVL Tree data structure. A lot of "Standard" functions and overloads are missing. */
/* Iterators are missing on purpose as well: this implementation is supposed to be read by beginners unlike the previous ones, and also, this is only meant for learning purposes. */


namespace container {
	template<typename Key>
	class AVL {
	private:
		struct Node {
			Node* left = nullptr;
			Node* right = nullptr;
			Node* parent;
			Key data;
			int balance_factor;
			template<typename...Args>
			constexpr Node(Args&&...args) : data{ std::forward<Args>(args)... } {}
		};

		Node* m_root;
		std::size_t m_size{};

	public:
		using key_type = Key;
		using value_type = Key;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using reference = Key&;
		using const_reference = const Key&;
		using pointer = Key*;
		using const_pointer = const pointer;

	private:
		constexpr int get_height(const Node* root) const {
			if (root == nullptr) return -1;
			/* Keep going deeper into the tree until either root->left or root->right is nullptr.
			Since both left and right substrees are checked, the deepest-node will be added 1 thanks to stack unwinding. */
			return std::max(get_height(root->left), get_height(root->right)) + 1;
		}

		constexpr void rotate_right(Node* (&node)) noexcept {
			Node* updated_root = node->left;
			Node* temp = updated_root->right;
			updated_root->right = node;
			node->left = temp;
			node = updated_root;
		}

		constexpr void rotate_left(Node* (&node)) noexcept {
			Node* updated_root = node->right;
			Node* temp = updated_root->left;
			updated_root->left = node;
			node->right = temp;
			node = updated_root;
		}

		constexpr void rebalance(Node*& current, const Key& data) {
			if (current->balance_factor > 1 && data < current->left->data) {
				rotate_right(current);
			}
			else if (current->balance_factor > 1 && data > current->left->data) {
				rotate_left(current->left);
				rotate_right(current);
			}
			else if (current->balance_factor < -1 && data > current->right->data) {
				rotate_left(current);
			}
			else if (current->balance_factor < -1 && data < current->right->data) {
				rotate_right(current->right);
				rotate_left(current);
			}
		}

		constexpr void insert(Node*& current, const Key& data) {
			if (current == nullptr) {
				current = new Node(data);
			}

			else if (data < current->data) {
				insert(current->left, data);
			}
			else if (data > current->data) {
				insert(current->right, data);
			}
			else return; // Disallow duplicate elements

			// Update the current's Node balance_factor. O(n) due to get_height()
			current->balance_factor = get_height(current->left) - get_height(current->right);
			rebalance(current, data);
		}

	public:
		// Traversal
		// First parameter: The stream, eg std::cout. Use: cout_pre_order(std::cout, classObject.getRoot())
		template<typename Stream>
		constexpr void cout_pre_order(Stream& stream, const Node* root) noexcept {
			if (root != nullptr) {
				// Pre-order execution: root->left->right. Since first case is root, we first need to cout the root's data; then recursively visit all left nodes, print them -> visit all right nodes, 
				// and print them.
				stream << root->data << ' ';
				cout_pre_order(root->left);
				cout_pre_order(root->right);
			}
		}
		
		template<typename Stream>
		constexpr void cout_post_order(Stream& stream, const Node* root) noexcept {
			if (root != nullptr) {
				// Post-order execution: Left->right->root. We first need to go int he deepest left-node; Once this is done recursively, we'll traverse to the deepest right-node.
				// Note that due to stack unwinding all the arguments, and function variables are still in memory. Due to stack unwinding the deepest left-node will be printed, then the deepest right-node,
				// until we hit the root.
				cout_post_order(m_root->left);
				cout_post_order(m_root->right);
				stream << root->data;
			}
		}

		template<typename Stream>
		constexpr void cout_in_order(Stream& stream, const Node* root) noexcept {
			if (root != nullptr) {
				// In-order execution: Left->root->right. Since the first case is the deepest left node, we first recursively traverse until we find it. When that is done, we print it;
				// Again, due to the stack remembering all local variables and arguments, we can then print the deepest-right node.
				cout_in_order(root->left);
				stream << root->data;
				cout_in_order(root->right);
			}
		}

		constexpr const Node* get_root() const noexcept {
			return m_root;
		}

	public:
		// Constructors, destructor, assignment/copy assignment
		constexpr AVL() noexcept = default;

		constexpr explicit AVL(std::initializer_list<Key> list)
			: m_size{ list.size() }
		{
			for (auto current : list) {
				insert(m_root, current);
			}
		}

		template<typename input_iter>
		constexpr AVL(input_iter first, input_iter last) {
			for (auto& first_elem = first; first_elem < last; ++first_elem) {
				insert(m_root, *first_elem);
				++m_size;
			}
		}

	private:
		constexpr void deep_copy(Node* other, Node*& current) {
			if (other == nullptr) { current = nullptr; }
			else {
				current = new Node();
				current->data = other->data;
				deep_copy(other->left, current->left);
				deep_copy(other->right, current->right);
			}
		}

	public:
		constexpr AVL(const AVL& other)
			: m_size{ other.m_size }
		{
			deep_copy(other.m_root, m_root);
		}

		constexpr AVL(AVL&& other) noexcept
			: AVL()
		{
			other.swap(*this);
		}

		constexpr void swap(AVL& other) noexcept {
			Node* temp_root = m_root;
			m_root = other.m_root;
			other.m_root = temp_root;
			temp_root = nullptr;
			std::swap(m_size, other.m_size);
		}

	private:
		constexpr void destroy(Node* current) noexcept {
			if (current)
			{
				destroy(current->left);
				destroy(current->right);
				delete current;
				current = nullptr;
			}
		}

	public:
		~AVL() noexcept {
			destroy(m_root);
		}

		constexpr AVL operator=(const AVL& other) {
			AVL temp(other);
			temp.swap(*this);
			return *this;
		}

		constexpr AVL& operator=(AVL&& other) {
			other.swap(*this);
			destroy(other.m_root);
			other.m_root = nullptr;
			return *this;
		}

		constexpr AVL& operator=(std::initializer_list<Key> list) {
			AVL temp{ list };
			temp.swap(*this);
			return *this;
		}

		// Other functions similar to the standard std::set
		constexpr void empty() const noexcept {
			return m_size == 0;
		}

		constexpr size_type size() const noexcept {
			return m_size;
		}

		constexpr void clear() noexcept {
			destroy(m_root);
			m_root = nullptr;
			m_size = 0;
		}

		// Just to have a STL-like insert function. This just calls the previously-implemented insert function. 
		constexpr void insert(const Key& value) {
			insert(m_root, value);
			++m_size;
		}

		template<typename input_iterator>
		constexpr void insert(input_iterator first, input_iterator last) {
			for (auto& first_elem = first; first_elem < last; ++first_elem) {
				insert(m_root, *first_elem);
				++m_size;
			}
		}

		constexpr void insert(std::initializer_list<Key> list) {
			for (auto current : list) {
				insert(m_root, current);
				++m_size;
			}
		}

	private:
		// Example adapted from GeeksforGeeks
		constexpr Node* get_min(Node* current) const {
			Node* curr = current;
			while (curr->left != nullptr) {
				curr = curr->left;
			}

			return curr;
		}

		// Removes specific node from the container based off the passed value
		constexpr void remove(Node*& current, const Key& data) {
			if (current == nullptr) return;
			// 3 main cases in deletion: Node has no children, node has one child, or node has two children.
			if (current->data == data) {
				// Case 1: No children. We can simply delete the node and make it nullptr.
				if (current->left == nullptr && current->right == nullptr) {
					delete current;
					current = nullptr;
				}
				// Case 2: One children. We check which children the node got (Left or right?), delete the current node, and assign its only children Node to it (So no links are broken)
				else if (current->left == nullptr && current->right != nullptr) {
					Node* current_right = current->right;
					delete current;
					current = current_right;
					current_right = nullptr;
				}
				else if (current->right == nullptr && current->left != nullptr) {
					Node* current_left = current->left;
					delete current;
					current = current_left;
					current_left = nullptr;
				}
				// Case 3: Two children. Two ways to proceed: Either take the largest element of the left-child, or the smallest element from the right-child. Then we just delete the current node
				// And assign it the chosen child.
				else {
					// Get the node holding the min. value on the right side
					Node* right_min = get_min(current->right);
					// Assign that data to current
					current->data = right_min->data;
					// Delete the node we just assigned to current through a recursive call. Since the node we're deleting is the last one,
					// the deletion will happen in one of the above if cases.
					remove(current->right, right_min->data);
				}
				--m_size;
			}

			else if (data < current->data) {
				remove(current->left, data);
			}
			else if (data > current->data) {
				remove(current->right, data);
			}
			else return;

			if (current != nullptr) {
				current->balance_factor = get_height(current->left) - get_height(current->right);
				rebalance(current, data);
			}
		}

		constexpr bool find(Node* current, const Key& data) {
			if (current == nullptr) return false;
			else if (current->data == data) return true;
			else if (data < current->data) {
				return find(current->left, data);
			}
			else return find(current->right, data);
		}

	public:
		constexpr void remove(const Key& data) {
			remove(m_root, data);
		}

		constexpr bool contains(const Key& data) {
			return find(m_root, data);
		}
	};
}

#endif

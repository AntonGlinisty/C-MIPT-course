#include <iostream>

template <typename T, typename Allocator = std::allocator<T>>
class List {
 private:
  struct Node;

 public:
  template <bool IsConst>
  class CommonIterator;

  using value_type = T;
  using allocator_type = Allocator;

  using iterator = CommonIterator<false>;
  using const_iterator = CommonIterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  using alloc_traits = std::allocator_traits<Allocator>;
  using node_alloc = typename alloc_traits::template rebind_alloc<Node>;
  using node_alloc_traits = typename alloc_traits::template rebind_traits<Node>;

  friend class CommonIterator<false>;
  friend class CommonIterator<true>;

  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;
  const_iterator cbegin() const;
  const_iterator cend() const;
  reverse_iterator rbegin();
  reverse_iterator rend();
  const_reverse_iterator crbegin() const;
  const_reverse_iterator crend() const;

  void add_node_back(Node* ptr);
  void add_node_front(Node* ptr);

  List();
  List(size_t count, const T& value, const Allocator& alloc = Allocator());
  explicit List(size_t count, const Allocator& alloc = Allocator());
  List(const List& other);
  List(std::initializer_list<T> init, const Allocator& alloc = Allocator());
  List& operator=(const List& other);
  ~List();

  template <typename U>
  void swap(U& arg);

  size_t size() const;
  bool empty() const;

  T& front();
  const T& front() const;
  T& back();
  const T& back() const;

  void push_back(const T& value);
  void push_back(T&& value);
  void pop_back();
  void push_front(const T& value);
  void push_front(T&& value);
  void pop_front();

  void clear() noexcept;
  node_alloc& get_allocator();

 private:
  size_t size_ = 0;
  Node* head_ = nullptr;
  Node* tail_ = nullptr;
  node_alloc alloc_;
};

template <typename T, typename Allocator>
struct List<T, Allocator>::Node {
  Node() = default;
  Node(Node* left_ptr, Node* right_ptr, const T& val)
      : left(left_ptr), right(right_ptr), value(val) {}
  Node* left = nullptr;
  Node* right = nullptr;
  T value;
};

template <typename T, typename Allocator>
template <bool IsConst>
class List<T, Allocator>::CommonIterator {
 public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = std::conditional_t<IsConst, const T, T>;
  using pointer = std::conditional_t<IsConst, const T, T>*;
  using reference = std::conditional_t<IsConst, const T, T>&;
  using difference_type = std::ptrdiff_t;

  CommonIterator(std::conditional_t<IsConst, const List, List>* list,
                 Node* node)
      : list_(list), node_(node) {}

  reference operator*() const { return node_->value; }

  pointer operator->() const { return &node_->value; }

  CommonIterator& operator++() {
    node_ = node_->right;
    return *this;
  }

  CommonIterator operator++(int) {
    CommonIterator prev = *this;
    operator++();
    return prev;
  }

  CommonIterator& operator--() {
    if (node_ != nullptr) {
      node_ = node_->left;
    } else {
      node_ = list_->tail_;
    }
    return *this;
  }

  CommonIterator operator--(int) {
    CommonIterator prev = *this;
    operator--();
    return prev;
  }

  bool operator==(const CommonIterator& other) const {
    return node_ == other.node_;
  }

  bool operator!=(const CommonIterator& other) const {
    return node_ != other.node_;
  }

 private:
  std::conditional_t<IsConst, const List, List>* list_;
  Node* node_;
};

template <typename T, typename Allocator>
void List<T, Allocator>::add_node_back(Node* ptr) {
  if (tail_ != nullptr) {
    tail_->right = ptr;
    ptr->left = tail_;
  } else {
    head_ = ptr;
  }
  tail_ = ptr;
  ++size_;
}

template <typename T, typename Allocator>
void List<T, Allocator>::add_node_front(Node* ptr) {
  if (head_ != nullptr) {
    head_->left = ptr;
  } else {
    tail_ = ptr;
  }
  head_ = ptr;
  ++size_;
}

template <typename T, typename Allocator>
List<T, Allocator>::List() {
  alloc_ = Allocator();
}

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t count, const T& value, const Allocator& alloc) {
  alloc_ = alloc;
  Node* ptr = nullptr;
  try {
    for (size_t i = 0; i < count; ++i) {
      ptr = node_alloc_traits::allocate(alloc_, 1);
      node_alloc_traits::construct(alloc_, ptr, tail_, nullptr, value);
      add_node_back(ptr);
    }
  } catch (...) {
    if (ptr != nullptr) {
      node_alloc_traits::deallocate(alloc_, ptr, 1);
    }
    clear();
    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t count, const Allocator& alloc) {
  alloc_ = alloc;
  Node* ptr = nullptr;
  try {
    for (size_t i = 0; i < count; ++i) {
      ptr = node_alloc_traits::allocate(alloc_, 1);
      node_alloc_traits::construct(alloc_, ptr);
      add_node_back(ptr);
    }
  } catch (...) {
    if (ptr != nullptr) {
      node_alloc_traits::deallocate(alloc_, ptr, 1);
    }
    clear();
    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(const List& other) {
  alloc_ =
      node_alloc_traits::select_on_container_copy_construction(other.alloc_);
  Node* curr = other.head_;
  Node* ptr = nullptr;
  try {
    while (size_ != other.size_) {
      ptr = node_alloc_traits::allocate(alloc_, 1);
      node_alloc_traits::construct(alloc_, ptr, tail_, nullptr, curr->value);
      curr = curr->right;
      add_node_back(ptr);
    }
  } catch (...) {
    if (ptr != nullptr) {
      node_alloc_traits::deallocate(alloc_, ptr, 1);
    }
    clear();
    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(std::initializer_list<T> init,
                         const Allocator& alloc) {
  alloc_ = alloc;
  Node* ptr = nullptr;
  try {
    for (const auto& elem : init) {
      ptr = node_alloc_traits::allocate(alloc_, 1);
      node_alloc_traits::construct(alloc_, ptr, tail_, nullptr, elem);
      add_node_back(ptr);
    }
  } catch (...) {
    if (ptr != nullptr) {
      node_alloc_traits::deallocate(alloc_, ptr, 1);
    }
    clear();
    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>& List<T, Allocator>::operator=(const List& other) {
  if (this == &other) {
    return *this;
  }
  List copy(other);
  if (alloc_traits::propagate_on_container_copy_assignment::value) {
    alloc_ = other.alloc_;
  }
  swap(copy);
  return *this;
}

template <typename T, typename Allocator>
List<T, Allocator>::~List() {
  clear();
}

template <typename T, typename Allocator>
template <typename U>
void List<T, Allocator>::swap(U& arg) {
  Node* head_tmp = head_;
  Node* tail_tmp = tail_;
  size_t size_tmp = size_;
  head_ = arg.head_;
  tail_ = arg.tail_;
  size_ = arg.size_;
  arg.head_ = head_tmp;
  arg.tail_ = tail_tmp;
  arg.size_ = size_tmp;
}

template <typename T, typename Allocator>
typename List<T, Allocator>::iterator List<T, Allocator>::begin() {
  return iterator(this, head_);
}

template <typename T, typename Allocator>
typename List<T, Allocator>::iterator List<T, Allocator>::end() {
  return iterator(this, tail_->right);
}

template <typename T, typename Allocator>
typename List<T, Allocator>::const_iterator List<T, Allocator>::begin() const {
  return const_iterator(this, head_);
}

template <typename T, typename Allocator>
typename List<T, Allocator>::const_iterator List<T, Allocator>::end() const {
  return const_iterator(this, tail_->right);
}

template <typename T, typename Allocator>
typename List<T, Allocator>::const_iterator List<T, Allocator>::cbegin() const {
  return const_iterator(this, head_);
}

template <typename T, typename Allocator>
typename List<T, Allocator>::const_iterator List<T, Allocator>::cend() const {
  return const_iterator(this, tail_->right);
}

template <typename T, typename Allocator>
typename List<T, Allocator>::reverse_iterator List<T, Allocator>::rbegin() {
  return std::reverse_iterator(end());
}

template <typename T, typename Allocator>
typename List<T, Allocator>::reverse_iterator List<T, Allocator>::rend() {
  return std::reverse_iterator(begin());
}

template <typename T, typename Allocator>
typename List<T, Allocator>::const_reverse_iterator
List<T, Allocator>::crbegin() const {
  return const_reverse_iterator(cend());
}

template <typename T, typename Allocator>
typename List<T, Allocator>::const_reverse_iterator List<T, Allocator>::crend()
    const {
  return const_reverse_iterator(cbegin());
}

template <typename T, typename Allocator>
size_t List<T, Allocator>::size() const {
  return size_;
}

template <typename T, typename Allocator>
bool List<T, Allocator>::empty() const {
  return size_ == 0;
}

template <typename T, typename Allocator>
T& List<T, Allocator>::front() {
  return head_->value;
}

template <typename T, typename Allocator>
const T& List<T, Allocator>::front() const {
  return head_->value;
}

template <typename T, typename Allocator>
T& List<T, Allocator>::back() {
  return tail_->value;
}

template <typename T, typename Allocator>
const T& List<T, Allocator>::back() const {
  return tail_->value;
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_back(const T& value) {
  Node* ptr = node_alloc_traits::allocate(alloc_, 1);
  node_alloc_traits::construct(alloc_, ptr, tail_, nullptr, value);
  add_node_back(ptr);
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_back(T&& value) {
  push_back(value);
}

template <typename T, typename Allocator>
void List<T, Allocator>::pop_back() {
  Node* newtail = tail_->left;
  node_alloc_traits::destroy(alloc_, tail_);
  node_alloc_traits::deallocate(alloc_, tail_, 1);
  tail_ = newtail;
  if (tail_ != nullptr) {
    tail_->right = nullptr;
  }
  --size_;
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_front(const T& value) {
  Node* ptr = node_alloc_traits::allocate(alloc_, 1);
  node_alloc_traits::construct(alloc_, ptr, nullptr, head_, value);
  add_node_front(ptr);
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_front(T&& value) {
  push_front(value);
}

template <typename T, typename Allocator>
void List<T, Allocator>::pop_front() {
  Node* newhead = head_->right;
  node_alloc_traits::destroy(alloc_, head_);
  node_alloc_traits::deallocate(alloc_, head_, 1);
  head_ = newhead;
  if (head_ != nullptr) {
    head_->left = nullptr;
  }
  --size_;
}

template <typename T, typename Allocator>
void List<T, Allocator>::clear() noexcept {
  if (head_ != nullptr) {
    Node* curr = head_;
    while (curr != tail_) {
      Node* newcurr = curr->right;
      node_alloc_traits::destroy(alloc_, curr);
      node_alloc_traits::deallocate(alloc_, curr, 1);
      curr = newcurr;
    }
    node_alloc_traits::destroy(alloc_, curr);
    node_alloc_traits::deallocate(alloc_, curr, 1);
  }
  head_ = nullptr;
  tail_ = nullptr;
  size_ = 0;
}

template <typename T, typename Allocator>
typename List<T, Allocator>::node_alloc& List<T, Allocator>::get_allocator() {
  return alloc_;
}
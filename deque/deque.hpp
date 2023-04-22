#include <iostream>
#include <vector>

using my_pair = std::pair<size_t, size_t>;

const size_t kIntCap = 10000;
const size_t kOutCap = 10000;

struct Utility {
  static void move_pos_right(my_pair& pos) {
    if (pos.second == kIntCap - 1) {
      pos = {pos.first + 1, 0};
    } else {
      pos = {pos.first, pos.second + 1};
    }
  }

  static void move_pos_left(my_pair& pos) {
    if (pos.second == 0) {
      pos = {pos.first - 1, kIntCap - 1};
    } else {
      pos = {pos.first, pos.second - 1};
    }
  }

  static my_pair move_pos(const my_pair& pos, int value) {
    my_pair new_pos = pos;
    if (value >= 0) {
      new_pos.first = pos.first + (pos.second + value) / kIntCap;
      new_pos.second = (pos.second + value) % kIntCap;
    } else {
      size_t val = -value;
      if (val <= pos.second) {
        new_pos.second = new_pos.second - val;
      } else {
        new_pos.first = new_pos.first - (val - pos.second) / kIntCap - 1;
        new_pos.second = kIntCap - (val - pos.second) % kIntCap;
      }
    }
    return new_pos;
  }
};

template <typename T, typename Allocator = std::allocator<T>>
class Deque {
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

  Deque();
  Deque(const Allocator& alloc);
  Deque(const Deque& other);
  Deque(size_t count, const Allocator& alloc = Allocator());
  Deque(size_t count, const T& value, const Allocator& alloc = Allocator());
  Deque(Deque&& other);
  Deque(std::initializer_list<T> init, const Allocator& alloc = Allocator());
  ~Deque();

  void reallocate();

  Deque& operator=(const Deque& other);
  Deque&& operator=(Deque&& other);

  size_t size() const;
  bool empty() const;

  T& operator[](size_t val);
  const T& operator[](size_t val) const;
  T& at(size_t val);
  const T& at(size_t val) const;

  void push_back(const T& elem);
  void push_back(T&& elem);
  void push_front(const T& elem);
  void push_front(T&& elem);
  void pop_back();
  void pop_front();

  template <typename... Args>
  void emplace_back(Args&&... args);

  template <typename... Args>
  void emplace_front(Args&&... args);

  void insert(iterator iter, const T& value);

  template <typename... Args>
  void emplace(iterator iter, Args&&... args);

  void erase(iterator iter);

  void swap(Deque& other);
  void clear() noexcept;

  Allocator& get_allocator();

 private:
  Allocator alloc_;
  std::vector<T*> arr_;
  size_t out_cap_ = kOutCap;
  size_t int_cap_ = kIntCap;
  my_pair start_pos_;
  my_pair end_pos_;
};

template <typename T, typename Allocator>
template <bool IsConst>
class Deque<T, Allocator>::CommonIterator {
 public:
  using iterator_category = std::random_access_iterator_tag;
  using value_type = std::conditional_t<IsConst, const T, T>;
  using pointer = std::conditional_t<IsConst, const T, T>*;
  using reference = std::conditional_t<IsConst, const T, T>&;
  using difference_type = std::ptrdiff_t;

  CommonIterator(std::conditional_t<IsConst, const Deque, Deque>* obj,
                 my_pair curr_pos)
      : deque_(obj), curr_pos_(curr_pos) {}

  reference operator*() const {
    return deque_->arr_[curr_pos_.first][curr_pos_.second];
  }

  pointer operator->() const {
    return &deque_->arr_[curr_pos_.first][curr_pos_.second];
  }

  CommonIterator& operator++() {
    Utility::move_pos_right(curr_pos_);
    return *this;
  }

  CommonIterator operator++(int) {
    CommonIterator prev = *this;
    operator++();
    return prev;
  }

  CommonIterator& operator--() {
    Utility::move_pos_left(curr_pos_);
    return *this;
  }

  CommonIterator operator--(int) {
    CommonIterator prev = *this;
    operator--();
    return prev;
  }

  CommonIterator& operator+=(int value) {
    curr_pos_ = Utility::move_pos(curr_pos_, value);
    return *this;
  }

  CommonIterator operator+(int value) const {
    CommonIterator copy = *this;
    copy += value;
    return copy;
  }

  CommonIterator& operator-=(int value) {
    curr_pos_ = Utility::move_pos(curr_pos_, -value);
    return *this;
  }

  CommonIterator operator-(int value) const {
    CommonIterator copy = *this;
    copy -= value;
    return copy;
  }

  difference_type operator-(const CommonIterator& other) const {
    return deque_->int_cap_ * (curr_pos_.first - other.curr_pos_.first) +
           curr_pos_.second - other.curr_pos_.second;
  }

  bool operator<(const CommonIterator& other) const {
    if (curr_pos_.first == other.curr_pos_.first) {
      return curr_pos_.second < other.curr_pos_.second;
    }
    return curr_pos_.first < other.curr_pos_.first;
  }

  bool operator>(const CommonIterator& other) const {
    if (curr_pos_.first == other.curr_pos_.first) {
      return curr_pos_.second > other.curr_pos_.second;
    }
    return curr_pos_.first > other.curr_pos_.first;
  }

  bool operator<=(const CommonIterator& other) const {
    return !(*this > other);
  }

  bool operator>=(const CommonIterator& other) const {
    return !(*this < other);
  }

  bool operator==(const CommonIterator& other) const {
    return (!(*this < other) && !(*this > other));
  }

  bool operator!=(const CommonIterator& other) const {
    return ((*this < other) || (*this > other));
  }

 private:
  std::conditional_t<IsConst, const Deque, Deque>* deque_;
  my_pair curr_pos_;
};

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque() {
  alloc_ = Allocator();
  arr_.assign(out_cap_, nullptr);
  for (size_t i = 0; i < out_cap_; ++i) {
    arr_[i] = alloc_traits::allocate(alloc_, int_cap_);
  }
  start_pos_ = {out_cap_ / 2, int_cap_ / 2};
  end_pos_ = {out_cap_ / 2, int_cap_ / 2 - 1};
}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque(const Allocator& alloc) {
  alloc_ = alloc;
  arr_.assign(out_cap_, nullptr);
  for (size_t i = 0; i < out_cap_; ++i) {
    arr_[i] = alloc_traits::allocate(alloc_, int_cap_);
  }
  start_pos_ = {out_cap_ / 2, int_cap_ / 2};
  end_pos_ = {out_cap_ / 2, int_cap_ / 2 - 1};
}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque(const Deque& other) {
  out_cap_ = other.out_cap_;
  arr_.assign(out_cap_, nullptr);
  alloc_ = alloc_traits::select_on_container_copy_construction(other.alloc_);
  for (size_t i = 0; i < out_cap_; ++i) {
    arr_[i] = alloc_traits::allocate(alloc_, int_cap_);
  }
  start_pos_ = other.start_pos_;
  end_pos_ = start_pos_;
  Utility::move_pos_left(end_pos_);
  try {
    for (size_t i = 0; i < other.size(); ++i) {
      alloc_traits::construct(alloc_, &operator[](i), other[i]);
      Utility::move_pos_right(end_pos_);
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque(size_t count, const Allocator& alloc) {
  alloc_ = alloc;
  out_cap_ = std::max(out_cap_, count * 4 / int_cap_);
  arr_.assign(out_cap_, nullptr);
  for (size_t i = 0; i < out_cap_; ++i) {
    arr_[i] = alloc_traits::allocate(alloc_, int_cap_);
  }
  start_pos_ = {out_cap_ / 2, int_cap_ / 2};
  end_pos_ = {out_cap_ / 2, int_cap_ / 2 - 1};
  try {
    for (size_t i = 0; i < count; ++i) {
      alloc_traits::construct(alloc_, &operator[](i));
      Utility::move_pos_right(end_pos_);
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque(size_t count, const T& value,
                           const Allocator& alloc) {
  alloc_ = alloc;
  out_cap_ = std::max(out_cap_, count * 4 / int_cap_);
  arr_.assign(out_cap_, nullptr);
  for (size_t i = 0; i < out_cap_; ++i) {
    arr_[i] = alloc_traits::allocate(alloc_, int_cap_);
  }
  start_pos_ = {out_cap_ / 2, int_cap_ / 2};
  end_pos_ = {out_cap_ / 2, int_cap_ / 2 - 1};
  try {
    for (size_t i = 0; i < count; ++i) {
      alloc_traits::construct(alloc_, &operator[](i), value);
      Utility::move_pos_right(end_pos_);
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::swap(Deque<T, Allocator>& other) {
  std::swap(arr_, other.arr_);
  std::swap(alloc_, other.alloc_);
  std::swap(start_pos_, other.start_pos_);
  std::swap(end_pos_, other.end_pos_);
  std::swap(out_cap_, other.out_cap_);
}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque(Deque&& other) : Deque() {
  swap(other);
}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque(std::initializer_list<T> init,
                           const Allocator& alloc) {
  alloc_ = alloc;
  out_cap_ = std::max(out_cap_, init.size() * 4 / int_cap_);
  arr_.assign(out_cap_, nullptr);
  for (size_t i = 0; i < out_cap_; ++i) {
    arr_[i] = alloc_traits::allocate(alloc_, int_cap_);
  }
  start_pos_ = {out_cap_ / 2, int_cap_ / 2};
  end_pos_ = {out_cap_ / 2, int_cap_ / 2 - 1};
  int index = 0;
  try {
    for (const auto& elem : init) {
      alloc_traits::construct(alloc_, &operator[](index++), elem);
      Utility::move_pos_right(end_pos_);
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <typename T, typename Allocator>
Deque<T, Allocator>& Deque<T, Allocator>::operator=(
    const Deque<T, Allocator>& other) {
  if (this == &other) {
    return *this;
  }
  Deque copy(other);
  swap(copy);
  if (alloc_traits::propagate_on_container_copy_assignment::value) {
    alloc_ = other.alloc_;
  }
  return *this;
}

template <typename T, typename Allocator>
Deque<T, Allocator>&& Deque<T, Allocator>::operator=(
    Deque<T, Allocator>&& other) {
  if (this == &other) {
    return std::move(*this);
  }
  swap(other);
  if (alloc_traits::propagate_on_container_copy_assignment::value) {
    alloc_ = other.alloc_;
  }
  return std::move(*this);
}
template <typename T, typename Allocator>
void Deque<T, Allocator>::reallocate() {
  size_t new_out_cap = out_cap_ * 2;
  std::vector<T*> new_arr(new_out_cap);
  for (size_t i = 0; i < new_out_cap; ++i) {
    new_arr[i] = alloc_traits::allocate(alloc_, int_cap_);
  }
  my_pair new_start_pos = {start_pos_.first + out_cap_ / 2, start_pos_.second};
  my_pair new_end_pos = {end_pos_.first + out_cap_ / 2, end_pos_.second};
  my_pair curr_pos = new_start_pos;
  try {
    for (size_t i = 0; i < size(); ++i) {
      alloc_traits::construct(alloc_, &new_arr[curr_pos.first][curr_pos.second],
                              std::move(operator[](i)));
      Utility::move_pos_right(curr_pos);
    }
  } catch (...) {
    curr_pos = new_start_pos;
    for (size_t i = 0; i < size(); ++i) {
      alloc_traits::destroy(alloc_, &new_arr[curr_pos.first][curr_pos.second]);
      Utility::move_pos_right(curr_pos);
    }
    for (size_t i = 0; i < out_cap_; ++i) {
      alloc_traits::deallocate(alloc_, new_arr[i], int_cap_);
    }
    clear();
    throw;
  }
  clear();
  arr_ = new_arr;
  out_cap_ = new_out_cap;
  start_pos_ = new_start_pos;
  end_pos_ = new_end_pos;
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::clear() noexcept {
  for (size_t i = 0; i < size(); ++i) {
    alloc_traits::destroy(alloc_, &operator[](i));
  }
  for (size_t i = 0; i < out_cap_; ++i) {
    alloc_traits::deallocate(alloc_, arr_[i], int_cap_);
  }
  start_pos_ = {out_cap_ / 2, int_cap_ / 2};
  end_pos_ = {out_cap_ / 2, int_cap_ / 2 - 1};
  out_cap_ = 0;
}

template <typename T, typename Allocator>
Deque<T, Allocator>::~Deque() {
  clear();
}

template <typename T, typename Allocator>
typename Deque<T, Allocator>::iterator Deque<T, Allocator>::begin() {
  return iterator(this, start_pos_);
}

template <typename T, typename Allocator>
typename Deque<T, Allocator>::iterator Deque<T, Allocator>::end() {
  my_pair pos = end_pos_;
  Utility::move_pos_right(pos);
  return iterator(this, pos);
}

template <typename T, typename Allocator>
typename Deque<T, Allocator>::const_iterator Deque<T, Allocator>::begin()
    const {
  return const_iterator(this, start_pos_);
}

template <typename T, typename Allocator>
typename Deque<T, Allocator>::const_iterator Deque<T, Allocator>::end() const {
  my_pair pos = end_pos_;
  Utility::move_pos_right(pos);
  return const_iterator(this, pos);
}

template <typename T, typename Allocator>
typename Deque<T, Allocator>::const_iterator Deque<T, Allocator>::cbegin()
    const {
  return const_iterator(this, start_pos_);
}

template <typename T, typename Allocator>
typename Deque<T, Allocator>::const_iterator Deque<T, Allocator>::cend() const {
  my_pair pos = end_pos_;
  Utility::move_pos_right(pos);
  return const_iterator(this, pos);
}

template <typename T, typename Allocator>
typename Deque<T, Allocator>::reverse_iterator Deque<T, Allocator>::rbegin() {
  return std::reverse_iterator(end());
}

template <typename T, typename Allocator>
typename Deque<T, Allocator>::reverse_iterator Deque<T, Allocator>::rend() {
  return std::reverse_iterator(begin());
}

template <typename T, typename Allocator>
typename Deque<T, Allocator>::const_reverse_iterator
Deque<T, Allocator>::crbegin() const {
  return const_reverse_iterator(cend());
}

template <typename T, typename Allocator>
typename Deque<T, Allocator>::const_reverse_iterator
Deque<T, Allocator>::crend() const {
  return const_reverse_iterator(cbegin());
}

template <typename T, typename Allocator>
size_t Deque<T, Allocator>::size() const {
  return int_cap_ * (end_pos_.first - start_pos_.first) + end_pos_.second -
         start_pos_.second + 1;
}

template <typename T, typename Allocator>
bool Deque<T, Allocator>::empty() const {
  return size() == 0;
}

template <typename T, typename Allocator>
T& Deque<T, Allocator>::operator[](size_t val) {
  my_pair pos = Utility::move_pos(start_pos_, val);
  return arr_[pos.first][pos.second];
}

template <typename T, typename Allocator>
const T& Deque<T, Allocator>::operator[](size_t val) const {
  my_pair pos = Utility::move_pos(start_pos_, val);
  return arr_[pos.first][pos.second];
}

template <typename T, typename Allocator>
T& Deque<T, Allocator>::at(size_t val) {
  if (val >= size()) {
    throw std::out_of_range("out_of_range in at()");
  }
  return operator[](val);
}

template <typename T, typename Allocator>
const T& Deque<T, Allocator>::at(size_t val) const {
  if (val >= size()) {
    throw std::out_of_range("out_of_range in const at()");
  }
  return operator[](val);
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::push_back(const T& elem) {
  if (end_pos_.first == out_cap_ - 1) {
    reallocate();
  }
  try {
    alloc_traits::construct(alloc_, &operator[](size()), elem);
  } catch (...) {
    alloc_traits::destroy(alloc_, &operator[](size()));
    throw;
  }
  Utility::move_pos_right(end_pos_);
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::push_back(T&& elem) {
  if (end_pos_.first == out_cap_ - 1) {
    reallocate();
  }
  try {
    alloc_traits::construct(alloc_, &operator[](size()), std::move(elem));
  } catch (...) {
    alloc_traits::destroy(alloc_, &operator[](size()));
    throw;
  }
  Utility::move_pos_right(end_pos_);
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::push_front(const T& elem) {
  if (start_pos_.first == 0) {
    reallocate();
  }
  Utility::move_pos_left(start_pos_);
  try {
    alloc_traits::construct(alloc_, &operator[](0), elem);
  } catch (...) {
    alloc_traits::destroy(alloc_, &operator[](0));
    Utility::move_pos_right(end_pos_);
    throw;
  }
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::push_front(T&& elem) {
  if (start_pos_.first == 0) {
    reallocate();
  }
  Utility::move_pos_left(start_pos_);
  try {
    alloc_traits::construct(alloc_, &operator[](0), std::move(elem));
  } catch (...) {
    alloc_traits::destroy(alloc_, &operator[](0));
    Utility::move_pos_right(end_pos_);
    throw;
  }
}

template <typename T, typename Allocator>
template <typename... Args>
void Deque<T, Allocator>::emplace_back(Args&&... args) {
  if (end_pos_.first == out_cap_ - 1) {
    reallocate();
  }
  try {
    alloc_traits::construct(alloc_, &operator[](size()),
                            std::forward<Args>(args)...);
  } catch (...) {
    alloc_traits::destroy(alloc_, &operator[](size()));
    throw;
  }
  Utility::move_pos_right(end_pos_);
}

template <typename T, typename Allocator>
template <typename... Args>
void Deque<T, Allocator>::emplace_front(Args&&... args) {
  if (start_pos_.first == 0) {
    reallocate();
  }
  Utility::move_pos_left(start_pos_);
  try {
    alloc_traits::construct(alloc_, &operator[](0),
                            std::forward<Args>(args)...);
  } catch (...) {
    alloc_traits::destroy(alloc_, &operator[](0));
    Utility::move_pos_right(end_pos_);
    throw;
  }
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::pop_back() {
  alloc_traits::destroy(alloc_, &operator[](size() - 1));
  Utility::move_pos_left(end_pos_);
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::pop_front() {
  alloc_traits::destroy(alloc_, &operator[](0));
  Utility::move_pos_right(start_pos_);
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::insert(Deque<T, Allocator>::iterator iter,
                                 const T& value) {
  auto new_iter = iter;
  if (end_pos_.first == out_cap_ - 1) {
    reallocate();
    new_iter += int_cap_ * out_cap_ / 4;
  }
  push_back(value);
  while (new_iter != end()) {
    std::swap(*new_iter, operator[](size() - 1));
    ++new_iter;
  }
}

template <typename T, typename Allocator>
template <typename... Args>
void Deque<T, Allocator>::emplace(iterator iter, Args&&... args) {
  auto new_iter = iter;
  if (end_pos_.first == out_cap_ - 1) {
    reallocate();
    new_iter += int_cap_ * out_cap_ / 4;
  }
  emplace_back(args...);
  while (new_iter != end()) {
    std::swap(*new_iter, operator[](size() - 1));
    ++new_iter;
  }
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::erase(iterator iter) {
  while (iter + 1 < end()) {
    std::swap(*iter, *(iter + 1));
    ++iter;
  }
  pop_back();
}

template <typename T, typename Allocator>
Allocator& Deque<T, Allocator>::get_allocator() {
  return alloc_;
}

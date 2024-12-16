#ifndef MY_VECTOR_H_
#define MY_VECTOR_H_

#include <cstddef>

#include <iostream>
#include <algorithm>
#include <exception>
#include <memory>
#include <optional>
#include <utility>

template <typename T>
struct RawMemory {
  static T* Allocate(int n) {
    return reinterpret_cast<T*>(operator new(n * sizeof(T)));
  }

  static void Deallocate(T* buf_) {
    operator delete(buf_);
  }

  RawMemory() = default;

  explicit RawMemory(size_t n) {
    buf_ = Allocate(n + 1);
    cp_ = n;
  }

  RawMemory(const RawMemory& other) = delete;

  explicit RawMemory(RawMemory&& other) {
    Swap(std::move(other));
  }

  ~RawMemory() {
    Deallocate(buf_);
  }

  RawMemory& operator=(const RawMemory& other) = delete;

  RawMemory& operator=(RawMemory&& other) {
    Swap(other);
  }

  T* operator+(int i) {
    return buf_ + i;
  }

  const T* operator+(int i) const {
    return buf_ + i;
  }

  T& operator[](size_t i) {
    return buf_[i];
  }

  const T& operator[](size_t i) const {
    return buf_[i];
  }

  void Swap(RawMemory& other) {
    std::swap(buf_, other.buf_);
    std::swap(cp_, other.cp_);
  }

  T *buf_ = nullptr;
  int cp_ = 0;
};

template <typename T>
class Vector {
 public:
  static void Construct(T* buf_) {
    new (buf_) T();
  }

  static void Construct(T* buf_, const T& elem) {
    new (buf_) T(elem);
  }

  static void Construct(T* buf_, T&& elem) {
    new (buf_) T(std::move(elem));
  }

  static void Destroy(T* buf_) {
    buf_->~T();
  }

  Vector() = default;

  explicit Vector(size_t n) : data_(n) {
    std::uninitialized_value_construct_n(data_.buf_, n);
    sz_ = n;
  }

  Vector(const Vector& other) : data_(other.sz_) {
    std::uninitialized_copy_n(other.data_.buf_, other.sz_, data_.buf_);
    sz_ = other.sz_;
  }

  Vector(Vector&& other) noexcept {
    Swap(other);
  }

  ~Vector() {
    std::destroy_n(data_.buf_, sz_);
  }

  Vector& operator=(const Vector& other) {
    if (other.sz_ > data_.cp_) {
      Vector data2(other);
      Swap(data2);
    } else {
      for (int i = 0; i < std::min(sz_, other.sz_); ++i) {
        data_[i] = other[i];
      }
      if (other.sz_ < sz_) {
        std::destroy_n(data_.buf_ + other.sz_, sz_ - other.sz_);
      } else if (other.sz_ > sz_) {
        std::uninitialized_copy_n(
            other.data_ + sz_, other.sz_ - sz_, data_ + sz_);
      }
      sz_ = other.sz_;
    }
    return *this;
  }

  Vector& operator=(Vector&& other) noexcept {
    Swap(other);
    return *this;
  }

  void Swap(Vector& other) {
    data_.Swap(other.data_);
    std::swap(sz_, other.sz_);
  }

  void Reserve(size_t n) {
    if (n > data_.cp_) {
      RawMemory<T> data2(n);
      std::uninitialized_move_n(data_.buf_, data_.cp_, data2.buf_);
      std::destroy_n(data_.buf_, sz_);
      data_.Swap(data2);
    }
  }

  void Resize(size_t n) {
    Reserve(n);
    if (n < sz_) {
      std::destroy_n(data_.buf_ + n, sz_ - n);
    } else if (n > sz_) {
      std::uninitialized_value_construct_n(data_.buf_ + sz_, n - sz_);
    }
    sz_ = n;
  }

  void PushBack(const T& elem) {
    if (sz_ == data_.cp_) {
      Reserve(sz_ == 0 ? 1 : sz_ * 2);
    }
    Construct(data_.buf_ + sz_, elem);
    ++sz_;
  }

  void PushBack(T&& elem) {
    if (sz_ == data_.cp_) {
      Reserve(sz_ == 0 ? 1 : sz_ * 2);
    }
    Construct(data_.buf_ + sz_, std::move(elem));
    ++sz_;
  }

  template <typename ... Args>
  T& EmplaceBack(Args&&... args) {
    if (sz_ == data_.cp_) {
      Reserve(sz_ == 0 ? 1 : data_.cp_ * 2);
    }
    auto item = new (data_.buf_ + sz_) T(std::forward<Args>(args)...);
    ++sz_;
    return *item;
  }

  void PopBack() {
    std::destroy_at(data_.buf_ + sz_ - 1);
    --sz_;
  }

  size_t Size() const noexcept {
    return sz_;
  }

  size_t Capacity() const noexcept {
    return data_.cp_;
  }

  const T& operator[](size_t i) const {
    return data_[i];
  }

  T& operator[](size_t i) {
    return data_[i];
  }

  using iterator = T*;
  using const_iterator = const T*;

  iterator begin() noexcept {
    return data_.buf_;
  }

  iterator end() noexcept {
    return (data_ + sz_);
  }

  const_iterator begin() const noexcept {
    return data_.buf_;
  }
  const_iterator end() const noexcept {
    return (data_ + sz_);
  }

  const_iterator cbegin() const noexcept {
    return data_.buf_;
  }
  const_iterator cend() const noexcept {
    return (data_ + sz_);
  }

  iterator Insert(const_iterator pos, const T& elem) {
    size_t idx = pos - begin();
    RawMemory<T> tmp(sz_ == data_.cp_ ? (sz_ == 0 ? 1 : 2 * sz_) : data_.cp_);
    std::uninitialized_move_n(data_.buf_, idx, tmp.buf_);
    Construct(tmp + idx, elem);
    if (sz_ > idx) {
      std::uninitialized_move_n(data_.buf_ + idx, sz_ - idx, tmp.buf_ + idx + 1);
    }
    data_.Swap(tmp);
    ++sz_;
    return (begin() + idx);
  }

  iterator Insert(const_iterator pos, T&& elem) {
    size_t idx = pos - begin();
    RawMemory<T> tmp(sz_ == data_.cp_ ? (sz_ == 0 ? 1 : 2 * sz_) : data_.cp_);
    std::uninitialized_move_n(data_.buf_, idx, tmp.buf_);
    Construct(tmp + idx, std::move(elem));
    if (sz_ > idx) {
      std::uninitialized_move_n(data_.buf_ + idx, sz_ - idx, tmp.buf_ + idx + 1);
    }
    data_.Swap(tmp);
    ++sz_;
    return (begin() + idx);
  }

  template <typename ... Args>
  iterator Emplace(const_iterator it, Args&&... args) {
    return Insert(it, T(std::forward<Args>(args)...));
  }

  iterator Erase(const_iterator it) {
    size_t idx = it - begin();
    RawMemory<T> tmp(data_.cp_);
    std::uninitialized_move_n(data_.buf_, idx, tmp.buf_);
    if (sz_ > idx) {
      std::uninitialized_move_n(data_.buf_ + idx + 1, sz_ - idx, tmp.buf_ + idx);
    }
    data_.Swap(tmp);
    --sz_;
    return (begin() + idx);
  }

 private:
  RawMemory<T> data_;
  int sz_ = 0;
};

#endif // MY_VECTOR_H_

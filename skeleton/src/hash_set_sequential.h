#ifndef HASH_SET_SEQUENTIAL_H
#define HASH_SET_SEQUENTIAL_H

#include <functional>
#include <vector>
#include <algorithm>

#include "src/hash_set_base.h"

template <typename T>
class HashSetSequential : public HashSetBase<T> {
 public:
  explicit HashSetSequential(size_t initial_capacity): size_(0), capacity_(initial_capacity), table_(initial_capacity) {
  }

  bool Add(T elem) final {
    size_t idx = Hash_(elem);

    bool contains = Contains_(elem);

    if (contains) return false;

    table_[idx].push_back(elem);
    size_++;

    if (Policy_()) {
      Resize_();
    }

    return true;
  }

  bool Remove(T elem) final {
    size_t idx = Hash_(elem);
    bool contains = Contains_(elem);

    if (contains) {
      std::erase(table_[idx], elem);
      size_--;
      return true;
    }

    return false;
    
  }

  [[nodiscard]] bool Contains(T elem) final {
    return Contains_(elem);
  }

  [[nodiscard]] size_t Size() const final {
    return size_;
  }

 private:
  size_t size_;     // # of elements in the set
  size_t capacity_; // # of buckets
  std::vector<std::vector<T>> table_; // vector of vectors

  bool Contains_(T elem) {
    size_t idx = Hash_(elem);
    std::vector<T>& bucket = table_[idx];

    for (T& item: bucket) {
      if (item == elem) return true;
    }
    return false;
  }

  bool Policy_() {
    return ((size_ * 4) >= (capacity_ * 3));
  }

  void Resize_() {
    capacity_ = std::max(capacity_ * 2, 1);
    std::vector<std::vector<T>> new_table(capacity_);
    for (auto& vctr: table_) {
      for (auto& x: vctr) {
        size_t idx = Hash_(x);
        new_table[idx].push_back(x);
      }
    }
    table_ = new_table;
  }

  size_t Hash_(T elem) {
    return std::hash<T>()(elem) % capacity_;
  }


};

#endif  // HASH_SET_SEQUENTIAL_H

#ifndef HASH_SET_STRIPED_H
#define HASH_SET_STRIPED_H

// once unique pointer goes out of scope the resource it was pointing to dies
#include <cassert>
#include <algorithm>
#include <functional>
#include <mutex>
#include <vector>
#include <memory>

#include "src/hash_set_base.h"

template <typename T>
class HashSetStriped : public HashSetBase<T> {
 public:
  explicit HashSetStriped(size_t initial_capacity): table_(std::max(initial_capacity, size_t{1})), capacity_(std::max(initial_capacity, size_t{1})) {
    size_t lock_count = std::max(initial_capacity, size_t{1});
    for (int i = 0; i < lock_count; i++) {
      std::mutex m;
      locks_[i] = std::make_unique(m)
    }
  }

  bool Add(T elem) final {
    size_t idx = Hash_(elem);`1
    locks_[idx & locks_.size()].get()->lock();

    bool contains = Contains_(elem);

    if (contains) {
      locks_[idx & locks_.size()].get()->unlock();
       return false
      };

    table_[idx].push_back(elem);
    size_++;

    if (Policy_()) {
      
      Resize_();
    }

    return true;
  }

  bool Remove(T elem) final {
    size_t idx = Hash_(elem);
    locks_[idx & locks_.size()].get()->lock();

    bool contains = Contains_(elem);

    if (contains) {
      std::erase(table_[idx], elem);
      size_--;
      return true;
    }
    return false;
  }

  [[nodiscard]] bool Contains(T elem) final {
    size_t idx = Hash_(elem);
    locks_[idx & locks_.size()].get()->lock();
    return Contains_(elem);
    locks_[idx & locks_.size()].get()->lock();
  }

  [[nodiscard]] size_t Size() const final {
    for (auto lck: locks_) {
      lck->lock();
    }
    return size_;
    for (auto lck: locks_) {
      lck.get() ->unlock();
    }

    
  }

 private:
    mutable std::mutex m;
    size_t size_{0};     // # of elements in the set
    size_t capacity_; // # of buckets
    std::vector<std::vector<T>> table_; // vector of vectors
    std::vector<std::unique_ptr<std::mutex>> locks_;

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
    size_t old_capacity = capacity_;
    for (auto lck: locks_) {
      lck.get() ->lock();
    }
    if (table_.size() != old_capacity) return;
    capacity_ = old_capacity * 2;
    std::vector<std::vector<T>> new_table(capacity_);
    for (auto& vctr: table_) {
      for (auto& x: vctr) {
        size_t idx = Hash_(x);
        new_table[idx].push_back(x);
      }
    }
    table_ = new_table;

    for (auto lck: locks_) {
      lck.get() ->unlock();
    }
  }

  size_t Hash_(T elem) {
    return std::hash<T>()(elem) % capacity_;
  }
};




#endif  // HASH_SET_STRIPED_H

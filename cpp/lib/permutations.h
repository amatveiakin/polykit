#pragma once

#include <vector>

#include "absl/algorithm/container.h"

#include "check.h"


template<typename T>
class Permutations {
public:
  class EndIterator {};

  class Iterator {
  public:
    explicit Iterator(std::vector<T> current_permutation)
        : current_permutation_(std::move(current_permutation)) {}

    // Avoid accidental (expensive) copying.
    Iterator(const Iterator&) = delete;
    Iterator& operator=(const Iterator&) = delete;

    const std::vector<T>& operator*() const {
      return current_permutation_;
    }
    void operator++() {
      CHECK(!done_);
      done_ = !absl::c_next_permutation(current_permutation_);
    }
    bool operator==(const EndIterator&) const { return done_; }
    bool operator!=(const EndIterator&) const { return !done_; }

  private:
    std::vector<T> current_permutation_;
    bool done_ = false;
  };

  Permutations(std::vector<T> elements)
      : current_permutation_(std::move(elements)) {
    absl::c_sort(current_permutation_);
  }

  Iterator begin() const { return Iterator(current_permutation_); }
  EndIterator end() const { return EndIterator(); }

private:
  std::vector<T> current_permutation_;
};


template<typename T>
Permutations<T> permutations(std::vector<T> elements) {
  return {std::move(elements)};
}

template<typename T>
Permutations<T> permutations(std::initializer_list<T> elements) {
  return {std::vector(elements)};
}

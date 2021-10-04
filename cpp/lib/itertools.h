// Analogs of combinatoric iterators from Python `itertools` package:
// https://docs.python.org/3/library/itertools.html
//
// * cartesian_power(p, repeat)
//   Equivalent of `itertools.product(p, repeat=repeat)`
//
// * cartesian_product(p...)
//   Equivalent of `itertools.product(p...)` without `repeat`.
//
// * permutations(p, [r])
//   r-length tuples, all possible orderings, no repeated elements
//   Note: the order may be different from the one `itertools` produces!
//
// * combinations(p, r)
//   r-length tuples, in sorted order[*], no repeated elements
//
// * combinations_with_replacement(p, r)
//   r-length tuples, in sorted order[*], with repeated elements
//
// [*] Here "sorted order" means the order in argument list, not the order of
//   how elements compare with operator<.
//
// Some of the functions produce iterators that generate values on the fly.
// Some functions generate the entire array of values immediately.
// Optimization potential: Turn everything into generators.
//
// TODO: Have both tuple and vector version of each function.

#pragma once

#include <vector>

#include "absl/algorithm/container.h"

#include "check.h"
#include "range.h"
#include "util.h"


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


namespace internal {

template<typename T>
void fill_cartesian_product(
  const std::vector<T>& elements,
  std::vector<T>& buffer,
  int target_size,
  std::vector<std::vector<T>>& result
) {
  if (buffer.size() == target_size) {
    result.push_back(buffer);
  } else {
    for (const T& e : elements) {
      buffer.push_back(e);
      fill_cartesian_product(elements, buffer, target_size, result);
      buffer.pop_back();
    }
  }
}

template<typename T>
void fill_combinations(
  const std::vector<T>& elements,
  int next_element,
  std::vector<T>& buffer,
  int target_size,
  std::vector<std::vector<T>>& result
) {
  if (buffer.size() == target_size) {
    result.push_back(buffer);
  } else {
    const int extra_elements_required = target_size - buffer.size() - 1;
    for (int i : range(next_element, elements.size() - extra_elements_required)) {
      buffer.push_back(elements[i]);
      fill_combinations(elements, i + 1, buffer, target_size, result);
      buffer.pop_back();
    }
  }
}

template<typename T>
void fill_combinations_with_replacement(
  const std::vector<T>& elements,
  int next_element,
  std::vector<T>& buffer,
  int target_size,
  std::vector<std::vector<T>>& result
) {
  if (buffer.size() == target_size) {
    result.push_back(buffer);
  } else {
    for (int i : range(next_element, elements.size())) {
      buffer.push_back(elements[i]);
      fill_combinations_with_replacement(elements, i, buffer, target_size, result);
      buffer.pop_back();
    }
  }
}

template<typename T>
void fill_cartesian_combinations(
  const std::vector<std::pair<std::vector<T>, int>>& elements_and_powers,
  int next_element,
  std::vector<T>& buffer,
  std::vector<std::vector<T>>& result
) {
  if (next_element == elements_and_powers.size()) {
    result.push_back(buffer);
  } else {
    const auto& [elements, power] = elements_and_powers[next_element];
    for (const auto& seq : combinations(elements, power)) {
      append_vector(buffer, seq);
      fill_cartesian_combinations(elements_and_powers, next_element + 1, buffer, result);
      buffer.erase(buffer.end() - seq.size(), buffer.end());
    }
  }
}

}  // namespace internal


template<typename T>
std::vector<std::tuple<T>> cartesian_product(const std::vector<T>& elements) {
  return mapped(elements, [](const T& x) { return std::tuple{x}; });
}

template<typename T, typename... Tail>
auto cartesian_product(const std::vector<T>& head, const Tail&... tail) {
  const auto tail_product = cartesian_product(tail...);
  std::vector<decltype(std::tuple_cat(std::declval<std::tuple<T>>(), tail_product.front()))> ret;
  for (const T& h : head) {
    for (const auto& t : tail_product) {
      ret.push_back(std::tuple_cat(std::tuple(h), t));
    }
  }
  return ret;
}


template<typename T>
std::vector<std::vector<T>> cartesian_power(const std::vector<T>& elements, int repeat) {
  std::vector<std::vector<T>> ret;
  std::vector<T> buffer;
  internal::fill_cartesian_product(elements, buffer, repeat, ret);
  CHECK(buffer.empty());
  return ret;
}

template<typename T>
std::vector<std::vector<T>> cartesian_power(std::initializer_list<T> elements, int repeat) {
  return cartesian_power(std::vector(elements), repeat);
}


template<typename T>
std::vector<std::vector<T>> combinations(std::vector<T> elements, int size) {
  CHECK_LE(0, size);
  std::vector<std::vector<T>> ret;
  std::vector<T> buffer;
  internal::fill_combinations(elements, 0, buffer, size, ret);
  CHECK(buffer.empty());
  return ret;
}

template<typename T>
std::vector<std::vector<T>> combinations(std::initializer_list<T> elements, int size) {
  return combinations(std::vector(elements), size);
}


template<typename T>
std::vector<std::vector<T>> combinations_with_replacement(std::vector<T> elements, int size) {
  CHECK_LE(0, size);
  std::vector<std::vector<T>> ret;
  std::vector<T> buffer;
  internal::fill_combinations_with_replacement(elements, 0, buffer, size, ret);
  CHECK(buffer.empty());
  return ret;
}

template<typename T>
std::vector<std::vector<T>> combinations_with_replacement(std::initializer_list<T> elements, int size) {
  return combinations_with_replacement(std::vector(elements), size);
}


template<typename T>
std::vector<std::vector<T>> permutations(const std::vector<T>& elements, int size) {
  CHECK_LE(0, size);
  std::vector<std::vector<T>> ret;
  for (const auto& c : combinations(elements, size)) {
    for (const auto& p : permutations(c)) {
      ret.push_back(p);
    }
  }
  return ret;
}

template<typename T>
std::vector<std::vector<T>> permutations(std::initializer_list<T> elements, int size) {
  return permutations(std::vector(elements), size);
}


// Like `cartesian_product(combinations(...), ..., combinations(...))`, but the result is not nested.
// TODO: Find a more adequate solution for this.
template<typename T>
std::vector<std::vector<T>> cartesian_combinations(const std::vector<std::pair<std::vector<T>, int>>& elements_and_powers) {
  std::vector<std::vector<T>> ret;
  std::vector<T> buffer;
  internal::fill_cartesian_combinations(elements_and_powers, 0, buffer, ret);
  CHECK(buffer.empty());
  return ret;
}

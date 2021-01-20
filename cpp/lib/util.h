#pragma once

#include "absl/algorithm/container.h"
#include "absl/container/inlined_vector.h"
#include "absl/types/span.h"

#include "check.h"


// For std::visit
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;


inline int pos_mod(int x, int y) {
  CHECK_GT(y, 0);
  return (x % y + y) % y;
}

inline int neg_one_pow(int power) {
  CHECK_GE(power, 0);
  return 1 - power % 2 * 2;
}

class IntegerDivisionError : public std::runtime_error {
public:
  IntegerDivisionError() : std::runtime_error("Integer division error") {}
};

inline int div_int(int a, int b) {
  if (a % b != 0) {
    throw IntegerDivisionError();
  }
  return a / b;
}

inline int factorial(int n) {
  CHECK_GE(n, 0);
  int ret = 1;
  for (int i = 1; i <= n; ++i) {
    ret *= i;
  }
  return ret;
}

template<typename T>
void sort_two(T& a, T& b) {
  if (b < a) {
    std::swap(a, b);
  }
}

inline std::vector<int> seq_incl(int from, int to) {
  CHECK_LE(from, to);
  std::vector<int> ret(to - from + 1);
  absl::c_iota(ret, from);
  return ret;
}

template<int From, int To>
inline std::array<int, To - From + 1> seq_incl_array() {
  std::array<int, To - From + 1> ret;
  absl::c_iota(ret, From);
  return ret;
}

// Equivalent of std::identity from C++20
struct identity_function_t {
  template<class T>
  constexpr T&& operator()(T&& t) const noexcept {
    return std::forward<T>(t);
  }
};
constexpr identity_function_t identity_function;

template<typename Container>
Container concat(Container a, Container b) {
  a.insert(a.end(), std::move_iterator(b.begin()), std::move_iterator(b.end()));
  return a;
}

template<typename T>
std::vector<T> slice(const std::vector<T>& v, int from, int to = -1) {
  if (to == -1) {
    to = v.size();
  }
  CHECK(0 <= from && from <= v.size());
  CHECK(0 <= to && to <= v.size());
  return std::vector<T>(v.begin() + from, v.begin() + to);
}

template<typename T>
std::vector<T> removed_slice(std::vector<T> v, int from, int to = -1) {
  if (to == -1) {
    to = v.size();
  }
  CHECK(0 <= from && from <= v.size());
  CHECK(0 <= to && to <= v.size());
  v.erase(v.begin() + from, v.begin() + to);
  return v;
}

template<typename T>
std::vector<T> removed_index(std::vector<T> v, int index) {
  v.erase(v.begin() + index);
  return v;
}

template<typename T>
std::vector<T> removed_indices(std::vector<T> v, std::vector<int> indices) {
  absl::c_sort(indices);
  // Optimization potential: O(N) solution.
  for (auto it = indices.rbegin(); it != indices.rend(); ++it) {
    v.erase(v.begin() + *it);
  }
  return v;
}

template<typename T>
std::vector<T> choose_indices(const std::vector<T>& v, const std::vector<int>& indices) {
  std::vector<T> ret;
  ret.reserve(indices.size());
  for (int idx : indices) {
    ret.push_back(v.at(idx));
  }
  return ret;
}

template<typename T, size_t N, size_t M>
std::array<T, M> choose_indices(const std::array<T, N>& v, const std::array<int, M>& indices) {
  std::array<T, M> ret;
  for (int i = 0; i < indices.size(); ++i) {
    ret[i] = v.at(indices[i]);
  }
  return ret;
}

template<typename T, size_t N>
std::array<T, N> permute(const std::array<T, N>& v, const std::array<int, N>& indices) {
  return choose_indices(v, indices);
}

template<typename Src, typename F>
auto mapped(const Src& src, F&& func) {
  std::vector<std::invoke_result_t<F, typename Src::value_type>> dst(src.size());
  absl::c_transform(src, dst.begin(), std::forward<F>(func));
  return dst;
}

template<typename In, size_t N, typename F>
auto mapped_array(const std::array<In, N>& src, F&& func) {
  std::array<std::invoke_result_t<F, In>, N> dst;
  absl::c_transform(src, dst.begin(), func);
  return dst;
}

template<typename T, typename F>
std::vector<T> filtered(std::vector<T> src, F&& func) {
  src.erase(
    std::remove_if(src.begin(), src.end(), [&](const T& element) {
      return !func(element);
    }),
    src.end()
  );
  return src;
}

inline std::vector<int> odd_elements(std::vector<int> v) {
  return filtered(std::move(v), [](int x) { return x % 2 == 1; });
}
inline std::vector<int> even_elements(std::vector<int> v) {
  return filtered(std::move(v), [](int x) { return x % 2 == 0; });
}

template<typename Container, typename T>
bool contains_naive(const Container& c, const T& value) {
  return absl::c_find(c, value) != c.end();
}

template<typename Container, typename T>
int element_index(const Container& c, const T& value) {
  const auto it = absl::c_find(c, value);
  CHECK(it != c.end()) << list_to_string(c) << " does not contain " << value;
  return it - c.begin();
}

template<typename T>
std::vector<T> rotated_vector(std::vector<T> v, int n) {
  n = pos_mod(n, v.size());
  absl::c_rotate(v, v.begin() + n);
  return v;
}

template<typename T, typename SrcContainerT>
void append_vector(std::vector<T>& dst, SrcContainerT&& src) {
  const size_t old_size = dst.size();
  dst.resize(old_size + src.size());
  absl::c_move(src, dst.begin() + old_size);
}

template<typename Container>
Container sorted(Container c) {
  absl::c_sort(c);
  return c;
}

template<typename Container>
Container reversed(Container c) {
  absl::c_reverse(c);
  return c;
}

template<typename T>
void keep_unique(std::vector<T>& v) {
  v.erase(std::unique(v.begin(), v.end()), v.end());
}

template<typename Container>
int num_distinct_elements(Container v) {
  absl::c_sort(v);
  return std::unique(v.begin(), v.end()) - v.begin();
}

template<size_t N, typename T>
std::array<T, N> to_array(std::vector<T> v) {
  CHECK_EQ(v.size(), N);
  std::array<T, N> ret;
  absl::c_move(v, ret.begin());
  return ret;
}

template<typename T, size_t N>
std::vector<T> to_vector(std::array<T, N> v) {
  std::vector<T> ret;
  absl::c_move(v, std::back_inserter(ret));
  return ret;
}

template<typename Container>
bool all_equal(const Container& c) {
  return absl::c_adjacent_find(c, std::not_equal_to<>()) == c.end();
}

template<typename Container>
std::vector<Container> all_permutations(const Container& c) {
  std::vector<int> indices(c.size());
  absl::c_iota(indices, 0);
  std::vector<Container> ret;
  do {
    ret.push_back(choose_indices(c, indices));
  } while (absl::c_next_permutation(indices));
  return ret;
}

template<typename T>
std::vector<T> set_intersection(const std::vector<T>& a, const std::vector<T>& b) {
  CHECK(absl::c_is_sorted(a)) << list_to_string(a);
  CHECK(absl::c_is_sorted(b)) << list_to_string(b);
  std::vector<T> ret;
  absl::c_set_intersection(a, b, std::back_inserter(ret));
  return ret;
}

template<typename T>
int set_intersection_size(const std::vector<T>& a, const std::vector<T>& b) {
  // Optimization potential: Use a fake iterators that counts instead of back_inserter.
  return set_intersection(a, b).size();
}

// TODO(C++20): Replace with / rewrite via std::ranges or range-v3.

#pragma once

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_set.h"
#include "absl/container/inlined_vector.h"
#include "absl/types/span.h"

#include "check.h"
#include "functional.h"
#include "range.h"


// For std::visit
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;


inline int pos_mod(int x, int y) {
  CHECK_GT(y, 0);
  return (x % y + y) % y;
}

inline int mod_eq(int a, int b, int mod) {
  return pos_mod(a, mod) == pos_mod(b, mod);
}

inline int neg_one_pow(int power) {
  CHECK_GE(power, 0);
  return 1 - power % 2 * 2;
}

inline int int_pow(int x, int p) {
  CHECK_LE(0, p);
  int ret = 1;
  while (p > 0) {
    if (p % 2 == 1) {
      ret *= x;
    }
    x *= x;
    p /= 2;
  }
  return ret;
}

inline int div_round_up(int a, int b) {
  return (a + b - 1) / b;
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

// TODO(C++20): Replace with std::identity.
struct identity_function_t {
  template<class T>
  constexpr T&& operator()(T&& t) const noexcept {
    return std::forward<T>(t);
  }
};
constexpr identity_function_t identity_function;

// For using together with `mapped`.
template<typename T>
struct convert_to_t {
  template<class U>
  T operator()(U&& u) const {
    return T(std::forward<U>(u));
  }
};
template<typename T>
constexpr convert_to_t<T> convert_to;


template<typename Container, typename Key>
auto value_or(const Container& container, const Key& key, const typename Container::mapped_type& default_value) {
  const auto it = container.find(key);
  return (it == container.end()) ? default_value : it->second;
}

template<typename Container, typename Key>
auto value_or(const Container& container, const Key& key) {
  return value_or(container, key, {});
}

template<typename Container, typename Key>
auto extract_value_or(Container& container, const Key& key, const typename Container::mapped_type& default_value) {
  auto it = container.find(key);
  if (it == container.end()) {
    return default_value;
  } else {
    auto ret = std::move(it->second);
    container.erase(it);
    return ret;
  }
}

template<typename Container, typename Key>
auto extract_value_or(Container& container, const Key& key) {
  return extract_value_or(container, key, {});
}

template<typename Container>
auto extract_back(Container& container) {
  CHECK(!container.empty());
  auto value = std::move(container.back());
  container.pop_back();
  return value;
}


namespace internal {
template <typename T>
class HasIterator {
private:
  typedef char YesType[1];
  typedef char NoType[2];
  template <typename U> static YesType& test(const typename U::iterator*);
  template <typename U> static NoType& test(...);
public:
  enum { value = sizeof(test<T>(nullptr)) == sizeof(YesType) };
};
}  // namespace internal

// Add `enable_if` to remove collision with `concat` from pybind11/detail/descr.h
template<typename Container,
    typename = std::enable_if_t<internal::HasIterator<Container>::value>>
Container concat(Container a, Container b) {
  a.insert(a.end(), std::move_iterator(b.begin()), std::move_iterator(b.end()));
  return a;
}

template<typename Container, typename... ContainerTail,
    typename = std::enable_if_t<(std::is_same_v<Container, ContainerTail> && ...)>>
Container concat(Container head, ContainerTail... tail) {
  return concat(head, concat(tail...));
}

template<typename SuperContainer>
auto flatten(const SuperContainer& containers) {
  typename SuperContainer::value_type ret;
  for (const auto& c: containers) {
    ret.insert(ret.end(), c.begin(), c.end());
  }
  return ret;
}

template<typename T>
std::vector<T> slice(const std::vector<T>& v, int from, int to = -1) {
  if (to == -1) {
    to = v.size();
  }
  CHECK(0 <= from && from <= to && to <= v.size())
      << "Cannot slice vector of length " << v.size() << " from " << from << " to " << to;
  return std::vector<T>(v.begin() + from, v.begin() + to);
}

template<typename T>
std::vector<T> slice_incl(const std::vector<T>& v, int from, int to) {
  return slice(v, from, to + 1);
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
std::vector<T> removed_indices_one_based(std::vector<T> v, std::vector<int> indices) {
  absl::c_sort(indices);
  // Optimization potential: O(N) solution.
  for (auto it = indices.rbegin(); it != indices.rend(); ++it) {
    v.erase(v.begin() + *it - 1);
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

template<typename T>
std::vector<T> choose_indices_one_based(const std::vector<T>& v, const std::vector<int>& indices) {
  std::vector<T> ret;
  ret.reserve(indices.size());
  for (int idx : indices) {
    ret.push_back(v.at(idx - 1));
  }
  return ret;
}

template<typename T, size_t N, size_t M>
std::array<T, M> choose_indices(const std::array<T, N>& v, const std::array<int, M>& indices) {
  std::array<T, M> ret;
  for (int i : range(indices.size())) {
    ret[i] = v.at(indices[i]);
  }
  return ret;
}

template<typename T, size_t N, size_t M>
std::array<T, M> choose_indices_one_based(const std::array<T, N>& v, const std::array<int, M>& indices) {
  std::array<T, M> ret;
  for (int i : range(indices.size())) {
    ret[i] = v.at(indices[i] - 1);
  }
  return ret;
}

template<typename T, typename MaskT>
std::vector<T> choose_by_mask(const std::vector<T>& v, const std::vector<MaskT>& mask) {
  CHECK_EQ(v.size(), mask.size());
  std::vector<T> ret;
  for (int i : range(v.size())) {
    if (mask[i]) {
      ret.push_back(v[i]);
    }
  }
  return ret;
}

template<typename T>
std::pair<std::vector<T>, std::vector<T>> split_slice(const std::vector<T>& v, int pos) {
  return {slice(v, 0, pos), slice(v, pos)};
}

template<typename T>
std::pair<std::vector<T>, std::vector<T>> split_indices(const std::vector<T>& v, const std::vector<int>& indices) {
  return {choose_indices(v, indices), removed_indices(v, indices)};
}

template<typename T, size_t N>
std::array<T, N> permute(const std::array<T, N>& v, const std::array<int, N>& indices) {
  return choose_indices(v, indices);
}

// Optimization potential: Replace instances of `append_vector(out, mapped(in))`
//   with `absl::transform(in, std::back_inserter(out))`.
// Optimization potential: Split into versions with `src` passed by l-value reference
//   vs r-value reference.
template<typename T, typename SrcContainerT>
void append_vector(std::vector<T>& dst, SrcContainerT src) {
  absl::c_move(src, std::back_inserter(dst));
}

template<typename Src, typename F>
auto mapped(const Src& src, F&& func) {
  std::vector<std::decay_t<std::invoke_result_t<F, typename Src::value_type>>> dst;
  dst.reserve(src.size());
  absl::c_transform(src, std::back_inserter(dst), std::forward<F>(func));
  return dst;
}

// A version of `mapped` that allows `func` to change source values.
// This is NOT in-place map; the result is returned as usual.
template<typename Src, typename F>
auto mapped_mutable(Src& src, F&& func) {
  std::vector<std::decay_t<std::invoke_result_t<F, typename Src::reference>>> dst;
  dst.reserve(src.size());
  std::transform(src.begin(), src.end(), std::back_inserter(dst), std::forward<F>(func));
  return dst;
}

// Similar to
//   view::zip(view::iota(0), src) | views::transform(func) | <convert-to-vector>
// (note: `zip` is from https://github.com/ericniebler/range-v3, it's missing in C++20).
template<typename Src, typename F>
auto mapped_with_index(const Src& src, const F& func) {
  std::vector<std::decay_t<std::invoke_result_t<F, int, typename Src::value_type>>> dst;
  dst.reserve(src.size());
  for (const int idx : range(src.size())) {
    dst.push_back(func(idx, src[idx]));
  }
  return dst;
}

template<typename In, size_t N, typename F>
auto mapped_array(const std::array<In, N>& src, F&& func) {
  std::array<std::decay_t<std::invoke_result_t<F, In>>, N> dst;
  absl::c_transform(src, dst.begin(), func);
  return dst;
}

// Acts as `mapped(src, &to_string)`; the latter is forbidden because `to_string` is overloaded.
template<typename Src>
auto mapped_to_string(const Src& src) {
  return mapped(src, DISAMBIGUATE(to_string));
}

// Equivalent to `flatten(mapped(src, func))`.
template<typename Src, typename F>
auto mapped_expanding(const Src& src, const F& func) {
  using ResultContainer = std::decay_t<std::invoke_result_t<F, typename Src::value_type>>;
  std::vector<typename ResultContainer::value_type> dst;
  for (const auto& value : src) {
    append_vector(dst, func(value));
  }
  return dst;
}

// Transforms:  vector<vector<...vector<x>...>>  =>  vector<vector<...vector<f(x)>...>>
template<size_t Nesting, typename Src, typename F>
auto mapped_nested(const Src& src, const F& func) {
  static_assert(Nesting >= 1);
  if constexpr (Nesting == 1) {
    return mapped(src, func);
  } else {
    return mapped(src, [&](const auto& v) {
      return mapped_nested<Nesting-1>(v, func);
    });
  }
}

// Given a vector of values x and function that maps x => std::option(y),
// returns a vector of non-null y.
template<typename Src, typename F>
auto filtered_mapped(const Src& src, const F& func) {
  std::vector<std::decay_t<typename std::invoke_result_t<F, typename Src::value_type>::value_type>> dst;
  for (const auto& old_value : src) {
    auto new_value = func(old_value);
    if (new_value.has_value()) {
      dst.push_back(std::move(new_value).value());
    }
  }
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

template<typename T, typename F>
std::vector<std::vector<T>> group_by(const std::vector<T>& src, const F& func) {
  std::vector<std::vector<T>> ret;
  std::vector<T> group;
  for (const T& element : src) {
    if (!group.empty() && !func(group.back(), element)) {
      ret.push_back(std::move(group));
      group.clear();
    }
    group.push_back(element);
  }
  if (!group.empty()) {
    ret.push_back(std::move(group));
  }
  return ret;
}

template<typename T>
std::vector<std::vector<T>> group_equal(const std::vector<T>& src) {
  return group_by(src, [](const T& lhs, const T& rhs) { return lhs == rhs; });
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
  CHECK(it != c.end()) << dump_to_string(c) << " does not contain " << value;
  return it - c.begin();
}

template<typename T>
std::vector<T> rotated_vector(std::vector<T> v, int n) {
  n = pos_mod(n, v.size());
  absl::c_rotate(v, v.begin() + n);
  return v;
}

template<typename Container>
Container sorted(Container c) {
  absl::c_sort(c);
  return c;
}

template <typename Container, typename Compare>
Container sorted(Container c, Compare&& cmp) {
  absl::c_sort(c, std::forward<Compare>(cmp));
  return c;
}

template<typename Container>
Container reversed(Container c) {
  absl::c_reverse(c);
  return c;
}

template<typename T>
std::vector<std::vector<T>> transposed(const std::vector<std::vector<T>>& v) {
  if (v.empty()) {
    return {};
  }
  const int n = v[0].size();
  std::vector<std::vector<T>> ret(n);
  for (const auto& row : v) {
    CHECK_EQ(row.size(), n);
    for (const int i : range(n)) {
      ret[i].push_back(row[i]);
    }
  }
  return ret;
}

template<typename T>
void keep_unique_sorted(std::vector<T>& v) {
  v.erase(std::unique(v.begin(), v.end()), v.end());
}

template<typename Container>
int num_distinct_elements_sorted(Container v) {
  absl::c_sort(v);
  return std::unique(v.begin(), v.end()) - v.begin();
}
template<typename Container>
int num_distinct_elements_unsorted(Container v) {
  absl::c_sort(v);
  return num_distinct_elements_sorted(v);
}

template<typename Container>
bool all_unique_sorted(const Container& v) {
  return absl::c_adjacent_find(v) == v.end();
}
template<typename Container, typename Compare>
bool all_unique_unsorted(Container v, Compare&& cmp) {
  absl::c_sort(v, std::forward<Compare>(cmp));
  return all_unique_sorted(v);
}
template<typename Container>
bool all_unique_unsorted(Container v) {
  return all_unique_unsorted(std::move(v), std::less<>());
}

template<size_t N, typename T>
std::array<T, N> to_array(std::vector<T> v) {
  CHECK_EQ(v.size(), N) << dump_to_string(v);
  std::array<T, N> ret;
  absl::c_move(v, ret.begin());
  return ret;
}

template<typename Container>
auto to_vector(const Container& c) {
  // Note. Don't use `std::vector(c.begin(), c.end())` because it doesn't support heterogeneous
  // iterators (used by `Permutations` for example).
  std::vector<typename Container::value_type> ret;
  using IteratorCategory = typename std::iterator_traits<decltype(c.begin())>::iterator_category;
  if constexpr (std::is_same_v<IteratorCategory, std::random_access_iterator_tag>) {
    ret.reserve(c.end() - c.begin());
  }
  for (const auto& v : c) {
    ret.push_back(v);
  }
  return ret;
}

// Specializations for map types remove `const` from key
// TODO: Consider auto detection (https://stackoverflow.com/a/35293958/3092679) here and in dump_to_string.
template<typename Key, typename T, typename Compare>
auto to_vector(const std::map<Key, T, Compare>& c) {
  return std::vector<typename std::pair<Key, T>>(c.begin(), c.end());
}
template<typename Key, typename T, typename Hash, typename Eq>
auto to_vector(const absl::flat_hash_map<Key, T, Hash, Eq>& c) {
  return std::vector<typename std::pair<Key, T>>(c.begin(), c.end());
}

template<typename DstElement, typename Container>
auto to_vector(const Container& c) {
  return std::vector<DstElement>(c.begin(), c.end());
}

template<typename Container>
auto to_set(const Container& c) {
  return absl::flat_hash_set<typename Container::value_type>(c.begin(), c.end());
}

template<typename Container>
bool all_equal(const Container& c) {
  return absl::c_adjacent_find(c, std::not_equal_to<>()) == c.end();
}
// Equivalent to all_equal(mapped(c, projector)), but doesn't construct the intermediate vector.
template<typename Container, typename Projector>
bool all_equal(const Container& c, const Projector& projector) {
  if (c.size() <= 1) {
    return true;
  }
  auto prev = projector(c[0]);
  for (size_t i : range(1, c.size())) {
    auto current = projector(c[i]);
    if (current != prev) {
      return false;
    }
    prev = std::move(current);
  }
  return true;
}

template<typename Container>
bool is_strictly_increasing(const Container& c) {
  return absl::c_adjacent_find(c, std::greater_equal<>()) == c.end();
}
template<typename Container>
bool is_non_decreasing(const Container& c) {
  return absl::c_adjacent_find(c, std::greater<>()) == c.end();
}
template<typename Container>
bool is_strictly_decreasing(const Container& c) {
  return absl::c_adjacent_find(c, std::less_equal<>()) == c.end();
}
template<typename Container>
bool is_non_increasing(const Container& c) {
  return absl::c_adjacent_find(c, std::less<>()) == c.end();
}

template<typename Container>
auto sum(const Container& c) {
  return absl::c_accumulate(c, typename Container::value_type());
}
template<typename Container, typename Projector>
auto sum(const Container& c, const Projector& projector) {
  return absl::c_accumulate(
    c,
    std::decay_t<std::invoke_result_t<Projector, typename Container::value_type>>(),
    [&](const auto& x, const auto& y) {
      return x + projector(y);
    }
  );
}

template<typename Container>
auto diffs(const Container& c) {
  return mapped(range(c.size() - 1), [&](const int i) { return c[i + 1] - c[i]; });
}

template<typename Container>
auto min_value(const Container& c) {
  return *absl::c_min_element(c);
}
template<typename Container>
auto max_value(const Container& c) {
  return *absl::c_max_element(c);
}
template<typename Container>
auto minmax_value(const Container& c) {
  const auto minmax_it = absl::c_minmax_element(c);
  return std::pair{*minmax_it.first, *minmax_it.second};
}

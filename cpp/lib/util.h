#pragma once

#include "absl/algorithm/container.h"
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

inline int div_int(int a, int b) {
  CHECK_EQ(a % b, 0);
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


inline std::vector<int> seq_incl(int from, int to) {
  ++to;
  std::vector<int> ret(to - from);
  absl::c_iota(ret, from);
  return ret;
}

template<typename T>
std::vector<T> concat(std::vector<T> a, std::vector<T> b) {
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
std::vector<T> choose_indices(const std::vector<T>& v, const std::vector<int> indices) {
  std::vector<T> ret;
  ret.reserve(indices.size());
  for (int idx : indices) {
    ret.push_back(v[idx]);
  }
  return ret;
}

template<typename In, typename F>
auto mapped(const absl::Span<const In>& src, F&& func) {
  std::vector<std::invoke_result_t<F, In>> dst(src.size());
  absl::c_transform(src, dst.begin(), func);
  return dst;
}

template<typename In, typename F>
auto mapped(const std::vector<In>& src, F&& func) {
  return mapped(absl::MakeConstSpan(src), std::forward<F>(func));
}

template<typename T>
void rotate_vector(std::vector<T>& v, int n) {
  n = pos_mod(n, v.size());
  absl::c_rotate(v, v.begin() + n);
}

template<typename T>
void append_vector(std::vector<T>& dst, const absl::Span<const T>& src) {
  const size_t old_size = dst.size();
  dst.resize(old_size + src.size());
  absl::c_move(src, dst.begin() + old_size);
}

template<typename T>
void append_vector(std::vector<T>& dst, const std::vector<T>& src) {
  append_vector(dst, absl::MakeConstSpan(src));
}

template<typename First, typename Second>
std::vector<std::pair<First, Second>> zip(
    const std::vector<First>& first,
    const std::vector<Second>& second) {
  CHECK_EQ(first.size(), second.size());
  std::vector<std::pair<First, Second>> ret;
  for (int i = 0; i < first.size(); ++i) {
    ret.push_back({first[i], second[i]});
  }
  return ret;
}

template<typename First, typename Second, typename F>
auto mapped_zip(
    const std::vector<First>& first,
    const std::vector<Second>& second,
    F&& func) {
  CHECK_EQ(first.size(), second.size());
  std::vector<std::invoke_result_t<F, First, Second>> ret;
  for (int i = 0; i < first.size(); ++i) {
    ret.push_back(func(first[i], second[i]));
  }
  return ret;
}

template<typename First, typename Second>
std::pair<std::vector<First>, std::vector<Second>> unzip(
    const std::vector<std::pair<First, Second>>& both) {
  std::vector<First> first;
  std::vector<Second> second;
  for (const auto& element : both) {
    first.push_back(element.first);
    second.push_back(element.second);
  }
  return {std::move(first), std::move(second)};
}

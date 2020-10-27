#pragma once

#include <vector>


// TODO: unit tests
// Returns quasi shuffle product of two words as a List of words.
// Rules:  // TODO: Check !!!
//   1 🟉 v = v
//   u 🟉 1 = u
//   ua 🟉 vb = (u 🟉 vb)a + (ua 🟉 v)b - (u 🟉 v)(a ⋄ b)
// Optimization potential: return Linear.
// Optimization potential: pass Spans internally.
template<typename T, typename F>
std::vector<std::vector<T>> quasi_shuffle_product(
    const std::vector<T>& u,
    const std::vector<T>& v,
    F glue_func) {
  if (u.empty() && v.empty()) {
    return {};
  }
  if (u.empty()) {
    return {v};
  }
  if (v.empty()) {
    return {u};
  }
  const T& a = u.back();
  const T& b = v.back();
  std::vector<T> u_trunc = u;
  std::vector<T> v_trunc = v;
  u_trunc.pop_back();
  v_trunc.pop_back();
  std::vector<std::vector<T>> ret;
  for (auto term : quasi_shuffle_product(u, v_trunc, glue_func)) {
    term.push_back(b);
    ret.push_back(term);
  }
  for (auto term : quasi_shuffle_product(u_trunc, v, glue_func)) {
    term.push_back(a);
    ret.push_back(term);
  }
  for (auto term : quasi_shuffle_product(u_trunc, v_trunc, glue_func)) {
    term.push_back(glue_func(a, b));
    ret.push_back(term);
  }
  return ret;
}

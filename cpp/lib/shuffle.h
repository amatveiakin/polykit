#pragma once

#include <vector>

#include "util.h"
#include "word.h"


// Returns shuffle product of two words as a List of words.
// Rules:
//   1 ⧢ v = v
//   u ⧢ 1 = u
//   ua ⧢ vb = (u ⧢ vb)a + (ua ⧢ v)b
// Optimization potential: Unroll for short words.
// Optimization potential: Cache results.
std::vector<IntWord> shuffle_product(const IntWord& u, const IntWord& v) {
  if (u.empty() && v.empty()) {
    return {};
  }
  if (u.empty()) {
    return {std::move(v)};
  }
  if (v.empty()) {
    return {std::move(u)};
  }
  IntWord u_trunc = u;
  IntWord v_trunc = v;
  const int a = u_trunc.pop_back();
  const int b = v_trunc.pop_back();
  std::vector<IntWord> ret;
  for (IntWord w : shuffle_product(u, v_trunc)) {
    w.push_back(b);
    ret.push_back(w);
  }
  for (IntWord w : shuffle_product(u_trunc, v)) {
    w.push_back(a);
    ret.push_back(w);
  }
  return ret;
}

// Returns  w1 ⧢ w2 ⧢ ... ⧢ wn
// Optimization potential: Do de-duping in the process.
std::vector<IntWord> shuffle_product(std::vector<IntWord> words) {
  if (words.size() == 0) {
    return {};
  } else if (words.size() == 1) {
    return {words[0]};
  } else {
    const IntWord& tail = words.back();
    words.pop_back();
    std::vector<IntWord> shuffle_product_head = shuffle_product(words);
    std::vector<IntWord> ret;
    for (const IntWord& w_head : shuffle_product_head) {
      append_vector(ret, shuffle_product(w_head, tail));
    }
    return ret;
  }
}

#include "shuffle.h"

#include "algebra.h"
#include "shuffle_unrolled.h"


IntWordExpr shuffle_product(const IntWord& u, const IntWord& v) {
  if (u.empty() && v.empty()) {
    return {};
  }
  if (u.empty()) {
    return IntWordExpr::single_key(v);
  }
  if (v.empty()) {
    return IntWordExpr::single_key(u);
  }
  {
    IntWordExpr unrolled = shuffle_power_unrolled(u, v);
    if (!unrolled.zero()) {
      return unrolled;
    }
  }
  IntWord u_trunc = u;
  IntWord v_trunc = v;
  const int a = u_trunc.pop_back();
  const int b = v_trunc.pop_back();
  IntWordExpr ret;
  shuffle_product(u, v_trunc).foreach_key([&](IntWord w, int coeff) {
    w.push_back(b);
    ret.add_to(w, coeff);
  });
  shuffle_product(u_trunc, v).foreach_key([&](IntWord w, int coeff) {
    w.push_back(a);
    ret.add_to(w, coeff);
  });
  return ret;
}

IntWordExpr shuffle_product(std::vector<IntWord> words) {
  if (words.size() == 0) {
    return {};
  } else if (words.size() == 1) {
    return IntWordExpr::single_key(words[0]);
  } else if (words.size() == 2) {
    return shuffle_product(words[0], words[1]);
  } else {
    const IntWord& w_tail = words.back();
    words.pop_back();
    const IntWordExpr shuffle_product_head = shuffle_product(words);
    IntWordExpr ret;
    shuffle_product_head.foreach_key([&](IntWord w_head, int coeff) {
      ret += coeff * shuffle_product(w_head, w_tail);
    });
    return ret;
  }
}

// Experiment with shuffle power unrolling - didn't speed up anything
//
// IntWordExpr shuffle_product(IntWordExpr expr) {
//   if (expr.zero()) {
//     return {};
//   }
//   std::vector<IntWordExpr> power_expressions;
//   expr.foreach_key([&](const IntWord& word, int coeff) {
//     CHECK_GT(coeff, 0);
//     if (coeff == 1) {
//       power_expressions.push_back(IntWordExpr::single(word));
//     } else {
//       power_expressions.push_back(shuffle_power_unrolled(word, coeff));
//     }
//   });
//   return outer_product_expanding(
//       absl::MakeConstSpan(power_expressions),
//       static_cast<IntWordExpr (*)(const IntWord&, const IntWord&)>(&shuffle_product));
// }

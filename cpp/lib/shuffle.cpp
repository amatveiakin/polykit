#include "shuffle.h"

#include "algebra.h"
#include "shuffle_unrolled.h"


WordExpr shuffle_product(const Word& u, const Word& v) {
  if (u.empty() && v.empty()) {
    return {};
  }
  if (u.empty()) {
    return WordExpr::single_key(v);
  }
  if (v.empty()) {
    return WordExpr::single_key(u);
  }
  {
    WordExpr unrolled = shuffle_product_unrolled(u, v);
    if (!unrolled.zero()) {
      return unrolled;
    }
  }
  Word u_trunc = u;
  Word v_trunc = v;
  const int a = u_trunc.pop_back();
  const int b = v_trunc.pop_back();
  WordExpr ret;
  shuffle_product(u, v_trunc).foreach_key([&](Word w, int coeff) {
    w.push_back(b);
    ret.add_to(w, coeff);
  });
  shuffle_product(u_trunc, v).foreach_key([&](Word w, int coeff) {
    w.push_back(a);
    ret.add_to(w, coeff);
  });
  return ret;
}

WordExpr shuffle_product(std::vector<Word> words) {
  if (words.size() == 0) {
    return {};
  } else if (words.size() == 1) {
    return WordExpr::single_key(words[0]);
  } else if (words.size() == 2) {
    return shuffle_product(words[0], words[1]);
  } else {
    const Word& w_tail = words.back();
    words.pop_back();
    const WordExpr shuffle_product_head = shuffle_product(words);
    WordExpr ret;
    shuffle_product_head.foreach_key([&](Word w_head, int coeff) {
      ret += coeff * shuffle_product(w_head, w_tail);
    });
    return ret;
  }
}

// Experiment with shuffle power unrolling - didn't speed up anything
//
// WordExpr shuffle_product(WordExpr expr) {
//   if (expr.zero()) {
//     return {};
//   }
//   std::vector<WordExpr> power_expressions;
//   expr.foreach_key([&](const Word& word, int coeff) {
//     CHECK_GT(coeff, 0);
//     if (coeff == 1) {
//       power_expressions.push_back(WordExpr::single(word));
//     } else {
//       power_expressions.push_back(shuffle_power_unrolled(word, coeff));
//     }
//   });
//   return outer_product_expanding(
//       absl::MakeConstSpan(power_expressions),
//       static_cast<WordExpr (*)(const Word&, const Word&)>(&shuffle_product));
// }

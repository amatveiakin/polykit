#include "word.h"


WordExpr word_expr_substitute(
    const WordExpr& expr,
    const std::vector<int>& new_points) {
  return expr.mapped<WordExpr>([&](const Word& w) {
    Word ret;
    for (unsigned char ch : w) {
      ret.push_back(new_points.at(ch - 1));
    }
    return ret;
  }).without_annotations();
}

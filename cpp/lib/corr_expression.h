// TODO: Organize this to be more in line with polylog formal symbol expressions.

#pragma once

#include "format.h"
#include "lexicographical.h"
#include "linear.h"
#include "multiword.h"
#include "util.h"
#include "word.h"


struct CorrFSymb {
  CorrFSymb() {}
  CorrFSymb(std::vector<int> points_arg)
    : points(std::move(lexicographically_minimal_rotation(points_arg))) {}

  std::vector<int> points;

  bool operator==(const CorrFSymb& other) const { return points == other.points; }
  bool operator< (const CorrFSymb& other) const { return points <  other.points; }
};

inline std::string to_string(const CorrFSymb& fsymb) {
  return fmt::function("Corr", mapped(
    fsymb.points,
    [](auto x){ return to_string(x); })
  );
}

struct CorrExprParam {
  using ObjectT = CorrFSymb;
  using StorageT = Word;
  static StorageT object_to_key(const ObjectT& obj) {
    return Word(obj.points.begin(), obj.points.end());
  }
  static ObjectT key_to_object(const StorageT& key) {
    return CorrFSymb{mapped(key.span(), [](int ch){ return ch; })};
  }
  static std::string object_to_string(const ObjectT& obj) {
    return to_string(obj);
  }
};

using CorrExpr = Linear<CorrExprParam>;

CorrExpr corr_expr_substitute(const CorrExpr& expr, const std::vector<int>& new_points);


constexpr int kCorrCoExprComponents = 2;

using CoCorrFSymb = std::array<CorrFSymb, kCorrCoExprComponents>;

inline std::string to_string(const CoCorrFSymb& fsymb) {
  return str_join(fsymb, fmt::coprod_lie());
}

struct CorrCoExprParam {
  using ObjectT = CoCorrFSymb;
  using StorageT = MultiWord;
  static StorageT object_to_key(const ObjectT& obj) {
    MultiWord ret;
    for (const auto& component : obj) {
      ret.append_segment(CorrExprParam::object_to_key(component));
    }
    return ret;
  }
  static ObjectT key_to_object(const StorageT& key) {
    CHECK_EQ(key.num_segments(), kCorrCoExprComponents);
    std::array<CorrFSymb, kCorrCoExprComponents> ret;
    for (int i = 0; i < kCorrCoExprComponents; ++i) {
      ret[i] = CorrExprParam::key_to_object(Word{key.segment(i)});
    }
    return ret;
  }
  static std::string object_to_string(const ObjectT& obj) {
    return to_string(obj);
  }
  static constexpr bool coproduct_is_lie_algebra = true;
};


using CorrCoExpr = Linear<CorrCoExprParam>;

CorrCoExpr corr_coproduct(const CorrExpr& lhs, const CorrExpr& rhs);
CorrCoExpr corr_comultiply(const CorrExpr& expr, std::pair<int, int> form);

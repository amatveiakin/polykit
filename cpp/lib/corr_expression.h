// TODO: Organize this to be more in line with polylog formal symbol expressions.

#pragma once

#include "format.h"
#include "lexicographical.h"
#include "linear.h"
#include "multiword.h"
#include "packed.h"
#include "util.h"
#include "word.h"


struct CorrFSymb {
  CorrFSymb() {}
  explicit CorrFSymb(std::vector<int> points_arg)
    : points(lexicographically_minimal_rotation(points_arg)) {}

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

#define OLD_CORR_EXPR 0
#if OLD_CORR_EXPR  // DEPRECATED[word-to-pvector]
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
#else
struct CorrExprParam {
  using ObjectT = CorrFSymb;
  using StorageT = PVector<unsigned char, 10>;
  static StorageT object_to_key(const ObjectT& obj) {
    return to_pvector<StorageT>(obj.points);
  }
  static ObjectT key_to_object(const StorageT& key) {
    return CorrFSymb(to_vector<int>(key));
  }
  static std::string object_to_string(const ObjectT& obj) {
    return to_string(obj);
  }
};
#endif

using CorrExpr = Linear<CorrExprParam>;

CorrExpr corr_expr_substitute(const CorrExpr& expr, const std::vector<int>& new_points);


constexpr int kCorrCoExprComponents = 2;

using CoCorrFSymb = std::array<CorrFSymb, kCorrCoExprComponents>;

inline std::string to_string(const CoCorrFSymb& fsymb) {
  return str_join(fsymb, fmt::coprod_lie());
}

struct CorrCoExprParam {
  using ObjectT = CoCorrFSymb;
  using StorageT = std::array<CorrExprParam::StorageT, kCorrCoExprComponents>;
  static StorageT object_to_key(const ObjectT& obj) {
    return mapped_array(obj, CorrExprParam::object_to_key);
  }
  static ObjectT key_to_object(const StorageT& key) {
    return mapped_array(key, CorrExprParam::key_to_object);
  }
  static std::string object_to_string(const ObjectT& obj) {
    return to_string(obj);
  }
  static constexpr bool coproduct_is_lie_algebra = true;
};


using CorrCoExpr = Linear<CorrCoExprParam>;

CorrCoExpr corr_coproduct(const CorrExpr& lhs, const CorrExpr& rhs);
CorrCoExpr corr_comultiply(const CorrExpr& expr, std::pair<int, int> form);

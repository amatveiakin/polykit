#pragma once

#include "format.h"
#include "lexicographical.h"
#include "linear.h"
#include "packed.h"
#include "util.h"


struct CorrFSymb {
  CorrFSymb() {}
  explicit CorrFSymb(std::vector<int> points_arg)
    : points(lexicographically_minimal_rotation(points_arg)) {}

  std::vector<int> points;

  bool operator==(const CorrFSymb& other) const { return points == other.points; }
  bool operator< (const CorrFSymb& other) const { return points <  other.points; }
};

inline std::string to_string(const CorrFSymb& fsymb) {
  return fmt::function_num_args(fmt::opname("Corr"), fsymb.points);
}

namespace internal {
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
}  // namespace internal

using CorrExpr = Linear<internal::CorrExprParam>;

CorrExpr corr_expr_substitute(const CorrExpr& expr, const std::vector<int>& new_points);


constexpr int kCorrCoExprComponents = 2;

using CoCorrFSymb = std::array<CorrFSymb, kCorrCoExprComponents>;

inline std::string to_string(const CoCorrFSymb& fsymb) {
  return str_join(fsymb, fmt::coprod_lie());
}

namespace internal {
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
}  // namespace internal

using CorrCoExpr = Linear<internal::CorrCoExprParam>;

CorrCoExpr corr_coproduct(const CorrExpr& lhs, const CorrExpr& rhs);
CorrCoExpr corr_comultiply(const CorrExpr& expr, std::pair<int, int> form);

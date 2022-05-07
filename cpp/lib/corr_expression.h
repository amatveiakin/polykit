#pragma once

#include "format.h"
#include "linear.h"
#include "pvector.h"
#include "util.h"


#define HAS_CORR_EXPR 1

struct CorrFSymb {
  CorrFSymb() {}
  explicit CorrFSymb(std::vector<int> points_arg);

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

CorrExpr substitute_variables(const CorrExpr& expr, const std::vector<int>& new_points);


constexpr int kCorrCoExprParts = 2;

using CoCorrFSymb = std::vector<CorrFSymb>;

inline std::string to_string(const CoCorrFSymb& fsymb) {
  return str_join(fsymb, fmt::coprod_iterated());
}

namespace internal {
struct CorrCoExprParam {
  using ObjectT = CoCorrFSymb;
  using StorageT = PVector<CorrExprParam::StorageT, kCorrCoExprParts>;
  IDENTITY_VECTOR_FORM
  LYNDON_COMPARE_LENGTH_FIRST
  static StorageT object_to_key(const ObjectT& obj) {
    return mapped_to_pvector<StorageT>(obj, CorrExprParam::object_to_key);
  }
  static ObjectT key_to_object(const StorageT& key) {
    return mapped(key, CorrExprParam::key_to_object);
  }
  static std::string object_to_string(const ObjectT& obj) {
    return to_string(obj);
  }
  static constexpr bool coproduct_is_lie_algebra = true;
  static constexpr bool coproduct_is_iterated = true;
};
}  // namespace internal

using CorrCoExpr = Linear<internal::CorrCoExprParam>;

CorrCoExpr corr_coproduct(const CorrExpr& lhs, const CorrExpr& rhs);
CorrCoExpr corr_comultiply(const CorrExpr& expr, std::pair<int, int> form);

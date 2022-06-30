#include "gamma.h"


// Computes Grassmannian polylogarithm of dimension n, weight n-1 on 2n points.
GammaExpr CGrLiVec(int weight, const std::vector<int>& points);

// Equivalent to `pullback(CGrLiVec(weight, li_points), pb_points)`
GammaExpr CGrLiVec(int weight, const std::vector<int>& pb_points, const std::vector<int>& li_points);

GammaExpr SymmCGrLi3(const std::vector<int>& points);
// Only partially symmetrized.
// TODO: Finish symmetrizing.
GammaExpr SymmCGrLi4_wip(const std::vector<int>& points);

bool are_CGrLi_args_ok(int weight, int num_points);

// When constructing a set of CGrLi of all possible point orders, it is sufficient to include
// only canonical orders. Each non-canonical order is equivalent to a canonical one.
bool is_canonical_CGrLi_arg_order(const std::vector<int>& points);


namespace internal {
struct CGrLiFixedWeightAndPbPoints {
  CGrLiFixedWeightAndPbPoints(int weight, std::vector<int> pb_points)
    : weight_(weight), pb_points_(std::move(pb_points)) {}

  template<typename... Args>
  GammaExpr operator()(Args... args) {
    return CGrLiVec(weight_, pb_points_, {args...});
  }
private:
  int weight_ = 0;
  std::vector<int> pb_points_;
};

struct CGrLiFixedWeight {
  CGrLiFixedWeight(int weight) : weight_(weight) {}

  template<typename... Args>
  GammaExpr operator()(Args... args) {
    return CGrLiVec(weight_, {args...});
  }

  template<typename... Args>
  auto operator[](std::vector<int> args) {
    return CGrLiFixedWeightAndPbPoints(weight_, std::move(args));
  }
private:
  int weight_ = 0;
};
}  // namespace internal

// TODO: Less clumsy pullback points notations
// TODO: Synchronize pullback points notation with GrLog and GrQLi
inline static auto CGrLi1 = internal::CGrLiFixedWeight(1);
inline static auto CGrLi2 = internal::CGrLiFixedWeight(2);
inline static auto CGrLi3 = internal::CGrLiFixedWeight(3);
inline static auto CGrLi4 = internal::CGrLiFixedWeight(4);
inline static auto CGrLi5 = internal::CGrLiFixedWeight(5);
inline static auto CGrLi6 = internal::CGrLiFixedWeight(6);
inline static auto CGrLi7 = internal::CGrLiFixedWeight(7);
inline static auto CGrLi8 = internal::CGrLiFixedWeight(8);

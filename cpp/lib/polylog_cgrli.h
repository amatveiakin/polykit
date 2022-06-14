#include "gamma.h"


// Computes Grassmannian polylogarithm of dimension n, weight n-1 on 2n points.
GammaExpr CGrLiVec(int weight, const std::vector<int>& points);

GammaExpr SymmCGrLi3(const std::vector<int>& points);
// Only partially symmetrized.
// TODO: Finish symmetrizing.
GammaExpr SymmCGrLi4_wip(const std::vector<int>& points);

bool are_CGrLi_args_ok(int weight, int num_points);


namespace internal {
template<typename... Args>
GammaExpr CGrLi_dispatch(int weight, Args... args) {
  return CGrLiVec(weight, {args...});
}
}  // namespace internal

template<typename... Args> GammaExpr CGrLi1(Args... args) { return internal::CGrLi_dispatch(1, args...); }
template<typename... Args> GammaExpr CGrLi2(Args... args) { return internal::CGrLi_dispatch(2, args...); }
template<typename... Args> GammaExpr CGrLi3(Args... args) { return internal::CGrLi_dispatch(3, args...); }
template<typename... Args> GammaExpr CGrLi4(Args... args) { return internal::CGrLi_dispatch(4, args...); }
template<typename... Args> GammaExpr CGrLi5(Args... args) { return internal::CGrLi_dispatch(5, args...); }
template<typename... Args> GammaExpr CGrLi6(Args... args) { return internal::CGrLi_dispatch(6, args...); }
template<typename... Args> GammaExpr CGrLi7(Args... args) { return internal::CGrLi_dispatch(7, args...); }
template<typename... Args> GammaExpr CGrLi8(Args... args) { return internal::CGrLi_dispatch(8, args...); }

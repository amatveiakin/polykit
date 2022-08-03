// Cluster Grassmannian polylogarithm. Given 2n points it has dimension n.
// The case of weight == n-1 gives Aomoto polylogarithm.

#include "gamma.h"


GammaExpr GLiVec(int weight, const std::vector<int>& points);

// Equivalent to `pullback(GLiVec(weight, li_points), pb_points)`
GammaExpr GLiVec(int weight, const std::vector<int>& pb_points, const std::vector<int>& li_points);

GammaExpr SymmGLi3(const std::vector<int>& points);
// Only partially symmetrized.
// TODO: Finish symmetrizing.
GammaExpr SymmGLi4_wip(const std::vector<int>& points);

bool are_GLi_args_ok(int weight, int num_points);

// When constructing a set of GLi of all possible point orders, it is sufficient to include
// only canonical orders. Each non-canonical order is equivalent to a canonical one.
bool is_canonical_GLi_arg_order(const std::vector<int>& points);


namespace internal {
struct GLiFixedWeightAndPbPoints {
  GLiFixedWeightAndPbPoints(int weight, std::vector<int> pb_points)
    : weight_(weight), pb_points_(std::move(pb_points)) {}

  template<typename... Args>
  GammaExpr operator()(Args... args) {
    return GLiVec(weight_, pb_points_, {args...});
  }
private:
  int weight_ = 0;
  std::vector<int> pb_points_;
};

struct GLiFixedWeight {
  GLiFixedWeight(int weight) : weight_(weight) {}

  template<typename... Args>
  GammaExpr operator()(Args... args) {
    return GLiVec(weight_, {args...});
  }

  template<typename... Args>
  auto operator[](std::vector<int> args) {
    return GLiFixedWeightAndPbPoints(weight_, std::move(args));
  }
private:
  int weight_ = 0;
};
}  // namespace internal

// TODO: Less clumsy pullback points notations
// TODO: Synchronize pullback points notation with GrLog and GrQLi
inline static auto GLi1 = internal::GLiFixedWeight(1);
inline static auto GLi2 = internal::GLiFixedWeight(2);
inline static auto GLi3 = internal::GLiFixedWeight(3);
inline static auto GLi4 = internal::GLiFixedWeight(4);
inline static auto GLi5 = internal::GLiFixedWeight(5);
inline static auto GLi6 = internal::GLiFixedWeight(6);
inline static auto GLi7 = internal::GLiFixedWeight(7);
inline static auto GLi8 = internal::GLiFixedWeight(8);

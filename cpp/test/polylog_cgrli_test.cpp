#include "lib/polylog_cgrli.h"

#include "gtest/gtest.h"

#include "lib/polylog_grqli.h"
#include "test_util/matchers.h"


GammaExpr CasimirDim3(const std::vector<int>& points) {
  const auto args = [&](const std::vector<int>& indices) {
    return choose_indices_one_based(points, indices);
  };
  CHECK_EQ(points.size(), 6);
  return
    + G(args({6,1,2}))
    + G(args({3,4,5}))
    - G(args({6,1,5}))
    - G(args({2,3,4}))
  ;
}

GammaExpr CGrLi_Dim3_alternative(int weight, const std::vector<int>& points) {
  CHECK_EQ(points.size(), 6);
  CHECK_LE(2, weight);
  const auto qli1 = [&](int bonus_arg, const std::vector<int>& main_args) {
    return GrQLiVec(1, choose_indices_one_based(points, {bonus_arg}), choose_indices_one_based(points, main_args));
  };
  const auto qlin = [&](int bonus_arg, const std::vector<int>& main_args) {
    return GrQLiVec(weight - 1, choose_indices_one_based(points, {bonus_arg}), choose_indices_one_based(points, main_args));
  };
  auto ret = (
    + tensor_product(qli1(6, {1,2,3,5}), qlin(3, {4,5,6,2}))
    - tensor_product(qli1(6, {1,2,4,5}), qlin(4, {3,5,6,2}))
    - tensor_product(qli1(1, {6,2,3,5}), qlin(3, {4,5,1,2}))
    + tensor_product(qli1(1, {6,2,4,5}), qlin(4, {3,5,1,2}))
  );
  if (weight > 2) {
    ret += tensor_product(CasimirDim3(points), CGrLi_Dim3_alternative(weight - 1, points));
  }
  return ret.without_annotations().annotate(
    fmt::function_num_args(
      fmt::sub_num(fmt::opname("CGrLi"), {weight}),
      points
    )
  );
}


#if 0  // TODO: Test that CGrLi is equal to these definitions.
GammaExpr CGrLi_Dim4_Weight3_alternative(const std::vector<int>& points) {
  CHECK_EQ(points.size(), 8);
  const auto f = [](const std::vector<int>& arguments) {
    CHECK_EQ(arguments.size(), 8);
    const auto args = [&](const std::vector<int>& indices) {
      return choose_indices_one_based(arguments, indices);
    };
    return tensor_product(absl::MakeConstSpan({
      GrQLiVec(1, args({2,3}), args({1,4,7,8})),
      GrQLiVec(1, args({3,7}), args({2,4,5,8})),
      GrQLiVec(1, args({5,7}), args({3,4,6,8})),
    }));
  };
  const auto f1 = [&](const std::vector<int>& arguments) {
    return sum_alternating(f, arguments, choose_indices_one_based(arguments, {1,2,3}));
  };
  return sum_alternating(f1, points, choose_indices_one_based(points, {5,6,7}));
}

GammaExpr CGrLi_Dim4_Weight4_alternative(const std::vector<int>& points) {
  CHECK_EQ(points.size(), 8);
  const auto f = [](const std::vector<int>& arguments) {
    CHECK_EQ(arguments.size(), 8);
    const auto args = [&](const std::vector<int>& bonus_points, const std::vector<int>& main_points) {
      return std::pair{choose_indices_one_based(arguments, bonus_points), choose_indices_one_based(arguments, main_points)};
    };
    const auto a = args({2,3}, {1,4,7,8});
    const auto b = args({3,7}, {2,4,5,8});
    const auto c = args({5,7}, {3,4,6,8});
    return
      + tensor_product(absl::MakeConstSpan({
        GrLogVec(a) + GrLogVec(b) + GrLogVec(c),
        GrQLiVec(1, a),
        GrQLiVec(1, b),
        GrQLiVec(1, c),
      }))
      + tensor_product(absl::MakeConstSpan({
        GrQLiVec(1, a),
        GrLogVec(b) + GrLogVec(c),
        GrQLiVec(1, b),
        GrQLiVec(1, c),
      }))
      + tensor_product(absl::MakeConstSpan({
        GrQLiVec(1, a),
        GrQLiVec(1, b),
        GrLogVec(c),
        GrQLiVec(1, c),
      }))
    ;
  };
  const auto f1 = [&](const std::vector<int>& arguments) {
    return sum_alternating(f, arguments, choose_indices_one_based(arguments, {1,2,3}));
  };
  return sum_alternating(f1, points, choose_indices_one_based(points, {5,6,7}));
}
#endif


TEST(CGrLiTest, IsTotallyWeaklySeparated) {
  // Should be true for any weight and dimension.
  for (const int dimension : range_incl(2, 4)) {
    for (const int weight : range_incl(3, 4)) {
      const auto points = to_vector(range_incl(1, 2 * dimension));
      EXPECT_TRUE(is_totally_weakly_separated(CGrLi(weight, points)));
    }
  }
}

TEST(CGrLiTest, EqualsAlternative_Dim3) {
  for (const int weight : range_incl(2, 5)) {
    // TODO: Sync indices
    EXPECT_EXPR_EQ(CGrLi(weight, {1,6,2,3,4,5}), CGrLi_Dim3_alternative(weight, {1,2,3,4,5,6}));
  }
}

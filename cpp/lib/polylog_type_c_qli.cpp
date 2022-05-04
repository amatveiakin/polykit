#include "polylog_type_c_qli.h"

#include "polylog_qli.h"


DeltaExpr typeC_QLi_arg4(int weight, const XArgs& args) {
  const auto& points = args.as_x();
  CHECK_EQ(points.size(), 4);
  return QLiVec(weight, args).dived_int(int_pow(2, weight-1)).without_annotations().annotate(
    fmt::function_num_args(
      fmt::sub_num(fmt::opname("typeC_QLi"), {weight}),
      points
    )
  );
}

DeltaExpr typeC_QLi_arg8(int weight, const XArgs& args) {
  const auto& points = args.as_x();
  CHECK_EQ(points.size(), 8);
  CHECK_GE(weight, 2);
  const int w = weight - 1;
  auto expr =
    + tensor_product(
      QLiVec   (1, choose_indices_one_based(points, std::vector{1,4,5,8})),
      QLiVec   (w, choose_indices_one_based(points, std::vector{1,2,3,4}))
    )
    - tensor_product(
      QLiNegVec(1, choose_indices_one_based(points, std::vector{2,5,6,1})),
      QLiNegVec(w, choose_indices_one_based(points, std::vector{2,3,4,5}))
    )
    + tensor_product(
      QLiVec   (1, choose_indices_one_based(points, std::vector{3,6,7,2})),
      QLiVec   (w, choose_indices_one_based(points, std::vector{3,4,5,6}))
    )
    - tensor_product(
      QLiNegVec(1, choose_indices_one_based(points, std::vector{4,7,8,3})),
      QLiNegVec(w, choose_indices_one_based(points, std::vector{4,5,6,7}))
    )
  ;
  if (weight > 2) {
    expr += tensor_product(cross_ratio(points), typeC_QLi_arg8(w, points)).dived_int(2);
  }
  return expr.without_annotations().annotate(
    fmt::function_num_args(
      fmt::sub_num(fmt::opname("typeC_QLi"), {weight}),
      points
    )
  );
}

DeltaExpr typeC_QLi(int weight, const XArgs& args) {
  // TODO: Do we need this check? Maybe as a warning?
  // CHECK(inv_points_are_central_symmetric(args));
  switch (args.size()) {
    case 4: return typeC_QLi_arg4(weight, args);
    case 8: return typeC_QLi_arg8(weight, args);
    default: FATAL("Unsupported number of arguments");
  }
}


DeltaExpr typeC_QLiSymm(int weight, const XArgs& args) {
  if (args.size() == 4) {
    return typeC_QLi(weight, args);
  }
  const auto& points = args.as_x();
  CHECK_EQ(points.size(), 8);
  return (
    + typeC_QLi(weight, points)
    + (
      + QLiVec(weight, choose_indices_one_based(points, {1,2,3,4}))
      - QLiVec(weight, choose_indices_one_based(points, {3,4,5,2}))
      + QLiVec(weight, choose_indices_one_based(points, {3,4,5,6}))
      - QLiVec(weight, choose_indices_one_based(points, {5,6,7,4}))
    )
    + (
      + typeC_QLi(weight, choose_indices_one_based(points, {1,2,5,6}))
      + typeC_QLi(weight, choose_indices_one_based(points, {3,4,7,8}))
    )
  ).without_annotations().annotate(
    fmt::function_num_args(
      fmt::sub_num(fmt::opname("typeC_QLiSymm"), {weight}),
      points
    )
  );
}

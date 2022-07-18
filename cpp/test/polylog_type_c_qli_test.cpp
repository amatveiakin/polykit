#include "lib/polylog_type_c_qli.h"

#include "gtest/gtest.h"

#include "lib/polylog_qli.h"
#include "test_util/helpers.h"
#include "test_util/matchers.h"


#if ENABLE_NEGATIVE_DELTA_VARIABLES
TEST(TypeCQLiTest, TypeCQLi_SymmetrizationFormula) {
  const int weight = 4;
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    +8*typeC_QLi(weight, {x1,x2,x3,x4,-x1,-x2,-x3,-x4})
    +8*typeC_QLi(weight, {x2,x3,x4,-x1,-x2,-x3,-x4,x1})
    +8*QLi4(x1,x2,x3,x4)
    +8*QLi4(x2,x3,x4,-x1)
    +8*QLi4(x3,x4,-x1,-x2)
    +8*QLi4(x4,-x1,-x2,-x3)
    +8*QLi4(-x1,-x2,-x3,-x4)
    +8*QLi4(-x2,-x3,-x4,x1)
    +8*QLi4(-x3,-x4,x1,x2)
    +8*QLi4(-x4,x1,x2,x3)
    +  QLi4(x1,x2,-x1,-x2)
    +  QLi4(x2,x3,-x2,-x3)
    +  QLi4(x3,x4,-x3,-x4)
    +  QLi4(x4,-x1,-x4,x1)
  );
}

TEST(TypeCQLiTest, LARGE_TypeCQLiSymm4_IsSymmetric) {
  const std::vector points = {x1,x2,x3,x4,-x1,-x2,-x3,-x4};
  for (const int weight : range_incl(2, 6)) {
    const int sign = neg_one_pow(weight);
    EXPECT_EXPR_ZERO_AFTER_LYNDON(
      + typeC_QLiSymm(weight, points)
      + sign * typeC_QLiSymm(weight, rotated_vector(points, 1))
    );
  }
}

TEST(TypeCQLiTest, TypeCQLiSymm3_Formula) {
  const int weight = 3;
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + typeC_QLiSymm(weight, {x1,x2,x3,x4,-x1,-x2,-x3,-x4})
    - (
      + typeC_QLi(weight, {x1,x2,-x1,-x2})
      + typeC_QLi(weight, {x2,x3,-x2,-x3})
      + typeC_QLi(weight, {x3,x4,-x3,-x4})
      + typeC_QLi(weight, {x1,x4,-x1,-x4})
    )
    + (
      + typeC_QLi(weight, {x1,x3,-x1,-x3})
      + typeC_QLi(weight, {x2,x4,-x2,-x4})
    )
    - (
      + QLiSymmVec(weight, {x1,x2,x3,x4})
      + QLiSymmVec(weight, {x2,x3,x4,-x1})
      + QLiSymmVec(weight, {x3,x4,-x1,-x2})
      + QLiSymmVec(weight, {x4,-x1,-x2,-x3})
    )
    - (
      + QLiSymmVec(weight, {x1,x2,x4,-x1})
      + QLiSymmVec(weight, {x2,x3,-x1,-x2})
      + QLiSymmVec(weight, {x3,x4,-x2,-x3})
      + QLiSymmVec(weight, {x4,-x1,-x3,-x4})
    )
    + (
      + QLiSymmVec(weight, {x1,x2,x3,-x1})
      + QLiSymmVec(weight, {x2,x3,x4,-x2})
      + QLiSymmVec(weight, {x3,x4,-x1,-x3})
      + QLiSymmVec(weight, {x4,-x1,-x2,-x4})
    )
    + (
      + QLiSymmVec(weight, {x1,x2,x3,-x3})
      + QLiSymmVec(weight, {x2,x3,x4,-x4})
      + QLiSymmVec(weight, {x1,x3,x4,-x1})
      + QLiSymmVec(weight, {x2,x4,-x1,-x2})
    )
  );
}

TEST(TypeCQLiTest, TypeCQLiSymm4_Sum) {
  const int weight = 4;
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    // sign for typeC_QLiSymm = (-1)^(sum of the positive indices)
    + typeC_QLiSymm(weight, {x1,x2,x3,x4,-x1,-x2,-x3,-x4})
    - typeC_QLiSymm(weight, {x1,x2,x3,x5,-x1,-x2,-x3,-x5})
    + typeC_QLiSymm(weight, {x1,x2,x4,x5,-x1,-x2,-x4,-x5})
    - typeC_QLiSymm(weight, {x1,x3,x4,x5,-x1,-x3,-x4,-x5})
    + typeC_QLiSymm(weight, {x2,x3,x4,x5,-x2,-x3,-x4,-x5})
    - typeC_QLiSymm(weight, {x1,x2,-x1,-x2})
    + typeC_QLiSymm(weight, {x1,x3,-x1,-x3})
    - typeC_QLiSymm(weight, {x1,x4,-x1,-x4})
    + typeC_QLiSymm(weight, {x1,x5,-x1,-x5})
    - typeC_QLiSymm(weight, {x2,x3,-x2,-x3})
    + typeC_QLiSymm(weight, {x2,x4,-x2,-x4})
    - typeC_QLiSymm(weight, {x2,x5,-x2,-x5})
    - typeC_QLiSymm(weight, {x3,x4,-x3,-x4})
    + typeC_QLiSymm(weight, {x3,x5,-x3,-x5})
    - typeC_QLiSymm(weight, {x4,x5,-x4,-x5})
    - (
      + QLiSymm4(x1,x2,x3,x4,x5,-x1)
      + QLiSymm4(x2,x3,x4,x5,-x1,-x2)
      + QLiSymm4(x3,x4,x5,-x1,-x2,-x3)
      + QLiSymm4(x4,x5,-x1,-x2,-x3,-x4)
      + QLiSymm4(x5,-x1,-x2,-x3,-x4,-x5)
    )
    - 2 * (
      + QLi4(x1,x2,x3,x4)
      + QLi4(x2,x3,x4,x5)
      + QLi4(x3,x4,x5,-x1)
      + QLi4(x4,x5,-x1,-x2)
      + QLi4(x5,-x1,-x2,-x3)
    )
    + (
      + QLi4(x1,x2,x3,x5)
      + QLi4(x2,x3,x4,-x1)
      + QLi4(x3,x4,x5,-x2)
      + QLi4(x4,x5,-x1,-x3)
      + QLi4(x5,-x1,-x2,-x4)
    )
    + (
      + QLi4(x1,x3,x4,x5)
      + QLi4(x2,x4,x5,-x1)
      + QLi4(x3,x5,-x1,-x2)
      + QLi4(x4,-x1,-x2,-x3)
      + QLi4(x5,-x2,-x3,-x4)
    )
    - (
      + QLi4(x1,x2,x4,x5)
      + QLi4(x2,x3,x5,-x1)
      + QLi4(x3,x4,-x1,-x2)
      + QLi4(x4,x5,-x2,-x3)
      + QLi4(x5,-x1,-x3,-x4)
    )
    - (
      + QLi4(x1,x2,x3,-x1)
      + QLi4(x2,x3,x4,-x2)
      + QLi4(x3,x4,x5,-x3)
      + QLi4(x4,x5,-x1,-x4)
      + QLi4(x5,-x1,-x2,-x5)
    )
    + (
      + QLi4(x1,x2,x4,-x1)
      + QLi4(x2,x3,x5,-x2)
      + QLi4(x3,x4,-x1,-x3)
      + QLi4(x4,x5,-x2,-x4)
      + QLi4(x5,-x1,-x3,-x5)
    )
    - (
      + QLi4(x1,x2,x5,-x1)
      + QLi4(x2,x3,-x1,-x2)
      + QLi4(x3,x4,-x2,-x3)
      + QLi4(x4,x5,-x3,-x4)
      + QLi4(x5,-x1,-x4,-x5)
    )
    - (
      + QLi4(x1,x3,x4,-x1)
      + QLi4(x2,x4,x5,-x2)
      + QLi4(x3,x5,-x1,-x3)
      + QLi4(x4,-x1,-x2,-x4)
      + QLi4(x5,-x2,-x3,-x5)
    )
    + (
      + QLi4(x1,x3,x5,-x1)
      + QLi4(x2,x4,-x1,-x2)
      + QLi4(x3,x5,-x2,-x3)
      + QLi4(x4,-x1,-x3,-x4)
      + QLi4(x5,-x2,-x4,-x5)
    )
    - (
      + QLi4(x1,x4,x5,-x1)
      + QLi4(x2,x5,-x1,-x2)
      + QLi4(x3,-x1,-x2,-x3)
      + QLi4(x4,-x2,-x3,-x4)
      + QLi4(x5,-x3,-x4,-x5)
    )
  );
}
#endif

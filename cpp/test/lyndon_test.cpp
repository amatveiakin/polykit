#include "lib/lyndon.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "test_util/helpers.h"
#include "test_util/matchers.h"


struct InversedVectorExprParam : SimpleVectorExprParam {
  static bool lyndon_compare(const VectorT::value_type& lhs, const VectorT::value_type& rhs) {
    return rhs < lhs;
  }
};

using InversedVectorExpr = Linear<InversedVectorExprParam>;

inline InversedVectorExpr IV(std::vector<int> data) {
  return InversedVectorExpr::single(std::move(data));
}


TEST(LyndonBasisTest, Empty) {
  EXPECT_EXPR_EQ(
    to_lyndon_basis(SimpleVectorExpr{}),
    SimpleVectorExpr{}
  );
}

TEST(LyndonBasisTest, Size1) {
  const auto expr =
    + SV({1})
    + SV({3})
    + SV({7})
  ;
  EXPECT_EXPR_EQ(
    to_lyndon_basis(expr),
    expr
  );
}

TEST(LyndonBasisTest, Size2) {
  EXPECT_EXPR_EQ(
    to_lyndon_basis(
      + SV({1,2})
      + SV({7,5})
    ),
    + SV({1,2})
    - SV({5,7})
  );
}

TEST(LyndonBasisTest, Size2_Inversed) {
  EXPECT_EXPR_EQ(
    to_lyndon_basis(
      + IV({1,2})
      + IV({7,5})
    ),
    - IV({2,1})
    + IV({7,5})
  );
}

TEST(LyndonBasisTest, Size3) {
  EXPECT_EXPR_ZERO(
    to_lyndon_basis(
      + SV({2,1,3})
      + SV({7,6,5})
    )
    + SV({1,2,3})
    + SV({1,3,2})
    - SV({5,6,7})
  );
}

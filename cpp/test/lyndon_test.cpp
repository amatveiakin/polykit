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


TEST(LyndonWordsTest, Binary) {
  EXPECT_THAT(
    mapped(range(0, 10), [](int length) {
      return get_lyndon_words(2, length).size();
    }),
    // https://oeis.org/A001037
    testing::ElementsAre(1, 2, 1, 2, 3, 6, 9, 18, 30, 56)
  );
}

TEST(LyndonWordsTest, Alphabet3_Length3) {
  EXPECT_THAT(
    get_lyndon_words(3, 3),
    testing::ElementsAre(
      std::vector{0, 0, 1},
      std::vector{0, 0, 2},
      std::vector{0, 1, 1},
      std::vector{0, 1, 2},
      std::vector{0, 2, 1},
      std::vector{0, 2, 2},
      std::vector{1, 1, 2},
      std::vector{1, 2, 2}
    )
  );
}

TEST(LyndonWordsTest, CustomAlphabet) {
  EXPECT_THAT(
    get_lyndon_words(std::vector{"b", "a"}, 3),
    testing::ElementsAre(
      std::vector{"b", "b", "a"},
      std::vector{"b", "a", "a"}
    )
  );
}

TEST(LyndonWordsTest, DoesNotFactorize) {
  for (const int alphabet_size : range_incl(2, 3)) {
    for (const int length : range_incl(2, 8)) {
      for (const auto& word: get_lyndon_words(alphabet_size, length)) {
        EXPECT_EQ(lyndon_factorize(word).size(), 1);
      }
    }
  }
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

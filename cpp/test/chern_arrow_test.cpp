
#include "lib/chern_arrow.h"

#include "gtest/gtest.h"

#include "test_util/matchers.h"
#include "lib/polylog_gli.h"
#include "lib/polylog_grli.h"
#include "lib/polylog_grqli.h"


static std::vector exprs_odd_num_points = {
  GLi2[{7}](1,2,3,4,5,6),
  GrQLi2(7)(1,2,3,4,5,6),
  GrLi(5)(1,2,3,4),
  G({1,2,3,4,5}),
  tensor_product(G({1,2,3}), G({3,4,5})),
};

static std::vector exprs_even_num_points = {
  GLi2(1,2,3,4,5,6),
  GrQLi2()(1,2,3,4,5,6),
  GrLi(5,6)(1,2,3,4),
  G({1,2,3,4,5,6}),
  tensor_product(G({1,2,3,4}), G({3,4,5,6})),
};

static std::vector exprs_any_num_points = concat(exprs_odd_num_points, exprs_even_num_points);

// Identities that should be true for all symbols.
class ChernArrowIdentityTest : public testing::TestWithParam<GammaExpr> {
public:
  const GammaExpr& expr() const { return GetParam(); }
};
class OddNumPointsIdentityTest : public testing::TestWithParam<GammaExpr> {
public:
  const GammaExpr& expr() const { return GetParam(); }
};
class EvenNumPointsIdentityTest : public testing::TestWithParam<GammaExpr> {
public:
  const GammaExpr& expr() const { return GetParam(); }
};

TEST_P(ChernArrowIdentityTest, LeftLeft) {
  const int n = detect_num_variables(expr());
  EXPECT_EXPR_ZERO(chern_arrow_left(chern_arrow_left(expr(), n+1), n+2));
}

TEST_P(ChernArrowIdentityTest, UpUp) {
  const int n = detect_num_variables(expr());
  EXPECT_EXPR_ZERO(chern_arrow_up(chern_arrow_up(expr(), n+1), n+2));
}

TEST_P(ChernArrowIdentityTest, LeftUp) {
  const int n = detect_num_variables(expr());
  EXPECT_EXPR_ZERO(
    + chern_arrow_left(chern_arrow_up(expr(), n+1), n+2)
    + chern_arrow_up(chern_arrow_left(expr(), n+1), n+2)
  );
}

TEST_P(ChernArrowIdentityTest, ABDecompositions) {
  const auto& x = expr();
  const int n = detect_num_variables(x);
  EXPECT_EXPR_EQ(a_full(x, n+1), a_minus(x, n+1) + a_plus_plus(x, n+1));
  EXPECT_EXPR_EQ(a_full(x, n+1), a_plus(x, n+1) + a_minus_minus(x, n+1));
  EXPECT_EXPR_EQ(b_full(x, n+1), b_minus(x, n+1) + b_plus_plus(x, n+1));
  EXPECT_EXPR_EQ(b_full(x, n+1), b_plus(x, n+1) + b_minus_minus(x, n+1));
}

TEST_P(OddNumPointsIdentityTest, ABOddEquivalences) {
  const auto& x = expr();
  const int n = detect_num_variables(x);
  EXPECT_EXPR_EQ(a_minus(x, n+1), a_minus_minus(x, n+1));
  EXPECT_EXPR_EQ(a_plus(x, n+1), a_plus_plus(x, n+1));
  EXPECT_EXPR_EQ(b_minus(x, n+1), b_minus_minus(x, n+1));
  EXPECT_EXPR_EQ(b_plus(x, n+1), b_plus_plus(x, n+1));
}

// Equations that hold true both for odd and even number of points.
// Proposition 4.1 from https://arxiv.org/pdf/2208.01564v1.pdf
TEST_P(ChernArrowIdentityTest, ABEquations) {
  const auto& x = expr();
  const int n = detect_num_variables(x);
  EXPECT_EXPR_ZERO(a_minus(a_minus_minus(x, n+1), n+2));
  EXPECT_EXPR_ZERO(a_plus(a_plus_plus(x, n+1), n+2));
  EXPECT_EXPR_ZERO(b_minus(b_minus_minus(x, n+1), n+2));
  EXPECT_EXPR_ZERO(b_plus(b_plus_plus(x, n+1), n+2));
  EXPECT_EXPR_EQ(a_minus_minus(a_plus(x, n+1), n+2), -a_plus_plus(a_minus(x, n+1), n+2));
  EXPECT_EXPR_EQ(b_minus_minus(b_plus(x, n+1), n+2), -b_plus_plus(b_minus(x, n+1), n+2));
  EXPECT_EXPR_EQ(a_plus(b_plus_plus(x, n+1), n+2), -b_plus(a_plus_plus(x, n+1), n+2));
  EXPECT_EXPR_EQ(a_minus_minus(b_plus(x, n+1), n+2), -b_plus_plus(a_minus(x, n+1), n+2));
  EXPECT_EXPR_EQ(a_minus(b_minus_minus(x, n+1), n+2), -b_minus(a_minus_minus(x, n+1), n+2));
  EXPECT_EXPR_EQ(a_plus_plus(b_minus(x, n+1), n+2), -b_minus_minus(a_plus(x, n+1), n+2));
}

TEST(ChernArrowTest, LARGE_GenerateABEquations) {
  ScopedFormatting sf(FormattingConfig()
    .set_encoder(Encoder::ascii)
    .set_rich_text_format(RichTextFormat::plain_text)
    .set_max_terms_in_annotations_one_liner(100)
  );

  const auto arrows = list_ab_function(ABDoublePlusMinus::Include);
  absl::flat_hash_set<std::pair<int, int>> zero_eqn_indices;
  std::vector<std::string> zero_eqns;
  for (const int out : range(arrows.size())) {
    for (const int in : range(arrows.size())) {
      const auto make_eqn = [&](const auto& expr, const int n) {
        return arrows[out](arrows[in](expr, n+1), n+2);
      };
      bool eqn_holds = true;
      for (const auto& expr : exprs_any_num_points) {
        const int n = detect_num_variables(expr);
        const auto eqn = make_eqn(expr, n);
        if (!eqn.is_zero()) {
          eqn_holds = false;
          break;
        }
      }
      if (eqn_holds) {
        const auto expr = GammaExpr().annotate("x");
        const auto eqn = make_eqn(expr, 0);
        zero_eqn_indices.insert({out, in});
        zero_eqns.push_back(absl::StrCat(annotations_one_liner(eqn.annotations()), " == 0"));
      }
    }
  }

  std::vector<std::string> equality_eqns;
  for (const int l_out : range(arrows.size())) {
    for (const int l_in : range(arrows.size())) {
      for (const int r_out : range(arrows.size())) {
        for (const int r_in : range(arrows.size())) {
          const bool is_trivial =
            std::tie(l_out, l_in) >= std::tie(r_out, r_in)
            || zero_eqn_indices.contains({l_out, l_in})
            || zero_eqn_indices.contains({r_out, r_in})
          ;
          if (is_trivial) {
            continue;
          }
          for (const int sign : {-1, 1}) {
            const auto make_eqn = [&](const auto& expr, const int n) {
              return
                + arrows[l_out](arrows[l_in](expr, n+1), n+2)
                + sign * arrows[r_out](arrows[r_in](expr, n+1), n+2)
              ;
            };
            bool eqn_holds = true;
            for (const auto& expr : exprs_any_num_points) {
              const int n = detect_num_variables(expr);
              const auto eqn = make_eqn(expr, n);
              if (!eqn.is_zero()) {
                eqn_holds = false;
                break;
              }
            }
            if (eqn_holds) {
              const auto expr = GammaExpr().annotate("x");
              const auto eqn = make_eqn(expr, 0);
              equality_eqns.push_back(absl::StrCat(annotations_one_liner(eqn.annotations()), " == 0"));
            }
          }
        }
      }
    }
  }

  EXPECT_THAT(zero_eqns, testing::UnorderedElementsAre(
    "a(a(x)) == 0",
    "a-(a--(x)) == 0",
    "a+(a++(x)) == 0",
    "b(b(x)) == 0",
    "b-(b--(x)) == 0",
    "b+(b++(x)) == 0"
  ));

  EXPECT_THAT(equality_eqns, testing::UnorderedElementsAre(
    "(a(a++(x)) + a(a-(x))) == 0",
    "(a(a-(x)) + a--(a++(x))) == 0",
    "(a(a+(x)) + a(a--(x))) == 0",
    "(a(a+(x)) + a++(a--(x))) == 0",
    "(a(b(x)) + b(a(x))) == 0",
    "(a(a--(x)) - a++(a--(x))) == 0",
    "(a(a++(x)) - a--(a++(x))) == 0",
    "(a-(a(x)) - a-(a+(x))) == 0",
    "(a++(a(x)) + a-(a(x))) == 0",
    "(a++(a(x)) + a-(a+(x))) == 0",
    "(a-(b--(x)) + b-(a--(x))) == 0",
    "(a+(a(x)) - a+(a-(x))) == 0",
    "(a+(a(x)) + a--(a(x))) == 0",
    "(a+(a-(x)) + a--(a(x))) == 0",
    "(a+(b++(x)) + b+(a++(x))) == 0",
    "(b(b++(x)) + b(b-(x))) == 0",
    "(b(b-(x)) + b--(b++(x))) == 0",
    "(b(b+(x)) + b(b--(x))) == 0",
    "(b(b+(x)) + b++(b--(x))) == 0",
    "(b(b--(x)) - b++(b--(x))) == 0",
    "(b(b++(x)) - b--(b++(x))) == 0",
    "(b-(b(x)) - b-(b+(x))) == 0",
    "(b++(b(x)) + b-(b(x))) == 0",
    "(b++(b(x)) + b-(b+(x))) == 0",
    "(b+(b(x)) - b+(b-(x))) == 0",
    "(b+(b(x)) + b--(b(x))) == 0",
    "(b+(b-(x)) + b--(b(x))) == 0",
    "(a++(a-(x)) + a--(a+(x))) == 0",
    "(a--(b+(x)) + b++(a-(x))) == 0",
    "(a++(b-(x)) + b--(a+(x))) == 0",
    "(b++(b-(x)) + b--(b+(x))) == 0"
  ));
}

TEST(ChernArrowTest, LARGE_GenerateABCEquations) {
  ScopedFormatting sf(FormattingConfig()
    .set_encoder(Encoder::ascii)
    .set_rich_text_format(RichTextFormat::plain_text)
    .set_max_terms_in_annotations_one_liner(100)
  );

  const auto ab_functions = list_nco_ab_function(ABDoublePlusMinus::Include);
  const std::vector c_functions = {
    c_minus,
    c_plus,
  };
  std::vector<std::string> zero_eqns;
  for (const auto& ab : ab_functions) {
    for (const auto& c : c_functions) {
      for (const bool c_inside : {false, true}) {
        const auto make_eqn = [&](const auto& x, const int n) {
          return c_inside
            ? ab(c(x, n), n+1)
            : c(ab(x, n+1), n+1)
          ;
        };
        bool eqn_holds = true;
        for (const auto& expr : exprs_any_num_points) {
          const int n = detect_num_variables(expr);
          const auto eqn = make_eqn(ncoproduct(expr), n);
          if (!eqn.is_zero()) {
            eqn_holds = false;
            break;
          }
        }
        if (eqn_holds) {
          const auto x = GammaNCoExpr().annotate("x");
          const auto eqn = make_eqn(x, 0);
          zero_eqns.push_back(absl::StrCat(annotations_one_liner(eqn.annotations()), " == 0"));
        }
      }
    }
  }

  std::vector<std::string> equality_eqns;
  for (const int l_ab_idx : range(ab_functions.size())) {
    for (const int l_c_idx : range(c_functions.size())) {
      for (const bool l_c_inside : {false, true}) {
        for (const int r_ab_idx : range(ab_functions.size())) {
          for (const int r_c_idx : range(c_functions.size())) {
            for (const bool r_c_inside : {false, true}) {
              const bool is_trivial =
                std::tie(l_ab_idx, l_c_idx, l_c_inside) <= std::tie(r_ab_idx, r_c_idx, r_c_inside)
              ;
              if (is_trivial) {
                continue;
              }
              const auto l_ab = ab_functions.at(l_ab_idx);
              const auto l_c = c_functions.at(l_c_idx);
              const auto r_ab = ab_functions.at(r_ab_idx);
              const auto r_c = c_functions.at(r_c_idx);
              for (const int sign : {-1, 1}) {
                const auto make_eqn = [&](const auto& x, const int n) {
                  const auto lhs = l_c_inside
                    ? l_ab(l_c(x, n), n+1)
                    : l_c(l_ab(x, n+1), n+1)
                  ;
                  const auto rhs = r_c_inside
                    ? r_ab(r_c(x, n), n+1)
                    : r_c(r_ab(x, n+1), n+1)
                  ;
                  return lhs + sign * rhs;
                };
                bool eqn_holds = true;
                for (const auto& expr : exprs_any_num_points) {
                  const int n = detect_num_variables(expr);
                  const auto eqn = make_eqn(ncoproduct(expr), n);
                  if (!eqn.is_zero()) {
                    eqn_holds = false;
                    break;
                  }
                }
                if (eqn_holds) {
                  const auto x = GammaNCoExpr().annotate("x");
                  const auto eqn = make_eqn(x, 0);
                  equality_eqns.push_back(absl::StrCat(annotations_one_liner(eqn.annotations()), " == 0"));
                }
              }
            }
          }
        }
      }
    }
  }

  EXPECT_THAT(zero_eqns, testing::IsEmpty());
  EXPECT_THAT(equality_eqns, testing::UnorderedElementsAre(
    "(b--(c-(x)) - c-(b--(x))) == 0",
    "(b++(c+(x)) - c+(b++(x))) == 0"
  ));
}

TEST(ChernArrowTest, LARGE_GenerateGLiViaLowerGLiABEquations) {
  ScopedFormatting sf(FormattingConfig()
    .set_encoder(Encoder::ascii)
    .set_rich_text_format(RichTextFormat::plain_text)
    .set_compact_x(true)
    .set_max_terms_in_annotations_one_liner(100)
  );

  std::vector<std::string> equations;
  const auto ab_full = list_ab_function(ABDoublePlusMinus::Include);
  const auto ab_core = list_ab_function(ABDoublePlusMinus::Exclude);
  const int p = 3;
  for (const auto& out : ab_core) {
    for (const auto& in : ab_full) {
      for (const int sign : {-1, 1}) {
        const auto eqn = to_lyndon_basis(
          + GLiVec(p, seq_incl(1, 2*p+2))
          + sign * out(in(GLiVec(p, seq_incl(1, 2*p)), 2*p+1), 2*p+2)
        );
        if (eqn.is_zero()) {
          equations.push_back(absl::StrCat(annotations_one_liner(eqn.annotations()), " = 0"));
        }
      }
    }
  }

  EXPECT_THAT(equations, testing::UnorderedElementsAre(
    "(GLi_3(1,2,3,4,5,6,7,8) - a-(b+(GLi_3(1,2,3,4,5,6)))) = 0",
    "(GLi_3(1,2,3,4,5,6,7,8) + a-(b--(GLi_3(1,2,3,4,5,6)))) = 0",
    "(GLi_3(1,2,3,4,5,6,7,8) - a+(b-(GLi_3(1,2,3,4,5,6)))) = 0",
    "(GLi_3(1,2,3,4,5,6,7,8) + b-(a+(GLi_3(1,2,3,4,5,6)))) = 0",
    "(GLi_3(1,2,3,4,5,6,7,8) - b-(a--(GLi_3(1,2,3,4,5,6)))) = 0",
    "(GLi_3(1,2,3,4,5,6,7,8) + b+(a-(GLi_3(1,2,3,4,5,6)))) = 0"
  ));
}

INSTANTIATE_TEST_SUITE_P(AllCases, ChernArrowIdentityTest, testing::ValuesIn(exprs_any_num_points));
INSTANTIATE_TEST_SUITE_P(AllCases, OddNumPointsIdentityTest, testing::ValuesIn(exprs_odd_num_points));
INSTANTIATE_TEST_SUITE_P(AllCases, EvenNumPointsIdentityTest, testing::ValuesIn(exprs_even_num_points));

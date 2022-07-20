#include "lib/delta_parse.h"

#include <sstream>

#include "gtest/gtest.h"

#include "lib/polylog_qli.h"
#include "test_util/matchers.h"


TEST(ParseDeltaExpressionTest, Basic) {
  EXPECT_EXPR_EQ(
    parse_delta_expression(R"(
      + (x1-x2)(x1-x3)(x2-x3)
      +2(x1-x3)(x1-x3)(x1-x4)
      - (x1-x4)(x1-x3)(x1-x4)
    )"),
    +  DeltaExpr::single({{1,2}, {1,3}, {2,3}})
    +2*DeltaExpr::single({{1,3}, {1,3}, {1,4}})
    -  DeltaExpr::single({{1,4}, {1,3}, {1,4}})
  );
}

TEST(ParseDeltaExpressionTest, FormMix) {
  EXPECT_EXPR_EQ(
    parse_delta_expression(R"(
      +2 (x1-x2)*(x1-x3)
      +3 (x_1-x_2)*(x_1-x_4)
      +5*[1,2][1,5]
    )"),
    +2*DeltaExpr::single({{1,2}, {1,3}})
    +3*DeltaExpr::single({{1,2}, {1,4}})
    +5*DeltaExpr::single({{1,2}, {1,5}})
  );
}

TEST(ParseDeltaExpressionTest, MultidigitNumbers) {
  EXPECT_EXPR_EQ(
    parse_delta_expression(R"(
      -137 (x05-x10)
    )"),
    -137*DeltaExpr::single({{5,10}})
  );
}

TEST(ParseDeltaExpressionTest, OutputIsReadable) {
  ScopedFormatting sf(FormattingConfig()
    .set_encoder(Encoder::ascii)
    .set_rich_text_format(RichTextFormat::plain_text)
    .set_expression_line_limit(FormattingConfig::kNoLineLimit)
  );
  const DeltaExpr expr = QLi3(1,2,3,4,5,6);
  std::stringstream ss;
  ss << expr.main();
  const DeltaExpr expr_parsed = parse_delta_expression(ss.str());
  EXPECT_EXPR_EQ(expr, expr_parsed);
}

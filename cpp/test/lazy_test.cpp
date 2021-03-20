#include "lib/lazy.h"

#include "gtest/gtest.h"

#include "lib/lazy/lazy_delta.h"
#include "lib/polylog_qli.h"
#include "test_util/matchers.h"


TEST(LazyTest, MakeLazyEqualsEager) {
  auto lazy = make_lazy([]() { return QLi3(1,2,3,4); });
  EXPECT_EXPR_EQ(
    lazy_substitute_variables(lazy, {4,3,2,1}).evaluate(),
    substitute_variables(QLi3(1,2,3,4), {4,3,2,1})
  );
}

TEST(LazyTest, AsLazyEqualsEager) {
  auto lazy = as_lazy(QLi3(1,2,3,4));
  EXPECT_EXPR_EQ(
    lazy_substitute_variables(lazy, {4,3,2,1}).evaluate(),
    substitute_variables(QLi3(1,2,3,4), {4,3,2,1})
  );
}

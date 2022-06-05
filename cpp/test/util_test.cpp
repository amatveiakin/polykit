#include "lib/util.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using testing::Eq;
using testing::Pointee;


TEST(IntPowTest, Basic) {
  EXPECT_EQ(int_pow(2, 0), 1);
  EXPECT_EQ(int_pow(2, 1), 2);
  EXPECT_EQ(int_pow(2, 2), 4);
  EXPECT_EQ(int_pow(2, 3), 8);
  EXPECT_EQ(int_pow(2, 4), 16);
  EXPECT_EQ(int_pow(2, 5), 32);
  EXPECT_EQ(int_pow(2, 6), 64);
  EXPECT_EQ(int_pow(2, 7), 128);
  EXPECT_EQ(int_pow(2, 8), 256);
}

TEST(AppendVectorTest, MoveOnly) {
  std::vector<std::unique_ptr<int>> a;
  a.push_back(std::make_unique<int>(1));
  std::vector<std::unique_ptr<int>> b;
  b.push_back(std::make_unique<int>(2));
  append_vector(a, std::move(b));  // should not compile without `std::move`
  EXPECT_THAT(a, testing::ElementsAre(Pointee(Eq(1)), Pointee(Eq(2))));
}

TEST(AllEqualTest, Basic) {
  EXPECT_FALSE(all_equal(std::vector{1, 1, 3, 1, 5, 3}));
  EXPECT_FALSE(all_equal(std::vector{1, 1, 3, 1, 5, 3}, [](const auto& v) { return v; }));
  EXPECT_TRUE(all_equal(std::vector{1, 1, 3, 1, 5, 3}, [](const auto& v) { return v % 2; }));
}

TEST(SumTest, Basic) {
  EXPECT_EQ(sum(std::vector{1, 2, 3, 4}), 10);
  EXPECT_EQ(sum(std::vector{1, 2, 3, 4}, [](const auto& v) { return v * v; }), 30);
}

TEST(GroupByTest, GroupEqual) {
  EXPECT_EQ(
    group_equal(std::vector<int>{1, 2, 2, 1, 3, 3, 3, 1, 1, 3}),
    (std::vector<std::vector<int>>{{1}, {2, 2}, {1}, {3, 3, 3}, {1, 1}, {3}})
  );
}

TEST(MappedTest, MappedExpanding) {
  std::vector input{1, 2, 3, 4};
  const auto func = [](const int x) { return std::vector<int>(x, x); };
  EXPECT_EQ(
    mapped_expanding(input, func),
    flatten(mapped(input, func))
  );
  EXPECT_EQ(
    mapped_expanding(input, func),
    (std::vector{1, 2, 2, 3, 3, 3, 4, 4, 4, 4})
  );
}

TEST(MappedTest, FilteredMapped) {
  EXPECT_EQ(
    filtered_mapped(
      std::vector{1, 1, 2, 3, 5, 8, 13, 21, 34},
      [](const int x) { return x % 2 == 0 ? std::optional(to_string(x)) : std::nullopt; }
    ),
    (std::vector<std::string>{"2", "8", "34"})
  );
}

TEST(MappedTest, MappedNested) {
  static constexpr auto add_one = [](const int x) { return x + 1; };
  EXPECT_EQ(
    mapped_nested<1>(std::vector<int>{10, 100, 1000}, add_one),
    (std::vector<int>{11, 101, 1001})
  );
  EXPECT_EQ(
    mapped_nested<2>(std::vector<std::vector<int>>{{10, 100}, {1000}}, add_one),
    (std::vector<std::vector<int>>{{11, 101}, {1001}})
  );
  EXPECT_EQ(
    mapped_nested<3>(std::vector<std::vector<std::vector<int>>>{{{10}, {100}}, {{1000}}}, add_one),
    (std::vector<std::vector<std::vector<int>>>{{{11}, {101}}, {{1001}}})
  );
}

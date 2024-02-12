#include "lib/compare.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"


TEST(ComparisonWrappers, ByValue) {
  std::vector<std::pair<int, int>> values = {
    {30, 2},
    {20, 1},
    {10, 1},
    {30, 1},
    {30, 3},
    {20, 2},
  };

  absl::c_sort(values, cmp::projected([](const auto& p) {
    return std::pair{cmp::desc_val(p.second), cmp::asc_val(p.first)};
  }));

  std::vector<std::pair<int, int>> expected_values = {
    {30, 3},
    {20, 2},
    {30, 2},
    {10, 1},
    {20, 1},
    {30, 1},
  };
  EXPECT_EQ(values, expected_values);
}

TEST(ComparisonWrappers, ByReference) {
  std::vector<std::unique_ptr<int>> values;
  values.push_back(std::make_unique<int>(3));
  values.push_back(std::make_unique<int>(1));
  values.push_back(std::make_unique<int>(7));

  absl::c_sort(values, cmp::projected([](const auto& v) {
    return cmp::asc_ref(*v);
  }));

  EXPECT_EQ(*values[0], 1);
  EXPECT_EQ(*values[1], 3);
  EXPECT_EQ(*values[2], 7);

  absl::c_sort(values, cmp::projected([](const auto& v) {
    return cmp::desc_ref(*v);
  }));

  EXPECT_EQ(*values[0], 7);
  EXPECT_EQ(*values[1], 3);
  EXPECT_EQ(*values[2], 1);
}

TEST(ComparisonWrappers, VecLengthFirst) {
  std::vector<std::vector<int>> values = {
    {20, 10},
    {100},
    {2, 2, 2},
    {1, 1, 1},
    {10, 10},
    {1, 2, 3},
  };

  absl::c_sort(values, cmp::projected([](const auto& v) {
    return std::pair{cmp::desc_val(v.size()), cmp::asc_ref(v)};
  }));

  std::vector<std::vector<int>> expected_values = {
    {1, 1, 1},
    {1, 2, 3},
    {2, 2, 2},
    {10, 10},
    {20, 10},
    {100},
  };
  EXPECT_EQ(values, expected_values);
}

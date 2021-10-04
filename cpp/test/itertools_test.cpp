#include "lib/itertools.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "lib/string.h"


using namespace std::literals::string_literals;

TEST(ItertoolsTest, Product_Elements4_Repeat2) {
  std::vector<std::string> result;
  for (const auto& p : cartesian_power({"A", "B", "C", "D"}, 2)) {
    result.push_back(str_join(p, ""));
  }
  absl::c_sort(result);
  EXPECT_THAT(result, testing::ElementsAre(
    "AA", "AB", "AC", "AD", "BA", "BB", "BC", "BD", "CA", "CB", "CC", "CD", "DA", "DB", "DC", "DD"
  ));
}

TEST(ItertoolsTest, Permutations_Elements0) {
  std::vector<std::vector<int>> result;
  for (const auto& p : permutations(std::vector<int>{})) {
    result.push_back(p);
  }
  EXPECT_THAT(result, testing::ElementsAre(
    std::vector<int>{}
  ));
}

TEST(ItertoolsTest, Permutations_Elements1) {
  std::vector<std::vector<std::string>> result;
  for (const auto& p : permutations({"solo"s})) {
    result.push_back(p);
  }
  EXPECT_THAT(result, testing::ElementsAre(
    std::vector{"solo"s}
  ));
}

TEST(ItertoolsTest, Permutations_Elements2) {
  std::vector<std::vector<std::string>> result;
  for (const auto& p : permutations({"one"s, "two"s})) {
    result.push_back(p);
  }
  EXPECT_THAT(result, testing::ElementsAre(
    std::vector{"one"s, "two"s},
    std::vector{"two"s, "one"s}
  ));
}

TEST(ItertoolsTest, Permutations_Elements3) {
  std::vector<std::vector<int>> result;
  for (const auto& p : permutations({3, 1, 2})) {
    result.push_back(p);
  }
  EXPECT_THAT(result, testing::ElementsAre(
    std::vector{1, 2, 3},
    std::vector{1, 3, 2},
    std::vector{2, 1, 3},
    std::vector{2, 3, 1},
    std::vector{3, 1, 2},
    std::vector{3, 2, 1}
  ));
}

TEST(ItertoolsTest, Permutations_Elements4_Size2) {
  std::vector<std::string> result;
  for (const auto& p : permutations({"A", "B", "C", "D"}, 2)) {
    result.push_back(str_join(p, ""));
  }
  absl::c_sort(result);
  EXPECT_THAT(result, testing::ElementsAre(
    "AB", "AC", "AD", "BA", "BC", "BD", "CA", "CB", "CD", "DA", "DB", "DC"
  ));
}

TEST(ItertoolsTest, Combinations_Elements4_Size2) {
  std::vector<std::string> result;
  for (const auto& p : combinations({"A", "B", "C", "D"}, 2)) {
    result.push_back(str_join(p, ""));
  }
  absl::c_sort(result);
  EXPECT_THAT(result, testing::ElementsAre(
    "AB", "AC", "AD", "BC", "BD", "CD"
  ));
}

TEST(ItertoolsTest, CombinationsWithReplacement_Elements4_Size2) {
  std::vector<std::string> result;
  for (const auto& p : combinations_with_replacement({"A", "B", "C", "D"}, 2)) {
    result.push_back(str_join(p, ""));
  }
  absl::c_sort(result);
  EXPECT_THAT(result, testing::ElementsAre(
    "AA", "AB", "AC", "AD", "BB", "BC", "BD", "CC", "CD", "DD"
  ));
}

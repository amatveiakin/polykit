#include "lib/permutations.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"


using namespace std::literals::string_literals;

TEST(PermutationsTest, Size0) {
  std::vector<std::vector<int>> result;
  for (const auto& p : permutations(std::vector<int>{})) {
    result.push_back(p);
  }
  EXPECT_THAT(result, testing::ElementsAre(
    std::vector<int>{}
  ));
}

TEST(PermutationsTest, Size1) {
  std::vector<std::vector<std::string>> result;
  for (const auto& p : permutations({"solo"s})) {
    result.push_back(p);
  }
  EXPECT_THAT(result, testing::ElementsAre(
    std::vector{"solo"s}
  ));
}

TEST(PermutationsTest, Size2) {
  std::vector<std::vector<std::string>> result;
  for (const auto& p : permutations({"one"s, "two"s})) {
    result.push_back(p);
  }
  EXPECT_THAT(result, testing::ElementsAre(
    std::vector{"one"s, "two"s},
    std::vector{"two"s, "one"s}
  ));
}

TEST(PermutationsTest, Size3) {
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

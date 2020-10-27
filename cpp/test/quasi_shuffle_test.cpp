#include "lib/quasi_shuffle.h"

#include "gtest/gtest.h"

#include "test_util/helpers.h"
#include "test_util/matchers.h"


TEST(QuasiShuffleProductTest, Shuffle_1_3) {
  EXPECT_THAT(
    quasi_shuffle_product(
      std::vector<std::string>{"a", "b", "c"},
      std::vector<std::string>{"x"},
      [](const std::string& p, const std::string& q) {
        return p + q;
      }
    )
    ,
    testing::UnorderedElementsAreArray(std::vector<std::vector<std::string>>{
      {"x", "a", "b", "c"},
      {"a", "x", "b", "c"},
      {"a", "b", "x", "c"},
      {"a", "b", "c", "x"},
      {"ax", "b", "c"},
      {"a", "bx", "c"},
      {"a", "b", "cx"},
    })
  );
}

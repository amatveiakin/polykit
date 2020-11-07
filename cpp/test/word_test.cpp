#include "lib/word.h"

#include "gtest/gtest.h"

#include "test_util/matchers.h"


TEST(WordTest, HashIsCorrect) {
  const std::vector<Word> words = {
    Word(),
    Word({0}),
    Word({1}),
    Word({0,0}),
    Word({1,0}),
    Word({0,1}),
    Word({0,0,0,0}),
  };
  EXPECT_EQ(
    num_distinct_elements(mapped(words, [](const Word& w) {
      return std::hash<Word>()(w);
    })),
    words.size()
  );
}

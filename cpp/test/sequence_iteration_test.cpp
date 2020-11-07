#include "lib/sequence_iteration.h"

#include "gtest/gtest.h"

#include "test_util/matchers.h"


TEST(SequenceIterationTest, AllSequences) {
  EXPECT_EQ(
    all_sequences(7, 0),
    (std::vector<std::vector<int>>{
      {},
    })
  );

  EXPECT_EQ(
    all_sequences(2, 3),
    (std::vector<std::vector<int>>{
      {0, 0, 0},
      {0, 0, 1},
      {0, 1, 0},
      {0, 1, 1},
      {1, 0, 0},
      {1, 0, 1},
      {1, 1, 0},
      {1, 1, 1},
    })
  );

  EXPECT_EQ(
    all_sequences(3, 2),
    (std::vector<std::vector<int>>{
      {0, 0},
      {0, 1},
      {0, 2},
      {1, 0},
      {1, 1},
      {1, 2},
      {2, 0},
      {2, 1},
      {2, 2},
    })
  );
}

TEST(SequenceIterationTest, IncreasingSequencesFixedLength) {
  EXPECT_EQ(
    increasing_sequences(3, 4),
    std::vector<std::vector<int>>{}
  );

  EXPECT_EQ(
    increasing_sequences(3, 2),
    (std::vector<std::vector<int>>{
      {0, 1},
      {0, 2},
      {1, 2},
    })
  );

  EXPECT_EQ(
    increasing_sequences(4, 2),
    (std::vector<std::vector<int>>{
      {0, 1},
      {0, 2},
      {0, 3},
      {1, 2},
      {1, 3},
      {2, 3},
    })
  );

  EXPECT_EQ(
    increasing_sequences(4, 3),
    (std::vector<std::vector<int>>{
      {0, 1, 2},
      {0, 1, 3},
      {0, 2, 3},
      {1, 2, 3},
    })
  );
}

TEST(SequenceIterationTest, IncreasingSequencesVarLength) {
  EXPECT_EQ(
    increasing_sequences(1),
    (std::vector<std::vector<int>>{
      {},
      {0},
    })
  );

  EXPECT_EQ(
    increasing_sequences(3),
    (std::vector<std::vector<int>>{
      {},
      {0},
      {1},
      {2},
      {0, 1},
      {0, 2},
      {1, 2},
      {0, 1, 2},
    })
  );
}

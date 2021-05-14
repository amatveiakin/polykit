#include "lib/sequence_iteration.h"

#include "gtest/gtest.h"

#include "test_util/matchers.h"


std::vector<std::vector<int>> increasing_sequences_naive(int alphabet_size, int length) {
  std::vector<std::vector<int>> ret;
  for (const auto& seq : all_sequences(alphabet_size, length)) {
    bool increasing = true;
    if (seq.size() >= 2) {
      for (int i : range(seq.size() - 1)) {
        if (seq[i] >= seq[i+1]) {
          increasing = false;
          break;
        }
      }
    }
    if (increasing) {
      ret.push_back(seq);
    }
  }
  return ret;
};

std::vector<std::vector<int>> nondecreasing_sequences_naive(int alphabet_size, int length) {
  std::vector<std::vector<int>> ret;
  for (const auto& seq : all_sequences(alphabet_size, length)) {
    bool nondecreasing = true;
    if (seq.size() >= 2) {
      for (int i : range(seq.size() - 1)) {
        if (seq[i] > seq[i+1]) {
          nondecreasing = false;
          break;
        }
      }
    }
    if (nondecreasing) {
      ret.push_back(seq);
    }
  }
  return ret;
};

TEST(SequenceIterationTest, AllSequencesGolden) {
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

TEST(SequenceIterationTest, IncreasingSequencesFixedLengthGolden) {
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

TEST(SequenceIterationTest, IncreasingSequencesVarLengthGolden) {
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

TEST(SequenceIterationTest, NondecreasingSequencesGolden) {
  EXPECT_EQ(
    nondecreasing_sequences(7, 0),
    (std::vector<std::vector<int>>{
      {},
    })
  );

  EXPECT_EQ(
    nondecreasing_sequences(3, 2),
    (std::vector<std::vector<int>>{
      {0, 0},
      {0, 1},
      {0, 2},
      {1, 1},
      {1, 2},
      {2, 2},
    })
  );

  EXPECT_EQ(
    nondecreasing_sequences(3, 3),
    (std::vector<std::vector<int>>{
      {0, 0, 0},
      {0, 0, 1},
      {0, 0, 2},
      {0, 1, 1},
      {0, 1, 2},
      {0, 2, 2},
      {1, 1, 1},
      {1, 1, 2},
      {1, 2, 2},
      {2, 2, 2},
    })
  );
}

TEST(SequenceIterationTest, LARGE_IncreasingSequencesFixedLengthAgainstNaive) {
  for (int alphabet_size : range(1, 5)) {
    for (int length : range(5)) {
      EXPECT_EQ(
        increasing_sequences(alphabet_size, length),
        increasing_sequences_naive(alphabet_size, length)
      );
    }
  }
}

TEST(SequenceIterationTest, LARGE_NondecreasingSequencesAgainstNaive) {
  for (int alphabet_size : range(1, 5)) {
    for (int length : range(5)) {
      EXPECT_EQ(
        nondecreasing_sequences(alphabet_size, length),
        nondecreasing_sequences_naive(alphabet_size, length)
      );
    }
  }
}

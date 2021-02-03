#include "sequence_iteration.h"

#include "absl/algorithm/container.h"

#include "check.h"
#include "util.h"


std::vector<std::vector<int>> all_sequences(int alphabet_size, int length) {
  CHECK_GE(length, 0);
  if (length == 0) {
    return {{}};
  }
  CHECK_GT(alphabet_size, 0);
  std::vector<std::vector<int>> ret;
  std::vector<int> seq(length, 0);
  while (true) {
    ret.push_back(seq);
    auto it = seq.rbegin();
    while (true) {
      if (*it < alphabet_size - 1) {
        ++(*it);
        break;
      } else {
        *it = 0;
      }
      ++it;
      if (it == seq.rend()) {
        return ret;
      }
    }
  }
};

// TODO: Optimized generator; use this impl for testing
std::vector<std::vector<int>> increasing_sequences(int alphabet_size, int length) {
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

// TODO: Deduplicate code
// TODO: Optimized generator; use this impl for testing
std::vector<std::vector<int>> nondecreasing_sequences(int alphabet_size, int length) {
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

// TODO: Optimized generator; use this impl for testing
std::vector<std::vector<int>> increasing_sequences(int alphabet_size) {
  std::vector<std::vector<int>> ret;
  for (int length : range_incl(alphabet_size)) {
    append_vector(ret, increasing_sequences(alphabet_size, length));
  }
  return ret;
}

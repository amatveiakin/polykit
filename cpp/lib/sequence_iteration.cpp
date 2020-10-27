#include "sequence_iteration.h"

#include "absl/algorithm/container.h"

#include "check.h"
#include "util.h"


std::vector<std::vector<int>> all_squences(int alphabet_size, int length) {
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
std::vector<std::vector<int>> increasing_squences(int alphabet_size, int length) {
  std::vector<std::vector<int>> ret;
  for (const auto& seq : all_squences(alphabet_size, length)) {
    bool increasing = true;
    if (seq.size() >= 2) {
      for (int i = 0; i < seq.size()-1; ++i) {
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

// TODO: Optimized generator; use this impl for testing
std::vector<std::vector<int>> increasing_squences(int alphabet_size) {
  std::vector<std::vector<int>> ret;
  for (int length = 0; length <= alphabet_size; ++length) {
    append_vector(ret, increasing_squences(alphabet_size, length));
  }
  return ret;
}

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

static void increment_tail(std::vector<int>& v, int from) {
  for (int i : range(from, v.size())) {
    ++v[i];
  }
}

static void tail_increasing_sequences(
    int alphabet_size, std::vector<int> seq, int start_from, std::vector<std::vector<int>>& ret) {
  if (start_from == seq.size() - 1) {
    while (seq.back() < alphabet_size) {
      ret.push_back(seq);
      ++seq.back();
    }
  } else {
    while (seq.back() < alphabet_size) {
      tail_increasing_sequences(alphabet_size, seq, start_from + 1, ret);
      increment_tail(seq, start_from);
    }
  }
}

std::vector<std::vector<int>> increasing_sequences(int alphabet_size, int length) {
  CHECK_GE(length, 0);
  if (length == 0) {
    return {{}};
  }
  CHECK_GT(alphabet_size, 0);
  if (length > alphabet_size) {
    return {};
  }
  std::vector<std::vector<int>> ret;
  std::vector<int> seq(length, 0);
  absl::c_iota(seq, 0);
  tail_increasing_sequences(alphabet_size, seq, 0, ret);
  return ret;
};

std::vector<std::vector<int>> nondecreasing_sequences(int alphabet_size, int length) {
  CHECK_GE(length, 0);
  if (length == 0) {
    return {{}};
  }
  CHECK_GT(alphabet_size, 0);
  std::vector<std::vector<int>> ret;
  std::vector<int> seq(length, 0);
  tail_increasing_sequences(alphabet_size, seq, 0, ret);
  return ret;
};

std::vector<std::vector<int>> increasing_sequences(int alphabet_size) {
  std::vector<std::vector<int>> ret;
  for (int length : range_incl(alphabet_size)) {
    append_vector(ret, increasing_sequences(alphabet_size, length));
  }
  return ret;
}

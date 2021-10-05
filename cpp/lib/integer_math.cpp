#include "integer_math.h"

#include "check.h"
#include "util.h"


// Optimization potential:
//   - precompute
//   - memoize
//   - simply multiply (be careful about overflows!)
//   - invert k when it's greater than n/2, multiply when k is small.
int binomial(int n, int k) {
  if (k > n) {
    return 0;
  }
  if (k == 0 || k == n) {
    return 1;
  }
  return binomial(n - 1, k - 1) + binomial(n - 1, k);
}

static void fill_partitions(
  std::vector<int>& buffer,
  int remainder,
  std::vector<std::vector<int>>& result
) {
  if (remainder == 0) {
    result.push_back(buffer);
  } else {
    int max_value = remainder;
    if (!buffer.empty()) {
      // Always generate decreasing sequences to avoid duplicates.
      max_value = std::min(max_value, buffer.back());
    }
    for (int summand = max_value; summand >= 1; --summand) {
      buffer.push_back(summand);
      fill_partitions(buffer, remainder - summand, result);
      buffer.pop_back();
    }
  }
}

// Optimization potential: precompute.
std::vector<std::vector<int>> get_partitions(int n) {
  CHECK_LT(0, n);
  std::vector<std::vector<int>> ret;
  std::vector<int> buffer;
  fill_partitions(buffer, n, ret);
  CHECK(buffer.empty());
  return ret;
}

// Optimization potential: discard longer sequences on the fly.
// Optimization potential: precompute.
std::vector<std::vector<int>> get_partitions(int n, int num_summands) {
  CHECK_LT(0, n);
  CHECK_LT(0, num_summands);
  return filtered(get_partitions(n), [&](const auto& seq) {
    return seq.size() == num_summands;
  });
}

#include "lyndon.h"


// Optimization potential: Cache (remember about thread-safety!)
std::vector<std::vector<int>> get_lyndon_words(int alphabet_size, int length) {
  CHECK_LE(2, alphabet_size);
  CHECK_LE(0, length);
  if (length == 0) {
    return {{}};
  }
  std::vector<std::vector<int>> words = {};
  const auto add_word = [&](const std::vector<int>& w) {
    // Note: would return all Lyndon words of length <= `length` if added unconditionally.
    if (w.size() == length) {
      words.push_back(w);
    }
  };
  std::vector<int> last_w = {0};
  add_word(last_w);
  while (true) {
    std::vector<int> w = mapped(range(length), [&](int i) {
      return last_w[i % last_w.size()];
    });
    while (!w.empty() && w.back() == alphabet_size - 1) {
      w.pop_back();
    }
    if (w.empty()) {
      break;
    }
    ++w.back();
    add_word(w);
    last_w = std::move(w);
  }
  return words;
}

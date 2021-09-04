#include "lyndon.h"


// alphabet_size -> length -> words
static absl::flat_hash_map<int, absl::flat_hash_map<int, std::vector<std::vector<int>>>> cache;

std::vector<std::vector<int>> generate_lyndon_words(int alphabet_size, int max_length) {
  CHECK_LE(2, alphabet_size);
  CHECK_LE(0, max_length);
  std::vector<std::vector<int>> words = {{}};
  if (max_length == 0) {
    return words;
  }
  std::vector<int> last_w = {0};
  words.push_back(last_w);
  while (true) {
    std::vector<int> w = mapped(range(max_length), [&](int i) {
      return last_w[i % last_w.size()];
    });
    while (!w.empty() && w.back() == alphabet_size - 1) {
      w.pop_back();
    }
    if (w.empty()) {
      break;
    }
    ++w.back();
    words.push_back(w);
    last_w = std::move(w);
  }
  return words;
}

const std::vector<std::vector<int>>& get_lyndon_words(int alphabet_size, int length) {
  if (const auto outer_it = cache.find(alphabet_size); outer_it != cache.end()) {
    const auto& inner = outer_it->second;
    if (const auto inner_it = inner.find(length); inner_it != inner.end()) {
      return inner_it->second;
    }
  }
  auto& words_in_alphabet = cache[alphabet_size];
  words_in_alphabet.clear();
  // Optimization potential: pre-generate longer words, like std::vector pre-allocates on `push_back`.
  for (auto&& word : generate_lyndon_words(alphabet_size, length)) {
    const int length = word.size();
    words_in_alphabet[length].push_back(std::move(word));
  }
  return words_in_alphabet[length];
}

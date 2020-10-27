#pragma once

#include <array>
#include <cstring>
#include <string>
#include <vector>

#include "absl/algorithm/container.h"
#include "absl/types/span.h"

#include "hash.h"
#include "linear.h"


constexpr int kWordStorageSize = 16;
constexpr int kMaxWordSize = kWordStorageSize - 1;  // 1 for length
constexpr int kWordAlphabetSize = std::numeric_limits<unsigned char>::max() + 1;

// Inline vector<int>. Small and well-suited to work as hash table key.
class Word {
public:
  using DataT = std::array<unsigned char, kWordStorageSize>;

  Word() {
    data_.fill(0);
  }
  template<typename T>
  Word(const T& src_begin, const T& src_end)
    : Word()
  {
    write_size(std::distance(src_begin, src_end));
    auto word_it = begin();
    for (auto it = src_begin; it != src_end; ++it) {
      // Optimization potential: disable the check in release.
      CHECK_LT(*it, kWordAlphabetSize);
      *word_it = *it;
      ++word_it;
    }
    // std::copy(src_begin, src_end, begin());
  }
  Word(std::initializer_list<int> data)
    : Word(data.begin(), data.end()) {}
  explicit Word(absl::Span<const unsigned char> data)
    : Word(data.begin(), data.end()) {}

  bool empty() const { return size() == 0; }
  int size() const { return data_[0]; }

  int operator[](int idx) const { return *(begin() + idx); }

  int front() const { return *begin(); }
  int back() const { return *std::prev(end()); }

  int pop_back() {
    const auto it = std::prev(end());
    const int value = *it;
    write_size(size() - 1);
    *it = 0;
    return value;
  }
  void push_back(int ch) {
    write_size(size() + 1);
    *std::prev(end()) = ch;
  }
  void append_word(const Word& other) {
    const auto it = end();
    write_size(size() + other.size());
    absl::c_copy(other, it);
  }

  DataT::iterator begin() { return data_.begin() + kDataStart; }
  DataT::iterator end() { return begin() + size(); }

  DataT::const_iterator begin() const { return data_.begin() + kDataStart; }
  DataT::const_iterator end() const { return begin() + size(); }

  absl::Span<const unsigned char> span() const {
    return absl::Span(data_.data() + kDataStart, size());
  }

  bool operator==(const Word& other) const {
    return data_ == other.data_;
  }
  bool operator!=(const Word& other) const {
    return data_ != other.data_;
  }
  bool operator<(const Word& other) const {
    return data_ < other.data_;
  }

private:
  static constexpr int kDataStart = 1;

  friend std::hash<Word>;

  void write_size(int new_size) {
    CHECK_LE(0, new_size);
    CHECK_LE(new_size, kMaxWordSize);
    data_[0] = new_size;
  }

  // Data model
  //   [0] - length
  //   [1]..[length] - content
  //   everything else = 0   <- important! (otherwise opeartor== will break)
  DataT data_;
};

template<typename F>
std::string to_string(const Word& w, F char_to_string) {
  return list_to_string(w, char_to_string);
}

inline std::string to_string(const Word& w) {
  return to_string(w, [](auto c){ return to_string(c); });
}

inline Word concat_words(const Word& w1, const Word& w2) {
  Word ret;
  ret.append_word(w1);
  ret.append_word(w2);
  return ret;
}

namespace std {
  template <>
  struct hash<Word> {
    size_t operator()(Word const& w) const noexcept { return hash_array(w.data_); }
  };
}

namespace internal {
struct WordExprParam : SimpleLinearParam<Word> {
  static StorageT monom_tensor_product(const StorageT& lhs, const StorageT& rhs) {
    return concat_words(lhs, rhs);
  }
  static int object_to_weight(const ObjectT& obj) {
    return obj.size();
  }
  static StorageT shuffle_preprocess(const StorageT& key) {
    return key;
  }
  static StorageT shuffle_postprocess(const StorageT& key) {
    return key;
  }
};
}

using WordExpr = Linear<internal::WordExprParam>;

inline int distinct_chars(Word word) {
  std::sort(word.begin(), word.end());
  return std::unique(word.begin(), word.end()) - word.begin();
}

template<typename LinearT>
LinearT terms_with_min_distinct_elements(const LinearT& expr, int min_distinct) {
  static_assert(std::is_same_v<typename LinearT::StorageT, Word>);
  return expr.filtered_key([&](const Word& key) {
    return distinct_chars(key) >= min_distinct;
  });
}

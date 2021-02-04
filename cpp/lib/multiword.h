// DEPRECATED[word-to-pvector]. Keeping for now to allow comparison benchmarks.

#pragma once

#include <array>
#include <cstring>
#include <string>
#include <vector>

#include "absl/types/span.h"

#include "coalgebra.h"
#include "hash.h"
#include "linear.h"
#include "word.h"


constexpr int kMultiWordMaxSegments = 7;
constexpr int kMultiWordStorageSize = 32;  // reasonable default
// constexpr int kMultiWordStorageSize = 64;
constexpr int kMaxMultiWordDataSize =
    kMultiWordStorageSize - kMultiWordMaxSegments - 1;  // 1 for num segments
constexpr int kMultiWordAlphabetSize = std::numeric_limits<unsigned char>::max() + 1;

// Inline vector<vector<int>>. Small and well-suited to work as hash table key.
class MultiWord {
public:
  using SegmentRef = absl::Span<const unsigned char>;

  class const_iterator {
  public:
    const_iterator() {}

    bool operator==(const const_iterator& other) const {
      return word_ == other.word_ && segment_idx_ == other.segment_idx_;
    }
    bool operator!=(const const_iterator& other) const {
      return !(*this == other);
    }

    SegmentRef operator*() const {
      return word_->segment(segment_idx_);
    }

    const_iterator& operator++() {
      segment_idx_++;
      return *this;
    }
    const_iterator operator++(int) {
      const_iterator old_value(*this);
      operator++();
      return old_value;
    }

  private:
    const_iterator(const MultiWord* word, int segment_idx)
      : word_(word), segment_idx_(segment_idx) {}

    friend class MultiWord;

    const MultiWord* word_ = nullptr;
    int segment_idx_ = 0;
  };

  using DataT = std::array<unsigned char, kMultiWordStorageSize>;

  MultiWord() {
    data_.fill(0);
  }
  MultiWord(absl::Span<const std::vector<unsigned char>> segments) : MultiWord() {
    for (const auto& s : segments) { append_segment(s); }
  }
  MultiWord(absl::Span<const absl::Span<const unsigned char>> segments) : MultiWord() {
    for (const auto& s : segments) { append_segment(s); }
  }
  MultiWord(absl::Span<const absl::Span<const int>> segments) : MultiWord() {
    for (const auto& s : segments) { append_segment(s); }
  }
  MultiWord(std::initializer_list<std::initializer_list<int>> segments) : MultiWord() {
    for (const auto& s : segments) { append_segment(s); }
  }

  bool empty() const { return num_segments() == 0; }
  int num_segments() const { return data_[0]; }
  int size() const { return num_segments(); }

  void append_segment(const absl::Span<const unsigned char>& w) {
    append_segment_impl(w);
  }
  void append_segment(const absl::Span<const int>& w) {
    append_segment_impl(w);
  }
  void append_segment(const Word& w) {
    append_segment(w.span());
  }

  SegmentRef segment(int idx) const {
    CHECK_LT(idx, kMultiWordMaxSegments);
    const int start = get_segment_start(idx);
    const int end = data_[kSegmentInfoStart + idx];
    return absl::MakeConstSpan(data_).subspan(start, end - start);
  }
  SegmentRef operator[](int idx) const { return segment(idx); }

  const_iterator begin() const {
    return const_iterator(this, 0);
  }
  const_iterator end() const {
    return const_iterator(this, num_segments());
  }

  bool operator==(const MultiWord& other) const {
    return data_ == other.data_;
  }
  bool operator!=(const MultiWord& other) const {
    return data_ != other.data_;
  }
  bool operator<(const MultiWord& other) const {
    return data_ < other.data_;
  }

private:
  friend std::hash<MultiWord>;

  static constexpr int kSegmentInfoStart = 1;
  static constexpr int kDataStart = kSegmentInfoStart + kMultiWordMaxSegments;

  void write_num_segments(int new_num) {
    CHECK_LE(0, new_num);
    CHECK_LE(new_num, kMultiWordMaxSegments);
    data_[0] = new_num;
  }
  int get_segment_start(int idx) const {
    return idx == 0 ? kDataStart : data_[kSegmentInfoStart + idx - 1];
  }

  template<typename T>
  void append_segment_impl(const absl::Span<const T>& w) {
    const int segment_idx = num_segments();
    write_num_segments(segment_idx + 1);
    const int segment_start = get_segment_start(segment_idx);
    const int segment_end = segment_start + w.size();
    CHECK_LT(segment_end, kMultiWordStorageSize);
    data_[kSegmentInfoStart + segment_idx] = segment_end;
    std::copy(w.begin(), w.end(), data_.begin() + segment_start);
  }

  // Data model
  //   [0] - num segments
  //   [1]..[kMultiWordMaxSegments-1] - segment ends
  //   [kMultiWordMaxSegments]..[?] - content
  //   everything else = 0   <- important! (otherwise opeartor== will break)
  DataT data_;
};

inline std::string to_string(const MultiWord& w) {
  std::vector<std::string> segment_strings;
  for (const auto& segment : w) {
    segment_strings.push_back(fmt::parens(str_join(segment, ", ")));
  }
  return fmt::parens(str_join(segment_strings, ", "));
}

namespace std {
  template <>
  struct hash<MultiWord> {
    size_t operator()(MultiWord const& w) const noexcept { return hash_array(w.data_); }
  };
}


namespace internal {
struct WordCoExprParam : SimpleLinearParam<MultiWord> {
  static std::string object_to_string(const MultiWord& word) {
    std::vector<std::string> segment_strings;
    for (const auto& segment : word) {
      segment_strings.push_back(fmt::parens(str_join(segment, ",")));
    }
    return str_join(segment_strings, fmt::coprod_lie());
  }
  static constexpr bool coproduct_is_lie_algebra = true;
};
}  // namespace internal

using WordCoExpr = Linear<internal::WordCoExprParam>;

// Explicit rules allow to omit template types when calling the function.
inline WordCoExpr coproduct(const WordExpr& lhs, const WordExpr& rhs) {
  return coproduct<WordCoExpr>(lhs, rhs);
}
inline WordCoExpr comultiply(const WordExpr& expr, std::pair<int, int> form) {
  return comultiply<WordCoExpr>(expr, form);
}

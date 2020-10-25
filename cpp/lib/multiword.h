#pragma once

#include <array>
#include <cstring>
#include <string>
#include <vector>

#include "absl/types/span.h"

#include "hash.h"
#include "linear.h"
#include "word.h"


constexpr int kMultiWordMaxSegments = 5;
constexpr int kMultiWordStorageSize = 16;
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
    // SegmentRef* operator->() const {
    //   return &**this;  // TODO: fix without returning reference to remporary
    // }

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
  MultiWord(std::vector<std::vector<unsigned char>> segments) : MultiWord() {
    for (const auto& s : segments) {
      append_segment(s);
    }
  }

  bool empty() const { return num_segments() == 0; }
  int num_segments() const { return data_[0]; }

  void append_segment(const absl::Span<const unsigned char>& w) {
    const int segment_idx = num_segments();
    write_num_segments(segment_idx + 1);
    const int segment_start = get_segment_start(segment_idx);
    const int segment_end = segment_start + w.size();
    CHECK_LT(segment_end, kMultiWordStorageSize);
    data_[kSegmentInfoStart + segment_idx] = segment_end;
    std::copy(w.begin(), w.end(), data_.begin() + segment_start);
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

  // Data model
  //   [0] - num segments
  //   [1]..[kMultiWordMaxSegments-1] - segment ends
  //   [kMultiWordMaxSegments]..[?] - content
  //   everything else = 0   <- important! (otherwise opeartor== will break)
  DataT data_;
};

template<typename F>
std::string to_string(const MultiWord& w, F char_to_string) {
  std::vector<std::string> segment_strings;
  for (const auto& segment : w) {
    segment_strings.push_back(list_to_string(segment));
  }
  return list_to_string(segment_strings, char_to_string);
}

inline std::string to_string(const MultiWord& w) {
  return to_string(w, [](auto c){ return to_string(c); });
}

namespace std {
  template <>
  struct hash<MultiWord> {
    size_t operator()(MultiWord const& w) const noexcept { return hash_array(w.data_); }
  };
}
#pragma once

#include <array>
#include <cassert>
#include <cstring>
#include <string>
#include <vector>


constexpr int kMaxWordSize = 15;
constexpr int kWordStorageSize = kMaxWordSize + 1;  // +1 for length
constexpr int kWordAlphabetSize = std::numeric_limits<unsigned char>::max() + 1;

// Should be small, as it's going to be used as hash table key.
class IntWord {
public:
  using DataT = std::array<unsigned char, kWordStorageSize>;
  
  IntWord() {
    data_.fill(0);
  }
  template<typename T>
  IntWord(const T& src_begin, const T& src_end)
    : IntWord()
  {
    // TODO: Add assertion that each char is less than kWordAlphabetSize in debug mode
    write_size(std::distance(src_begin, src_end));
    std::copy(src_begin, src_end, begin());
  }
  IntWord(std::initializer_list<int> data)
    : IntWord(data.begin(), data.end()) {}

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

  bool operator==(const IntWord& other) const {
    return data_ == other.data_;
  }
  bool operator!=(const IntWord& other) const {
    return data_ != other.data_;
  }
  bool operator<(const IntWord& other) const {
    return data_ < other.data_;
  }

  DataT::iterator begin() { return std::next(data_.begin()); }
  DataT::iterator end() { return begin() + size(); }

  DataT::const_iterator begin() const { return std::next(data_.begin()); }
  DataT::const_iterator end() const { return begin() + size(); }

  template<typename F>
  std::string to_string(F char_to_string) const {
    return list_to_string(*this, char_to_string);
  }
  std::string to_string() const {
    return to_string([](DataT::value_type c){ return std::to_string(c); });
  }

private:
  friend IntWord concat_words(const IntWord&, const IntWord&);
  friend std::hash<IntWord>;

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
std::string to_string(IntWord w, F char_to_string) {
  return list_to_string(w, char_to_string);
}

inline std::string to_string(IntWord w) {
  return to_string(w, [](auto c){ return to_string(c); });
}

inline IntWord concat_words(const IntWord& w1, const IntWord& w2) {
  IntWord ret;
  ret.write_size(w1.size() + w2.size());
  auto it = ret.begin();
  it = std::copy(w1.begin(), w1.end(), it);
  it = std::copy(w2.begin(), w2.end(), it);
  return ret;
}

template <class T>
inline void hash_combine(size_t& hash, const T& new_hash) {
  hash ^= new_hash + 0x9e3779b9 + (hash << 6) + (hash >> 2);
}

namespace std {
  template <>
  // TODO: Test
  struct hash<IntWord> {
    size_t operator()(IntWord const& w) const noexcept {
      static_assert(kWordStorageSize % sizeof(size_t) == 0);
      constexpr int kWordStorageSizeInHashT = kWordStorageSize / sizeof(size_t);
      std::array<size_t, kWordStorageSizeInHashT> as_hash_t;
      std::memcpy(as_hash_t.data(), w.data_.data(), kWordStorageSize);
      size_t ret = 0;
      for (const size_t v : as_hash_t) {
        hash_combine(ret, std::hash<size_t>()(v));
      }
      return ret;
    }
  };
}

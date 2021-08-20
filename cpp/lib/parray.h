// A fixed-size array that can compactly store two 4-bit values per byte.

// Optimization potential: make uint4_t range CHECKs debug-only.

#include <array>
#include <limits>

#include "absl/types/span.h"

#include "bit.h"
#include "check.h"


enum class uint4_t : unsigned char;

namespace std {
template<>
struct numeric_limits<uint4_t> {
  static constexpr uint4_t min() noexcept    { return static_cast<uint4_t>(0); }
  static constexpr uint4_t max() noexcept    { return static_cast<uint4_t>((1 << 4) - 1); }
  static constexpr uint4_t lowest() noexcept { return static_cast<uint4_t>(0); }
  static constexpr int radix = 2;
};
}  // namespace std

inline bool is_uint4_value_ok(int value) {
  return 0 <= value && value <= static_cast<int>(std::numeric_limits<uint4_t>::max());
}

inline uint4_t to_uint4_unsafe(int value) {
  return static_cast<uint4_t>(value);
}

inline uint4_t to_uint4(int value) {
  CHECK(is_uint4_value_ok(value)) << value;
  return static_cast<uint4_t>(value);
}

inline int to_integer(uint4_t value) {
  return static_cast<int>(value);
}


// The primary goal of this class is to provide PArray<uint4_t> specialization
// (and potentially others). The generic version offers no benefit over std::array.
// It is implemented as a polymorphic type in order to allow conditional packing.
template<typename T, int N>
class PArray {
  static_assert(N > 0);

public:
  explicit PArray(absl::Span<const T> data) {
    CHECK_EQ(data.size(), N);
    absl::c_copy(data, elements_.begin());
  }

  bool empty() const { return elements_.empty(); }
  int size() const { return elements_.size(); }

  const T& operator[](int idx) const { return elements_[idx]; }

  // Returns the content as an array. Subclasses may return temporary objects instead
  // of a const reference. Suggested usage in template code:
  //   const auto& v = parray.unpacked();
  // This way, there will be no copying in the general case. The temporaries will be
  // properly handled as well since binding to a const reference extends lifetime:
  // https://herbsutter.com/2008/01/01/gotw-88-a-candidate-for-the-most-important-const/
  const std::array<T, N>& unpacked() const { return elements_; }

  template<typename OutT>
  std::array<OutT, N> mapped() const {
    return mapped(elements_, [](const T& value) { return OutT(value); });
  }

  bool operator==(const PArray& other) { return elements_ == other.elements_; }
  bool operator!=(const PArray& other) { return elements_ != other.elements_; }
  bool operator< (const PArray& other) { return elements_ <  other.elements_; }
  bool operator<=(const PArray& other) { return elements_ <= other.elements_; }
  bool operator> (const PArray& other) { return elements_ >  other.elements_; }
  bool operator>=(const PArray& other) { return elements_ >= other.elements_; }

private:
  std::array<T, N> elements_;
};


template<int N>
class PArray<uint4_t, N> {
  static_assert(N > 0);

public:
  explicit PArray(absl::Span<const int> data) : PArray(ConstructorImpl{}, data) {}
  explicit PArray(absl::Span<const unsigned char> data) : PArray(ConstructorImpl{}, data) {}
  explicit PArray(std::initializer_list<int> data) : PArray(ConstructorImpl{}, absl::Span<const int>(data)) {}

  bool empty() const { return size() == 0; }
  int size() const { return N; }

  int operator[](int idx) const {
    const auto [x, y] = unpack_pair(bytes_[idx / 2]);
    return idx % 2 == 0 ? x : y;
  }

  std::array<unsigned char, N> unpacked() const { return mapped<unsigned char>(); }

  template<typename OutT>
  std::array<OutT, N> mapped() const {
    std::array<OutT, N> ret;
    int dst = 0;
    for (std::byte pack : bytes_) {
      const auto [x, y] = unpack_pair(pack);
      ret[dst] = x;
      if (dst+1 < N) {
        ret[dst+1] = y;
      }
      dst += kValuesPerByte;
    }
    return ret;
  }

  bool operator==(const PArray& other) { return bytes_ == other.bytes_; }
  bool operator!=(const PArray& other) { return bytes_ != other.bytes_; }
  bool operator< (const PArray& other) { return bytes_ <  other.bytes_; }
  bool operator<=(const PArray& other) { return bytes_ <= other.bytes_; }
  bool operator> (const PArray& other) { return bytes_ >  other.bytes_; }
  bool operator>=(const PArray& other) { return bytes_ >= other.bytes_; }

private:
  struct ConstructorImpl {};

  template<typename T>
  explicit PArray(ConstructorImpl, absl::Span<const T> data) {
    static_assert(std::is_integral_v<T>);
    CHECK_EQ(data.size(), N);
    CHECK(absl::c_all_of(data, is_uint4_value_ok)) << dump_to_string(data);
    int dst = 0;
    for (int src = 0; src < data.size(); src += kValuesPerByte) {
      const int a = data[src];
      // Optimization potential: Replace with a compile-time check for N oddity
      const int b = (src+1 < data.size()) ? data[src+1] : 0;
      bytes_[dst] = std::byte((a << kShift) | b);
      ++dst;
    }
  }

  static constexpr int kValuesPerByte = 2;  // Note: some code implicitly assumes it's 2
  static constexpr int kShift = 4;
  static constexpr int kLowerValueMask = (1 << kShift) - 1;
  static constexpr int kSizeInBytes = (N + kValuesPerByte - 1) / kValuesPerByte;
  static_assert(kValuesPerByte * kShift <= CHAR_BIT);

  static std::pair<int, int> unpack_pair(std::byte compressed) {
    const int v = std::to_integer<int>(compressed);
    return {v >> kShift, v & kLowerValueMask};
  }

  std::array<std::byte, kSizeInBytes> bytes_;
};

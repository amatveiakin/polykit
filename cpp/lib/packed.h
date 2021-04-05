// Compact continous containers. Specialized alternatives for std::vector.

// Optimization potential: replace absl::InlineVector with a custom implementation
//   that uses a 1-byte inline size and occupies 16 bytes (rather than 24) on x64.
// Optimization potential: add uint_4 support in inlined vector.
// Optimization potential: pack nested inlined vectors in one continuous storage
//   to allow memcpy/memcmp.
// Optimization potential: serialize data (e.g. via ProtocolBuffers or FlatBuffers)
//   if nesting level is high, but working in key space is unimportant.
// Optimization potential: make uint4_t range CHECKs debug-only.

#pragma once

#include <array>
#include <climits>
#include <cstddef>
#include <limits>

#include "absl/algorithm/container.h"
#include "absl/container/inlined_vector.h"
#include "absl/types/span.h"

#include "bit.h"
#include "check.h"
#include "string.h"


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


namespace internal {
// On choosing PVector max size. PVector is designed to be used as Linear storage
// type, meaning it should be optimized for using as absl::flat_hash_map key.
// This brings two considerations:
//   - The most performance-critical operation on PVector is `operator==`,
//     because it is invoked each times linear expressions are added
//     (note: I haven't benchmarked this, but this is my strong intuition).
//     Therefore it's desirable to stay withing inline representation as much
//     as possible to reduce the amount of indirection in `operator==`.
//   - Another critical operation is move. Unlike std::unordered_hash_set,
//     absl::flat_hash_map stores keys directly inside, meaning that it has to
//     move all keys every time the map is re-hashed. Therefore it's desirable
//     to keep PVector small.
//
// Optimization potential: tune this value and benchmark against real use-cases.
constexpr int kMaxPVectorSize = 64;

template<typename T, int N>
struct should_use_inlined_vector {
  static constexpr bool value = sizeof(absl::InlinedVector<T, N>) < kMaxPVectorSize;
};
static_assert(
  internal::should_use_inlined_vector<char, 1>::value,
  "PVector wouldn't ever use an inlined representation"
);

template<typename T, int N, typename Enable = void>
struct inlined_vector_size {
  static constexpr int value = N;
};
template<typename T, int N>
struct inlined_vector_size<
    T, N,
    std::enable_if_t<sizeof(absl::InlinedVector<T, N + 1>) == sizeof(absl::InlinedVector<T, N>)>
> {
  static constexpr int value = inlined_vector_size<T, N + 1>::value;
};

template<typename T, int N>
using pvector_base_type = std::conditional_t<
  should_use_inlined_vector<T, N>::value,
  absl::InlinedVector<T, inlined_vector_size<T, N>::value>,
  std::vector<T>
>;
}  // namespace internal


// The primary goal of this class is to provide PArray<uint4_t> specialization
// (and potentially others). The generic version offers no benefit over std::array.
// However it is written as a polymorphic type in order to allow conditional
// packing.
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
      bytes_[dst] = std::byte((a << kShift) + b);
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


// TODO: Private inheritance + re-export methods manually to make sure interfaces are the same.
// Optimization potential: add a togglable PVector stats collector to know how many
//   heap representations were used; destructor is probably the right place, since
//   PVectors very rarely shrink.
//   Stretch goal: add PVector tags to distinguish between different expression types.
template<typename T, int N>
class PVector : public internal::pvector_base_type<T, N> {
private:
  using ParentT = internal::pvector_base_type<T, N>;
public:
  static constexpr int inlined_size = N;
  using ParentT::ParentT;
};

template<typename, typename = void> struct is_pvector : std::false_type {};
template<typename T, int N> struct is_pvector<PVector<T, N>> : std::true_type {};
template<typename T> inline constexpr bool is_pvector_v = is_pvector<T>::value;

template<typename T, int N>
std::string dump_to_string_impl(const PVector<T, N>& v) {
  return dump_to_string(static_cast<const internal::pvector_base_type<T, N>&>(v));
}


template<typename Dst, typename In>
auto to_pvector(absl::Span<const In> src) {
  static_assert(is_pvector_v<Dst>);
  Dst dst(src.size());
  absl::c_copy(src, dst.begin());
  return dst;
}
template<int N, typename In>
auto to_pvector(absl::Span<const In> src) {
  return to_pvector<PVector<In, N>>(src);
}

template<typename Dst, typename In>
auto to_pvector(const std::vector<In>& src) {
  return to_pvector<Dst>(absl::MakeConstSpan(src));
}
template<int N, typename In>
auto to_pvector(const std::vector<In>& src) {
  return to_pvector<N>(absl::MakeConstSpan(src));
}

template<typename Dst, typename In, typename F>
auto mapped_to_pvector(absl::Span<const In> src, F&& func) {
  static_assert(is_pvector_v<Dst>);
  Dst dst(src.size());
  absl::c_transform(src, dst.begin(), std::forward<F>(func));
  return dst;
}
template<int N, typename In, typename F>
auto mapped_to_pvector(absl::Span<const In> src, F&& func) {
  return mapped_to_pvector<PVector<std::invoke_result_t<F, In>, N>>(src, std::forward<F>(func));
}

template<typename Dst, typename In, typename F>
auto mapped_to_pvector(const std::vector<In>& src, F&& func) {
  return mapped_to_pvector<Dst>(absl::MakeConstSpan(src), std::forward<F>(func));
}
template<int N, typename In, typename F>
auto mapped_to_pvector(const std::vector<In>& src, F&& func) {
  return mapped_to_pvector<N>(absl::MakeConstSpan(src), std::forward<F>(func));
}


namespace std {
// TODO: Find a solution that doesn't require expanding std, or
// at least move to an appropriate place.
template<size_t N>
bool operator<(const std::bitset<N>& a, const std::bitset<N>& b) {
  if constexpr (N <= sizeof(unsigned long) * CHAR_BIT) {
    return a.to_ulong() < b.to_ulong();
  } else {
    static_assert(N <= sizeof(unsigned long long) * CHAR_BIT, "Bitset too large");
    return a.to_ullong() < b.to_ullong();
  }
}
template<size_t N>
bool operator>(const std::bitset<N>& a, const std::bitset<N>& b) { return b < a; }
template<size_t N>
bool operator<=(const std::bitset<N>& a, const std::bitset<N>& b) { return !(b < a); }
template<size_t N>
bool operator>=(const std::bitset<N>& a, const std::bitset<N>& b) { return !(a < b); }
}  // namespace std

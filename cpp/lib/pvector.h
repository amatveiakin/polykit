// Automatic inlined vector. Main difference from absl::InlinedVector is that PVector
// wouldn't inline if the total object side exceeds certain limit. Optimized for being
// used as a hash map key.

// Optimization potential: replace absl::InlineVector with a custom implementation
//   that uses a 1-byte inline size and occupies 16 bytes (rather than 24) on x64.
// Optimization potential: add uint_4 support in inlined vector.
// Optimization potential: pack nested inlined vectors in one continuous storage
//   to allow memcpy/memcmp.
// Optimization potential: serialize data (e.g. via ProtocolBuffers or FlatBuffers)
//   if nesting level is high, but working in key space is unimportant.

#pragma once

#include <bitset>
#include <climits>
#include <typeindex>

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/inlined_vector.h"
#include "absl/types/span.h"

#include "check.h"
#include "memory_usage_micro.h"
#include "string.h"


namespace internal {
// On choosing PVector max size. PVector is designed to be used as Linear storage
// type, meaning it should be optimized for using as absl::flat_hash_map key.
// This brings two considerations:
//   - The most performance-critical operations on PVector are `operator==` and
//     hashing, because they are invoked every times linear expressions are added
//     (note: I haven't benchmarked this, but this is my strong intuition).
//     Therefore it's desirable to stay withing inline representation as much
//     as possible to reduce the amount of indirection.
//   - Another critical operation is move. Unlike std::unordered_hash_set,
//     absl::flat_hash_map stores keys directly inside, meaning that it has to
//     move all keys every time the map is re-hashed. Therefore it's desirable
//     to keep PVector small.
//
// Optimization potential: tune this value and benchmark against real use-cases.
constexpr int kMaxPVectorSize = 64;

#if DISABLE_PACKING
template<typename T, int N>
struct should_use_inlined_vector {
  static constexpr bool value = false;
};
#else
template<typename T, int N>
struct should_use_inlined_vector {
  static constexpr bool value = sizeof(absl::InlinedVector<T, N>) < kMaxPVectorSize;
};
template<typename T>
struct should_use_inlined_vector<T, 0> {
  static constexpr bool value = false;
};
static_assert(
  internal::should_use_inlined_vector<char, 1>::value,
  "PVector wouldn't ever use an inlined representation"
);
#endif

// Prevent
//   static_assert(N > 0, "`absl::InlinedVector` requires an inlined capacity.");
// error when computing `inlined_vector_size`.
// (It is never used because should_use_inlined_vector<T, 0> is always false.)
template<typename T, int N>
struct sizeof_inlined_vector {
  static constexpr size_t value = sizeof(absl::InlinedVector<T, N>);
};
template<typename T>
struct sizeof_inlined_vector<T, 0> {
  static constexpr size_t value = 0;
};

template<typename T, int N, typename Enable = void>
struct inlined_vector_size {
  static constexpr int value = N;
};
template<typename T, int N>
struct inlined_vector_size<
    T, N,
    std::enable_if_t<sizeof_inlined_vector<T, N + 1>::value == sizeof_inlined_vector<T, N>::value>
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


struct PVectorStats {
  using Key = std::pair<std::string_view, int>;  // (type, inlined size)
  struct Value {
    int64_t num_empty = 0;
    int64_t num_inlined = 0;
    int64_t num_heap = 0;
  };
  absl::flat_hash_map<Key, Value> data;
};

extern PVectorStats pvector_stats;

std::ostream& operator<<(std::ostream& os, const PVectorStats& stats);

// TODO: Private inheritance + re-export methods manually to make sure interfaces are the same.
template<typename T, int N>
class PVector : public internal::pvector_base_type<T, N> {
public:
  static constexpr bool kCanInline = internal::should_use_inlined_vector<T, N>::value;
  static constexpr size_t kInlineSize = internal::inlined_vector_size<T, N>::value;  // meaningful only if kCanInline is true
  using ParentT = internal::pvector_base_type<T, N>;

  using ParentT::ParentT;

  bool is_inlined() const {
    if constexpr (kCanInline) {
      return
        reinterpret_cast<const char*>(this) <= reinterpret_cast<const char*>(this->data()) &&
        reinterpret_cast<const char*>(this->data()) < reinterpret_cast<const char*>(this) + sizeof(PVector)
      ;
    } else {
      return false;
    }
  }
  ~PVector() {
#if PVECTOR_STATS
    // TODO: Add synchronization now that we have `mapped_parallel`.
    auto& value = pvector_stats.data[{get_type_name<PVector>(), kInlineSize}];
    if constexpr (!kCanInline) {
      value.num_inlined = -1;
    }
    if (this->empty()) {
      ++value.num_empty;
    } else if (is_inlined()) {
      ++value.num_inlined;
    } else {
      ++value.num_heap;
    }
#endif
  }

  template <typename H>
  friend H AbslHashValue(H h, const PVector& vec) {
    return H::combine(std::move(h), static_cast<const ParentT&>(vec));
  }
};

template<typename, typename = void> struct is_pvector : std::false_type {};
template<typename T, int N> struct is_pvector<PVector<T, N>> : std::true_type {};
template<typename T> inline constexpr bool is_pvector_v = is_pvector<T>::value;

template<typename T, int N>
std::string dump_to_string_impl(const PVector<T, N>& v) {
  return dump_to_string(static_cast<const internal::pvector_base_type<T, N>&>(v));
}

template<typename T, int N>
MemoryUsage estimated_heap_usage(const PVector<T, N>& v) {
  if (v.is_inlined()) {
    return sum(v, [](const auto& e) { return estimated_heap_usage(e); });
  } else {
    return simple_container_estimated_ram_usage(v, true);
  }
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
  return mapped_to_pvector<PVector<std::decay_t<std::invoke_result_t<F, In>>, N>>(src, std::forward<F>(func));
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
// TODO: Find a solution that doesn't require expanding std.
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

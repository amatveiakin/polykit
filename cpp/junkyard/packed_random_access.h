#pragma once

#include <array>
#include <climits>
#include <cstddef>
#include <limits>

#include "absl/algorithm/container.h"
#include "absl/types/span.h"

#include "check.h"


// General concept. End-type is a POD (in practice: int or bitset),
// everything else is always flattened, so that we can compute equality
// via memcmp and hash quickly.

// Limitations:
//   * Size in bytes is <= 256
//   * Size in elements is <= 256 (note: doesn't follow from above for int4)

// Element types:
//   * fixed size, classic
//   * fixed size, packed (used for ints)
//   * variable size (cannot be packed)


// PVector data model, element is fixed size
//   * size in elements
//   * content

// PVector data model, element is variable size
//   * size in elements
//   * offsets
//   * content


// PArray data model, element is fixed size
//   * content

// PArray data model, element is variable size
//   * offsets
//   * content



template<typename T>
class PVector {
  // Should choose other overloaded for variable-size elements.
  static_assert(PElementTraits<T>::fixed_size);

public:
  PVector() {}

  bool empty() const { return size() == 0; }
  int size() const { return rep.size(); }

  // Note: there is no way to change elements. This would be hard
  // for PVector<uint4_t> and completely break PVector<PVector>.
  const T& operator[](int idx) const { return *(begin() + idx); }

  void pop_back() {
    const int old_size = size();
    CHECK_GT(old_size, 0);
    rep.resize(old_size - 1);
  }
  void push_back(T value) {
    const int old_size = size();
    rep.resize(old_size + 1);
    // Optimization potential: init in place rather than default-init
    // then move-assign.
    rep.data_ptr_as_type()[old_size] = std::move(value);
  }

  // TODO: proper iterator types
  const T* begin() const { return rep.data_ptr_as_type(); }
  const T* end() const { return begin() + size(); }

  bool operator==(const PVector& other) const {
    return rep_.bytes == other.rep_.bytes;
  }

private:
  template<int N>
  struct Rep {
    static constexpr int kMaxSize =
      std::min<int>(internal::kTheoreticalMaxInlineElements, (N - 1) / sizeof(T));
    static_assert(N >= internal::kMinRepSize);

    Rep() {
      bytes.fill(std::byte(0));
      CHECK_EQ(size(), 0);
    }

    const std::byte* data_ptr() const {
      return bytes.data() + 1;
    }
    const T* data_ptr_as_type() const {
      return reinterpret_cast<const T*>(data_ptr());
    }
    int size_in_bytes() const {
      return size() * sizeof(T);
    }
    int size() const {
      return std::to_integer<int>(bytes[0]);
    }
    void resize(int new_size) {
      CHECK_LE(new_size, kMaxSize);
      const int old_size = size();
      if (new_size > old_size) {
        init_values(old_size, new_size);
      } else if (new_size < old_size) {
        delete_values(new_size, old_size);
        std::fill(
          data_ptr() + new_size * sizeof(T),
          data_ptr() + old_size * sizeof(T),
          std::byte(0)
        );
      }
      bytes[0] = std::byte(new_size);
    }
    void delete_values(int idx_begin, int idx_end) {
      T* ptr = data_ptr_as_type();
      T* begin = ptr + idx_begin;
      T* end = ptr + idx_end;
      for (T* it = begin; it != end; ++it) {
        it->~T();
      }
    }
    void init_values(int old_size, int new_size) {
      std::byte* ptr = data_ptr();
      std::byte* begin = ptr + old_size * sizeof(T);
      std::byte* end  = ptr + new_size * sizeof(T);
      for (std::byte* p = begin; p < end; p += sizeof(T)) {
        new(p) T();
      }
    }

    // Data model:
    //   * bytes[0] - size
    //   * bytes[1...X) - data
    //   * bytes[X...) = 0  - important! for operator ==
    //
    // Note: no `alignas(T)`: correct alignment must be ensured manually
    std::array<std::byte, N> bytes;
  };

  Rep<internal::rep_size(sizeof(T))> rep_;
};

template<typename T>
class PVector<PVector<T>> {
public:
  PVector() {}

  bool empty() const { return size() == 0; }
  int size() const { return pre_.num_elements(); }

  PSpan<T> operator[](int idx) const {
    CHECK_LT(idx, num_elements_);
    const int element_size = rep_.element_end(idx) - rep_.element_start(idx);
    const std::byte* element_ptr = rep_.data_ptr() + rep_.element_start(idx);
    return PSpan<T>(element_size, element_ptr);
  }

  void pop_back() {
    rep_.pop_back();
  }
  void push_back(const PVector<T>& vec) {
    rep.push_back_element(vec.rep_.);
  }

  bool operator==(const PVector& other) const {
    return rep_.bytes == other.rep_.bytes;
  }

private:
  template<int N>
  static Rep {
    static_assert(N >= kMinRepSize);

    Rep() {
      bytes.fill(std::byte(0));
      CHECK_EQ(size(), 0);
    }

    int num_elements() const {
      return std::to_integer<int>(bytes[0]);
    }
    void set_num_elements(int new_num) {
      CHECK_LE(new_num, kMaxSize);
      bytes[0] = std::byte(new_size);
    }

    const std::byte* offsets_ptr() const {
      return bytes.data() + 1;
    }
    const std::byte* data_ptr() const {
      return offsets_ptr() + kMaxVariadicVectorElements;
    }
    absl::Span<const std::byte> everything_except_size() const {
      // TODO: This is bad for nested storage: space for unused offsets is wasted!
      const std::byte* data_begin = offsets_ptr();
      const std::byte* data_end = ;
      return { };
    }

    const int element_start(int idx) const {
      CHECK_LT(idx, num_elements());
      return idx == 0 ? 0 : element_end(idx - 1);
    }
    const int element_end(int idx) const {
      CHECK_LT(idx, num_elements());
      return std::to_integer<int>(offset_ptr()[idx]);
    }
    void set_element_end(int idx, int new_end) {
      CHECK_LT(idx, num_elements());
      CHECK_LT(new_end, kMaxElementEnd);
      offset_ptr()[idx] = std::byte(new_end);
    }

    void push_back_element(absl::Span<const std::byte> data) {
      const int old_size = size();
      set_size(old_size + 1);
      const int start = element_start(old_size);
      set_element_end(old_size, start + data.size());
      absl::c_copy(data, data_ptr() + start);
    }

    // Data model:
    //   * <1 byte> - size
    //   * <kMaxVariadicVectorElements bytes> - element offsets
    //   * <everything else> - data
    //
    // Note: no `alignas(T)`: correct alignment must be ensured manually
    std::array<std::byte, N> bytes;
  };

  Rep<internal::variable_element_rep_size(T::expected_total_size)> rep_;
};



template<typename T>
class PSpan<T> {
  // Should choose other overloaded for variable-size elements.
  static_assert(PElementTraits<T>::fixed_size);

public:
  int size() const { return num_elements_; }

  const T& operator[](int idx) const {
    CHECK_LT(idx, num_elements_);
    ptr_as_type()[idx];
  }

private:
  PSpan(int num_elements, const std::byte* ptr)
    : num_elements_(num_elements), ptr_(ptr) {}

  const T* ptr_as_type() const {
    return reinterpret_cast<const T*>(data_ptr());
  }

  int num_elements_ = 0;
  const std::byte* ptr_ = nullptr;
};

template<typename T>
class PSpan<PVector<T>> {
public:
  int size() const { return num_elements_; }

  PSpan<T> operator[](int idx) const {
    CHECK_LT(idx, num_elements_);
    const int element_size = element_end(idx) - element_start(idx);
    const std::byte* element_ptr = data_ptr() + element_start(idx);
    return PSpan<T>(element_size, element_ptr);
  }

private:
  PSpan(int num_elements, const std::byte* ptr)
    : num_elements_(num_elements), ptr_(ptr) {}

  const std::byte* offsets_ptr() const {
    return ptr_;
  }
  const std::byte* data_ptr() const {
    return offsets_ptr() + kMaxVariadicVectorElements;
  }

  const int element_start(int idx) const {
    CHECK_LT(idx, num_elements_);
    return idx == 0 ? 0 : element_end(idx - 1);
  }
  const int element_end(int idx) const {
    CHECK_LT(idx, num_elements_);
    return std::to_integer<int>(offset_ptr()[idx]);
  }

  int num_elements_ = 0;
  const std::byte* ptr_ = nullptr;
};

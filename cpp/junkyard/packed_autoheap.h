#pragma once

#include <array>
#include <climits>
#include <cstddef>
#include <limits>

#include "absl/algorithm/container.h"
#include "absl/types/span.h"

#include "check.h"


template<typename T>
class PVector {
  // TODO: emulate std::is_trivially_relocatable_v
  // static_assert(std::std::is_trivially_relocatable_v<T>);
  // static_assert(`operator==` is `memcmp`);

public:
  PVector() {}

  bool empty() const { return size() == 0; }
  int size() const { return rep.size(); }

  // Note: there is no way to change elements. This would be hard
  // for PVector<int> and completely break PVector<PVector>.
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
    rep.data_as_type()[old_size] = std::move(value);
  }

  // TODO: proper iterator types
  const T* begin() const { return rep.data_as_type(); }
  const T* end() const { return begin() + size(); }

  bool operator==(const PVector& other) const {
    // TODO: Fix: This would not work for a heap-rep PVector!
    return size() == other.size() &&
      std::memcmp(rep.data_as_bytes(), other.data_as_bytes(), size_in_bytes()) == 0;
  }

private:
  template<int N>
  class Rep {
  public:
    static constexpr int kMaxInlineElements =
      std::min<int>(internal::kTheoreticalMaxInlineElements, N / sizeof(T) - 1);
    static_assert(N >= internal::kMinRepSize);

    Rep() {
      bytes.fill(std::byte(0));
      // TODO: Make checks debug-only
      CHECK(is_inline());
      CHECK_EQ(size(), 0);
    }
    ~Rep() {
      delete_values(0, size());
      if (!is_inline()) {
        delete data_as_bytes();
      }
    }

    bool is_inline() const {
      return bytes[0] != internal::kHeapRepMarker;
    }
    const std::byte* data_as_bytes() const {
      return is_inline()
        ? bytes.data() + sizeof(T)
        : *reinterpret_cast<std::byte* const*>(bytes.data() + sizeof(void*));
    }
    std::byte* data_as_bytes() {
      return is_inline()
        ? bytes.data() + sizeof(T)
        : *reinterpret_cast<std::byte**>(bytes.data() + sizeof(void*));
    }
    const T* data_as_type() const {
      return reinterpret_cast<const T*>(data_as_bytes());
    }
    T* data_as_type() {
      return reinterpret_cast<T*>(data_as_bytes());
    }
    int size_in_bytes() const {
      return size() * sizeof(T);
    }
    int size() const {
      if (is_inline()) {
        return std::to_integer<int>(bytes[0]);
      } else {
        uint32_t ret = 0;
        std::memcpy(&ret, bytes.data() + 4, 4);
        return ret;
      }
    }
    // Note: never goes from heap back to inline.
    void resize(int new_size) {
      const int old_size = size();
      if (new_size < old_size) {
        delete_values(new_size, old_size);
      }
      if (is_inline()) {
        if (new_size <= kMaxInlineElements) {
          write_inline_size(new_size);
        } else {
          const int new_capacity = internal::capacity_for_size(new_size);
          alloc_and_move(data_as_bytes(), old_size, new_capacity);
          write_heap_size(new_size);
        }
      } else {
        const int old_capacity = internal::capacity_for_size(old_size);
        const int new_capacity = internal::capacity_for_size(new_size);
        // Optimization potential: prevent shrinking if staying in heap
        // Optimization potential: allow going back to inline
        if (new_capacity != old_capacity) {
          const std::byte* old_data = data_as_bytes();
          alloc_and_move(old_data, old_size, new_capacity);
          delete old_data;
        }
        write_heap_size(new_size);
      }
      init_values(old_size, new_size);
    }

  private:
    void write_inline_size(int new_size) {
      bytes[0] = std::byte(new_size);
    }
    void write_heap_size(int new_size) {
      bytes[0] = internal::kHeapRepMarker;
      uint32_t size_to_write = new_size;
      std::memcpy(bytes.data() + 4, &size_to_write, 4);
    }
    void delete_values(int idx_begin, int idx_end) {
      T* data_ptr = data_as_type();
      T* begin = data_ptr + idx_begin;
      T* end = data_ptr + idx_end;
      for (T* it = begin; it != end; ++it) {
        it->~T();
      }
    }
    void alloc_and_move(const std::byte* old_data, int old_size, int new_capacity) {
      std::byte* new_data_ptr = new std::byte[new_capacity * sizeof(T)];
      std::memcpy(new_data_ptr, old_data, old_size * sizeof(T));
      std::memcpy(bytes.data() + sizeof(void*), &new_data_ptr, sizeof(void*));
    }
    void init_values(int old_size, int new_size) {
      std::byte* data_ptr = data_as_bytes();
      std::byte* begin = data_ptr + old_size * sizeof(T);
      std::byte* end  = data_ptr + new_size * sizeof(T);
      for (std::byte* p = begin; p < end; p += sizeof(T)) {
        new(p) T();
      }
    }

    // Inline rep data model:
    //   * data[0] - size
    //   * data[1...sizeof(T)) - unused (bad alignment)
    //   * data[sizeof(T)...) - data
    //
    // Heap rep data model:
    //   * data[0] = kHeapRepMarker
    //   * data[1-4) - unused  (note: can be used to store excess capacity when that's supported)
    //   * data[4-8) - size
    //   * data[8-16) - pointer
    //   * data[16-...) - unused
    //
    // Optimization potential: sparse metadata storage in heap rep when N is large,
    //   avoid bit operations for things like computing the size.
    //
    // TODO: Consider storing inlined size + union of reps (like absl::InlinedVector)
    // TODO: Consider using std::aligned_union
    alignas(T) std::array<std::byte, N> bytes;
  };

  Rep<internal::rep_size(sizeof(T))> rep;
};

#pragma once

#include "absl/container/flat_hash_map.h"

#include "functional.h"
#include "util.h"


struct MemoryUsage {
  size_t bytes = 0;
  size_t allocations = 0;
};

inline MemoryUsage operator+(const MemoryUsage& a, const MemoryUsage& b) {
  return {a.bytes + b.bytes, a.allocations + b.allocations};
}
inline MemoryUsage operator-(const MemoryUsage& a, const MemoryUsage& b) {
  return {a.bytes - b.bytes, a.allocations - b.allocations};
}

std::string to_string(const MemoryUsage& u);

// DO NOT OVERLOAD. Overload `estimated_heap_usage` instead.
template<typename T>
MemoryUsage estimated_ram_usage(const T& v);

template<typename Container>
MemoryUsage simple_container_estimated_ram_usage(const Container& c, bool cap_spare_capacity) {
  using value_type = typename Container::value_type;
  constexpr size_t kPageSize = 4096;
  size_t spare_capacity_usage = (c.capacity() - c.size()) * sizeof(value_type);
  if (cap_spare_capacity) {
    // Sequential containers like vectors don't initialize the spare capacity,
    // so the parts that spill over to the next page are essentially free.
    spare_capacity_usage = std::min(spare_capacity_usage, kPageSize);
  } else {
    // Containers like dense hash maps write at arbitrary locations, so the
    // entire capacity is used.
  }
  return MemoryUsage{spare_capacity_usage, 1}
    + sum(c, DISAMBIGUATE(estimated_ram_usage));
}

template<typename T>
static typename std::enable_if_t<std::is_fundamental_v<T>, MemoryUsage>
estimated_heap_usage(const T&) {
  return {0, 0};
}

inline bool is_string_inlined(const std::string& s) {
  return
    reinterpret_cast<const char*>(&s) <= reinterpret_cast<const char*>(s.data()) &&
    reinterpret_cast<const char*>(s.data()) < reinterpret_cast<const char*>(&s) + sizeof(s)
  ;
}

inline MemoryUsage estimated_heap_usage(const std::string& s) {
  if (is_string_inlined(s)) {
    return {0, 0};
  } else {
    return {s.capacity(), 1};
  }
}

template<typename T1, typename T2>
MemoryUsage estimated_heap_usage(const std::pair<T1, T2>& p) {
  return estimated_heap_usage(p.first) + estimated_heap_usage(p.second);
}

template<typename... Ts>
MemoryUsage estimated_heap_usage(const std::tuple<Ts...>& t) {
  return std::apply([](const auto&... args) {
    return (estimated_heap_usage(args) + ...);
  }, t);
}

template<typename T>
MemoryUsage estimated_heap_usage(const std::vector<T>& v) {
  return simple_container_estimated_ram_usage(v, true);
}

template<typename K, typename V>
MemoryUsage estimated_heap_usage(const absl::flat_hash_map<K, V>& c) {
  return simple_container_estimated_ram_usage(c, false);
}

template<typename T>
MemoryUsage estimated_heap_usage(const absl::flat_hash_set<T>& c) {
  return simple_container_estimated_ram_usage(c, false);
}

template<typename T>
MemoryUsage estimated_ram_usage(const T& v) {
  return estimated_heap_usage(v) + MemoryUsage{sizeof(v), 0};
}

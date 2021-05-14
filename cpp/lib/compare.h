#pragma once

#include "absl/algorithm/container.h"


namespace cmp {

template<typename F>
auto greater_from_less(const F& less) {
  return [=](const auto& a, const auto& b) -> bool {
    return less(b, a);
  };
}

template<typename F>
auto lexicographical(const F& comp) {
  return [=](const auto& a, const auto& b) -> bool {
    return absl::c_lexicographical_compare(a, b, comp);
  };
}

template<typename Projector>
auto projected(const Projector& projector) {
  return [=](const auto& a, const auto& b) -> bool {
    return projector(a) < projector(b);
  };
}

}  // namespace cmp

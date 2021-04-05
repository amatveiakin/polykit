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

// TODO: Finish
// template<typename Object, typename Projected>
// auto projected(Projected (*projector)(const Object&)) {
//   return [=](const Object& lhs, const Object& rhs) {
//     return projector(lhs) < projector(rhs);
//   };
// }

}  // namespace cmp

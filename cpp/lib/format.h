#pragma once

#include "format_basic.h"
#include "x.h"


namespace fmt {

inline std::string function_num_args(
    const std::string& name,
    const XArgs& indices,
    HSpacing hspacing = HSpacing::dense) {
  return function(name, indices.as_string(), hspacing);
}

}  // namespace fmt

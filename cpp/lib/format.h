#pragma once

#include "format_basic.h"
#include "x.h"


namespace fmt {

inline std::string function_num_args(
    const std::string& name,
    const XArgs& indices,
    HSpacing hspacing = HSpacing::dense) {
  return function(name, mapped_to_string(indices.as_x()), hspacing);
}

}  // namespace fmt

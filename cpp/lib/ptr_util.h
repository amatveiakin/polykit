#pragma once

#include <memory>


template<typename T>
auto wrap_shared(T value) {
  return std::make_shared<const T>(std::move(value));
}

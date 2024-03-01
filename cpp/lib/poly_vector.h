#pragma once

#include "poly_limits.h"
#include "capped_vector.h"
#include "pvector.h"


template<typename T>
using WeightVector = std::conditional_t<
  kDynamicWeight,
  PVector<T, kMaxWeight>,
  CappedVector<T, kMaxWeight>
>;

template<typename T>
using CopartVector = std::conditional_t<
  kDynamicCoparts,
  PVector<T, kMaxCoparts>,
  CappedVector<T, kMaxCoparts>
>;

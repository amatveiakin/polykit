#pragma once

#include "word.h"


// Gives a small boost to shuffle (and thus Lyndon) speed,
// but increases compilation time.
// Zero return value means unroll doesn't exist.
#ifdef UNROLL_SHUFFLE
WordExpr shuffle_product_unrolled(Word u, Word v);
#else
inline WordExpr shuffle_product_unrolled(Word u, Word v) {
  return {};
}
#endif

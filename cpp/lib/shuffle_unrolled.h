#include "linear.h"


// By default shuffle is unrolled in release mode. To override, set UNROLL_SHUFFLE to 0 or 1.
#ifndef UNROLL_SHUFFLE
#  ifdef NDEBUG
#    define UNROLL_SHUFFLE 1
#  else
#    define UNROLL_SHUFFLE 0
#  endif
#endif

#if UNROLL_SHUFFLE
template<typename MonomT>
BasicLinear<SimpleLinearParam<MonomT>> shuffle_product_unrolled_1_1(const MonomT& u, const MonomT& v) {
  BasicLinear<SimpleLinearParam<MonomT>> ret;
  ret.add_to_key({u[0], v[0]}, 1);
  ret.add_to_key({v[0], u[0]}, 1);
  return ret;
}

template<typename MonomT>
BasicLinear<SimpleLinearParam<MonomT>> shuffle_product_unrolled_1_2(const MonomT& u, const MonomT& v) {
  BasicLinear<SimpleLinearParam<MonomT>> ret;
  ret.add_to_key({u[0], v[0], v[1]}, 1);
  ret.add_to_key({v[0], u[0], v[1]}, 1);
  ret.add_to_key({v[0], v[1], u[0]}, 1);
  return ret;
}

template<typename MonomT>
BasicLinear<SimpleLinearParam<MonomT>> shuffle_product_unrolled_1_3(const MonomT& u, const MonomT& v) {
  BasicLinear<SimpleLinearParam<MonomT>> ret;
  ret.add_to_key({u[0], v[0], v[1], v[2]}, 1);
  ret.add_to_key({v[0], u[0], v[1], v[2]}, 1);
  ret.add_to_key({v[0], v[1], u[0], v[2]}, 1);
  ret.add_to_key({v[0], v[1], v[2], u[0]}, 1);
  return ret;
}

template<typename MonomT>
BasicLinear<SimpleLinearParam<MonomT>> shuffle_product_unrolled_1_4(const MonomT& u, const MonomT& v) {
  BasicLinear<SimpleLinearParam<MonomT>> ret;
  ret.add_to_key({u[0], v[0], v[1], v[2], v[3]}, 1);
  ret.add_to_key({v[0], u[0], v[1], v[2], v[3]}, 1);
  ret.add_to_key({v[0], v[1], u[0], v[2], v[3]}, 1);
  ret.add_to_key({v[0], v[1], v[2], u[0], v[3]}, 1);
  ret.add_to_key({v[0], v[1], v[2], v[3], u[0]}, 1);
  return ret;
}

template<typename MonomT>
BasicLinear<SimpleLinearParam<MonomT>> shuffle_product_unrolled_1_5(const MonomT& u, const MonomT& v) {
  BasicLinear<SimpleLinearParam<MonomT>> ret;
  ret.add_to_key({u[0], v[0], v[1], v[2], v[3], v[4]}, 1);
  ret.add_to_key({v[0], u[0], v[1], v[2], v[3], v[4]}, 1);
  ret.add_to_key({v[0], v[1], u[0], v[2], v[3], v[4]}, 1);
  ret.add_to_key({v[0], v[1], v[2], u[0], v[3], v[4]}, 1);
  ret.add_to_key({v[0], v[1], v[2], v[3], u[0], v[4]}, 1);
  ret.add_to_key({v[0], v[1], v[2], v[3], v[4], u[0]}, 1);
  return ret;
}

template<typename MonomT>
BasicLinear<SimpleLinearParam<MonomT>> shuffle_product_unrolled_2_2(const MonomT& u, const MonomT& v) {
  BasicLinear<SimpleLinearParam<MonomT>> ret;
  ret.add_to_key({u[0], u[1], v[0], v[1]}, 1);
  ret.add_to_key({u[0], v[0], u[1], v[1]}, 1);
  ret.add_to_key({u[0], v[0], v[1], u[1]}, 1);
  ret.add_to_key({v[0], u[0], u[1], v[1]}, 1);
  ret.add_to_key({v[0], u[0], v[1], u[1]}, 1);
  ret.add_to_key({v[0], v[1], u[0], u[1]}, 1);
  return ret;
}

template<typename MonomT>
BasicLinear<SimpleLinearParam<MonomT>> shuffle_product_unrolled_2_3(const MonomT& u, const MonomT& v) {
  BasicLinear<SimpleLinearParam<MonomT>> ret;
  ret.add_to_key({u[0], u[1], v[0], v[1], v[2]}, 1);
  ret.add_to_key({u[0], v[0], u[1], v[1], v[2]}, 1);
  ret.add_to_key({u[0], v[0], v[1], u[1], v[2]}, 1);
  ret.add_to_key({u[0], v[0], v[1], v[2], u[1]}, 1);
  ret.add_to_key({v[0], u[0], u[1], v[1], v[2]}, 1);
  ret.add_to_key({v[0], u[0], v[1], u[1], v[2]}, 1);
  ret.add_to_key({v[0], u[0], v[1], v[2], u[1]}, 1);
  ret.add_to_key({v[0], v[1], u[0], u[1], v[2]}, 1);
  ret.add_to_key({v[0], v[1], u[0], v[2], u[1]}, 1);
  ret.add_to_key({v[0], v[1], v[2], u[0], u[1]}, 1);
  return ret;
}

template<typename MonomT>
BasicLinear<SimpleLinearParam<MonomT>> shuffle_product_unrolled_2_4(const MonomT& u, const MonomT& v) {
  BasicLinear<SimpleLinearParam<MonomT>> ret;
  ret.add_to_key({u[0], u[1], v[0], v[1], v[2], v[3]}, 1);
  ret.add_to_key({u[0], v[0], u[1], v[1], v[2], v[3]}, 1);
  ret.add_to_key({u[0], v[0], v[1], u[1], v[2], v[3]}, 1);
  ret.add_to_key({u[0], v[0], v[1], v[2], u[1], v[3]}, 1);
  ret.add_to_key({u[0], v[0], v[1], v[2], v[3], u[1]}, 1);
  ret.add_to_key({v[0], u[0], u[1], v[1], v[2], v[3]}, 1);
  ret.add_to_key({v[0], u[0], v[1], u[1], v[2], v[3]}, 1);
  ret.add_to_key({v[0], u[0], v[1], v[2], u[1], v[3]}, 1);
  ret.add_to_key({v[0], u[0], v[1], v[2], v[3], u[1]}, 1);
  ret.add_to_key({v[0], v[1], u[0], u[1], v[2], v[3]}, 1);
  ret.add_to_key({v[0], v[1], u[0], v[2], u[1], v[3]}, 1);
  ret.add_to_key({v[0], v[1], u[0], v[2], v[3], u[1]}, 1);
  ret.add_to_key({v[0], v[1], v[2], u[0], u[1], v[3]}, 1);
  ret.add_to_key({v[0], v[1], v[2], u[0], v[3], u[1]}, 1);
  ret.add_to_key({v[0], v[1], v[2], v[3], u[0], u[1]}, 1);
  return ret;
}

template<typename MonomT>
BasicLinear<SimpleLinearParam<MonomT>> shuffle_product_unrolled_3_3(const MonomT& u, const MonomT& v) {
  BasicLinear<SimpleLinearParam<MonomT>> ret;
  ret.add_to_key({u[0], u[1], u[2], v[0], v[1], v[2]}, 1);
  ret.add_to_key({u[0], u[1], v[0], u[2], v[1], v[2]}, 1);
  ret.add_to_key({u[0], u[1], v[0], v[1], u[2], v[2]}, 1);
  ret.add_to_key({u[0], u[1], v[0], v[1], v[2], u[2]}, 1);
  ret.add_to_key({u[0], v[0], u[1], u[2], v[1], v[2]}, 1);
  ret.add_to_key({u[0], v[0], u[1], v[1], u[2], v[2]}, 1);
  ret.add_to_key({u[0], v[0], u[1], v[1], v[2], u[2]}, 1);
  ret.add_to_key({u[0], v[0], v[1], u[1], u[2], v[2]}, 1);
  ret.add_to_key({u[0], v[0], v[1], u[1], v[2], u[2]}, 1);
  ret.add_to_key({u[0], v[0], v[1], v[2], u[1], u[2]}, 1);
  ret.add_to_key({v[0], u[0], u[1], u[2], v[1], v[2]}, 1);
  ret.add_to_key({v[0], u[0], u[1], v[1], u[2], v[2]}, 1);
  ret.add_to_key({v[0], u[0], u[1], v[1], v[2], u[2]}, 1);
  ret.add_to_key({v[0], u[0], v[1], u[1], u[2], v[2]}, 1);
  ret.add_to_key({v[0], u[0], v[1], u[1], v[2], u[2]}, 1);
  ret.add_to_key({v[0], u[0], v[1], v[2], u[1], u[2]}, 1);
  ret.add_to_key({v[0], v[1], u[0], u[1], u[2], v[2]}, 1);
  ret.add_to_key({v[0], v[1], u[0], u[1], v[2], u[2]}, 1);
  ret.add_to_key({v[0], v[1], u[0], v[2], u[1], u[2]}, 1);
  ret.add_to_key({v[0], v[1], v[2], u[0], u[1], u[2]}, 1);
  return ret;
}

template<typename MonomT>
BasicLinear<SimpleLinearParam<MonomT>> shuffle_product_unrolled(MonomT u, MonomT v) {
  if (u.size() > v.size()) {
    std::swap(u, v);
  }
  switch (u.size()) {
    case 1:
      switch (v.size()) {
        case 1: return shuffle_product_unrolled_1_1(u, v);
        case 2: return shuffle_product_unrolled_1_2(u, v);
        case 3: return shuffle_product_unrolled_1_3(u, v);
        case 4: return shuffle_product_unrolled_1_4(u, v);
        case 5: return shuffle_product_unrolled_1_5(u, v);
      }
      break;
    case 2:
      switch (v.size()) {
        case 2: return shuffle_product_unrolled_2_2(u, v);
        case 3: return shuffle_product_unrolled_2_3(u, v);
        case 4: return shuffle_product_unrolled_2_4(u, v);
      }
      break;
    case 3:
      switch (v.size()) {
        case 3: return shuffle_product_unrolled_3_3(u, v);
      }
      break;
  }
  return {};
}
#endif

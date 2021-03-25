#include "linear.h"


template<typename MonomT>
Linear<SimpleLinearParam<MonomT>> shuffle_product_unrolled_1_1_1(const MonomT& v0, const MonomT& v1, const MonomT& v2) {
  Linear<SimpleLinearParam<MonomT>> ret;
  ret.add_to_key({v0[0], v1[0], v2[0]}, 1);
  ret.add_to_key({v0[0], v2[0], v1[0]}, 1);
  ret.add_to_key({v1[0], v0[0], v2[0]}, 1);
  ret.add_to_key({v1[0], v2[0], v0[0]}, 1);
  ret.add_to_key({v2[0], v0[0], v1[0]}, 1);
  ret.add_to_key({v2[0], v1[0], v0[0]}, 1);
  return ret;
}

template<typename MonomT>
Linear<SimpleLinearParam<MonomT>> shuffle_product_unrolled_1_1_2(const MonomT& v0, const MonomT& v1, const MonomT& v2) {
  Linear<SimpleLinearParam<MonomT>> ret;
  ret.add_to_key({v0[0], v1[0], v2[0], v2[1]}, 1);
  ret.add_to_key({v0[0], v2[0], v1[0], v2[1]}, 1);
  ret.add_to_key({v0[0], v2[0], v2[1], v1[0]}, 1);
  ret.add_to_key({v1[0], v0[0], v2[0], v2[1]}, 1);
  ret.add_to_key({v1[0], v2[0], v0[0], v2[1]}, 1);
  ret.add_to_key({v1[0], v2[0], v2[1], v0[0]}, 1);
  ret.add_to_key({v2[0], v0[0], v1[0], v2[1]}, 1);
  ret.add_to_key({v2[0], v0[0], v2[1], v1[0]}, 1);
  ret.add_to_key({v2[0], v1[0], v0[0], v2[1]}, 1);
  ret.add_to_key({v2[0], v1[0], v2[1], v0[0]}, 1);
  ret.add_to_key({v2[0], v2[1], v0[0], v1[0]}, 1);
  ret.add_to_key({v2[0], v2[1], v1[0], v0[0]}, 1);
  return ret;
}

template<typename MonomT>
Linear<SimpleLinearParam<MonomT>> shuffle_product_unrolled_1_1_3(const MonomT& v0, const MonomT& v1, const MonomT& v2) {
  Linear<SimpleLinearParam<MonomT>> ret;
  ret.add_to_key({v0[0], v1[0], v2[0], v2[1], v2[2]}, 1);
  ret.add_to_key({v0[0], v2[0], v1[0], v2[1], v2[2]}, 1);
  ret.add_to_key({v0[0], v2[0], v2[1], v1[0], v2[2]}, 1);
  ret.add_to_key({v0[0], v2[0], v2[1], v2[2], v1[0]}, 1);
  ret.add_to_key({v1[0], v0[0], v2[0], v2[1], v2[2]}, 1);
  ret.add_to_key({v1[0], v2[0], v0[0], v2[1], v2[2]}, 1);
  ret.add_to_key({v1[0], v2[0], v2[1], v0[0], v2[2]}, 1);
  ret.add_to_key({v1[0], v2[0], v2[1], v2[2], v0[0]}, 1);
  ret.add_to_key({v2[0], v0[0], v1[0], v2[1], v2[2]}, 1);
  ret.add_to_key({v2[0], v0[0], v2[1], v1[0], v2[2]}, 1);
  ret.add_to_key({v2[0], v0[0], v2[1], v2[2], v1[0]}, 1);
  ret.add_to_key({v2[0], v1[0], v0[0], v2[1], v2[2]}, 1);
  ret.add_to_key({v2[0], v1[0], v2[1], v0[0], v2[2]}, 1);
  ret.add_to_key({v2[0], v1[0], v2[1], v2[2], v0[0]}, 1);
  ret.add_to_key({v2[0], v2[1], v0[0], v1[0], v2[2]}, 1);
  ret.add_to_key({v2[0], v2[1], v0[0], v2[2], v1[0]}, 1);
  ret.add_to_key({v2[0], v2[1], v1[0], v0[0], v2[2]}, 1);
  ret.add_to_key({v2[0], v2[1], v1[0], v2[2], v0[0]}, 1);
  ret.add_to_key({v2[0], v2[1], v2[2], v0[0], v1[0]}, 1);
  ret.add_to_key({v2[0], v2[1], v2[2], v1[0], v0[0]}, 1);
  return ret;
}

template<typename MonomT>
Linear<SimpleLinearParam<MonomT>> shuffle_product_unrolled_1_1_4(const MonomT& v0, const MonomT& v1, const MonomT& v2) {
  Linear<SimpleLinearParam<MonomT>> ret;
  ret.add_to_key({v0[0], v1[0], v2[0], v2[1], v2[2], v2[3]}, 1);
  ret.add_to_key({v0[0], v2[0], v1[0], v2[1], v2[2], v2[3]}, 1);
  ret.add_to_key({v0[0], v2[0], v2[1], v1[0], v2[2], v2[3]}, 1);
  ret.add_to_key({v0[0], v2[0], v2[1], v2[2], v1[0], v2[3]}, 1);
  ret.add_to_key({v0[0], v2[0], v2[1], v2[2], v2[3], v1[0]}, 1);
  ret.add_to_key({v1[0], v0[0], v2[0], v2[1], v2[2], v2[3]}, 1);
  ret.add_to_key({v1[0], v2[0], v0[0], v2[1], v2[2], v2[3]}, 1);
  ret.add_to_key({v1[0], v2[0], v2[1], v0[0], v2[2], v2[3]}, 1);
  ret.add_to_key({v1[0], v2[0], v2[1], v2[2], v0[0], v2[3]}, 1);
  ret.add_to_key({v1[0], v2[0], v2[1], v2[2], v2[3], v0[0]}, 1);
  ret.add_to_key({v2[0], v0[0], v1[0], v2[1], v2[2], v2[3]}, 1);
  ret.add_to_key({v2[0], v0[0], v2[1], v1[0], v2[2], v2[3]}, 1);
  ret.add_to_key({v2[0], v0[0], v2[1], v2[2], v1[0], v2[3]}, 1);
  ret.add_to_key({v2[0], v0[0], v2[1], v2[2], v2[3], v1[0]}, 1);
  ret.add_to_key({v2[0], v1[0], v0[0], v2[1], v2[2], v2[3]}, 1);
  ret.add_to_key({v2[0], v1[0], v2[1], v0[0], v2[2], v2[3]}, 1);
  ret.add_to_key({v2[0], v1[0], v2[1], v2[2], v0[0], v2[3]}, 1);
  ret.add_to_key({v2[0], v1[0], v2[1], v2[2], v2[3], v0[0]}, 1);
  ret.add_to_key({v2[0], v2[1], v0[0], v1[0], v2[2], v2[3]}, 1);
  ret.add_to_key({v2[0], v2[1], v0[0], v2[2], v1[0], v2[3]}, 1);
  ret.add_to_key({v2[0], v2[1], v0[0], v2[2], v2[3], v1[0]}, 1);
  ret.add_to_key({v2[0], v2[1], v1[0], v0[0], v2[2], v2[3]}, 1);
  ret.add_to_key({v2[0], v2[1], v1[0], v2[2], v0[0], v2[3]}, 1);
  ret.add_to_key({v2[0], v2[1], v1[0], v2[2], v2[3], v0[0]}, 1);
  ret.add_to_key({v2[0], v2[1], v2[2], v0[0], v1[0], v2[3]}, 1);
  ret.add_to_key({v2[0], v2[1], v2[2], v0[0], v2[3], v1[0]}, 1);
  ret.add_to_key({v2[0], v2[1], v2[2], v1[0], v0[0], v2[3]}, 1);
  ret.add_to_key({v2[0], v2[1], v2[2], v1[0], v2[3], v0[0]}, 1);
  ret.add_to_key({v2[0], v2[1], v2[2], v2[3], v0[0], v1[0]}, 1);
  ret.add_to_key({v2[0], v2[1], v2[2], v2[3], v1[0], v0[0]}, 1);
  return ret;
}

template<typename MonomT>
Linear<SimpleLinearParam<MonomT>> shuffle_product_unrolled_1_2_2(const MonomT& v0, const MonomT& v1, const MonomT& v2) {
  Linear<SimpleLinearParam<MonomT>> ret;
  ret.add_to_key({v0[0], v1[0], v1[1], v2[0], v2[1]}, 1);
  ret.add_to_key({v0[0], v1[0], v2[0], v1[1], v2[1]}, 1);
  ret.add_to_key({v0[0], v1[0], v2[0], v2[1], v1[1]}, 1);
  ret.add_to_key({v0[0], v2[0], v1[0], v1[1], v2[1]}, 1);
  ret.add_to_key({v0[0], v2[0], v1[0], v2[1], v1[1]}, 1);
  ret.add_to_key({v0[0], v2[0], v2[1], v1[0], v1[1]}, 1);
  ret.add_to_key({v1[0], v0[0], v1[1], v2[0], v2[1]}, 1);
  ret.add_to_key({v1[0], v0[0], v2[0], v1[1], v2[1]}, 1);
  ret.add_to_key({v1[0], v0[0], v2[0], v2[1], v1[1]}, 1);
  ret.add_to_key({v1[0], v1[1], v0[0], v2[0], v2[1]}, 1);
  ret.add_to_key({v1[0], v1[1], v2[0], v0[0], v2[1]}, 1);
  ret.add_to_key({v1[0], v1[1], v2[0], v2[1], v0[0]}, 1);
  ret.add_to_key({v1[0], v2[0], v0[0], v1[1], v2[1]}, 1);
  ret.add_to_key({v1[0], v2[0], v0[0], v2[1], v1[1]}, 1);
  ret.add_to_key({v1[0], v2[0], v1[1], v0[0], v2[1]}, 1);
  ret.add_to_key({v1[0], v2[0], v1[1], v2[1], v0[0]}, 1);
  ret.add_to_key({v1[0], v2[0], v2[1], v0[0], v1[1]}, 1);
  ret.add_to_key({v1[0], v2[0], v2[1], v1[1], v0[0]}, 1);
  ret.add_to_key({v2[0], v0[0], v1[0], v1[1], v2[1]}, 1);
  ret.add_to_key({v2[0], v0[0], v1[0], v2[1], v1[1]}, 1);
  ret.add_to_key({v2[0], v0[0], v2[1], v1[0], v1[1]}, 1);
  ret.add_to_key({v2[0], v1[0], v0[0], v1[1], v2[1]}, 1);
  ret.add_to_key({v2[0], v1[0], v0[0], v2[1], v1[1]}, 1);
  ret.add_to_key({v2[0], v1[0], v1[1], v0[0], v2[1]}, 1);
  ret.add_to_key({v2[0], v1[0], v1[1], v2[1], v0[0]}, 1);
  ret.add_to_key({v2[0], v1[0], v2[1], v0[0], v1[1]}, 1);
  ret.add_to_key({v2[0], v1[0], v2[1], v1[1], v0[0]}, 1);
  ret.add_to_key({v2[0], v2[1], v0[0], v1[0], v1[1]}, 1);
  ret.add_to_key({v2[0], v2[1], v1[0], v0[0], v1[1]}, 1);
  ret.add_to_key({v2[0], v2[1], v1[0], v1[1], v0[0]}, 1);
  return ret;
}

template<typename MonomT>
Linear<SimpleLinearParam<MonomT>> shuffle_product_unrolled_1_2_3(const MonomT& v0, const MonomT& v1, const MonomT& v2) {
  Linear<SimpleLinearParam<MonomT>> ret;
  ret.add_to_key({v0[0], v1[0], v1[1], v2[0], v2[1], v2[2]}, 1);
  ret.add_to_key({v0[0], v1[0], v2[0], v1[1], v2[1], v2[2]}, 1);
  ret.add_to_key({v0[0], v1[0], v2[0], v2[1], v1[1], v2[2]}, 1);
  ret.add_to_key({v0[0], v1[0], v2[0], v2[1], v2[2], v1[1]}, 1);
  ret.add_to_key({v0[0], v2[0], v1[0], v1[1], v2[1], v2[2]}, 1);
  ret.add_to_key({v0[0], v2[0], v1[0], v2[1], v1[1], v2[2]}, 1);
  ret.add_to_key({v0[0], v2[0], v1[0], v2[1], v2[2], v1[1]}, 1);
  ret.add_to_key({v0[0], v2[0], v2[1], v1[0], v1[1], v2[2]}, 1);
  ret.add_to_key({v0[0], v2[0], v2[1], v1[0], v2[2], v1[1]}, 1);
  ret.add_to_key({v0[0], v2[0], v2[1], v2[2], v1[0], v1[1]}, 1);
  ret.add_to_key({v1[0], v0[0], v1[1], v2[0], v2[1], v2[2]}, 1);
  ret.add_to_key({v1[0], v0[0], v2[0], v1[1], v2[1], v2[2]}, 1);
  ret.add_to_key({v1[0], v0[0], v2[0], v2[1], v1[1], v2[2]}, 1);
  ret.add_to_key({v1[0], v0[0], v2[0], v2[1], v2[2], v1[1]}, 1);
  ret.add_to_key({v1[0], v1[1], v0[0], v2[0], v2[1], v2[2]}, 1);
  ret.add_to_key({v1[0], v1[1], v2[0], v0[0], v2[1], v2[2]}, 1);
  ret.add_to_key({v1[0], v1[1], v2[0], v2[1], v0[0], v2[2]}, 1);
  ret.add_to_key({v1[0], v1[1], v2[0], v2[1], v2[2], v0[0]}, 1);
  ret.add_to_key({v1[0], v2[0], v0[0], v1[1], v2[1], v2[2]}, 1);
  ret.add_to_key({v1[0], v2[0], v0[0], v2[1], v1[1], v2[2]}, 1);
  ret.add_to_key({v1[0], v2[0], v0[0], v2[1], v2[2], v1[1]}, 1);
  ret.add_to_key({v1[0], v2[0], v1[1], v0[0], v2[1], v2[2]}, 1);
  ret.add_to_key({v1[0], v2[0], v1[1], v2[1], v0[0], v2[2]}, 1);
  ret.add_to_key({v1[0], v2[0], v1[1], v2[1], v2[2], v0[0]}, 1);
  ret.add_to_key({v1[0], v2[0], v2[1], v0[0], v1[1], v2[2]}, 1);
  ret.add_to_key({v1[0], v2[0], v2[1], v0[0], v2[2], v1[1]}, 1);
  ret.add_to_key({v1[0], v2[0], v2[1], v1[1], v0[0], v2[2]}, 1);
  ret.add_to_key({v1[0], v2[0], v2[1], v1[1], v2[2], v0[0]}, 1);
  ret.add_to_key({v1[0], v2[0], v2[1], v2[2], v0[0], v1[1]}, 1);
  ret.add_to_key({v1[0], v2[0], v2[1], v2[2], v1[1], v0[0]}, 1);
  ret.add_to_key({v2[0], v0[0], v1[0], v1[1], v2[1], v2[2]}, 1);
  ret.add_to_key({v2[0], v0[0], v1[0], v2[1], v1[1], v2[2]}, 1);
  ret.add_to_key({v2[0], v0[0], v1[0], v2[1], v2[2], v1[1]}, 1);
  ret.add_to_key({v2[0], v0[0], v2[1], v1[0], v1[1], v2[2]}, 1);
  ret.add_to_key({v2[0], v0[0], v2[1], v1[0], v2[2], v1[1]}, 1);
  ret.add_to_key({v2[0], v0[0], v2[1], v2[2], v1[0], v1[1]}, 1);
  ret.add_to_key({v2[0], v1[0], v0[0], v1[1], v2[1], v2[2]}, 1);
  ret.add_to_key({v2[0], v1[0], v0[0], v2[1], v1[1], v2[2]}, 1);
  ret.add_to_key({v2[0], v1[0], v0[0], v2[1], v2[2], v1[1]}, 1);
  ret.add_to_key({v2[0], v1[0], v1[1], v0[0], v2[1], v2[2]}, 1);
  ret.add_to_key({v2[0], v1[0], v1[1], v2[1], v0[0], v2[2]}, 1);
  ret.add_to_key({v2[0], v1[0], v1[1], v2[1], v2[2], v0[0]}, 1);
  ret.add_to_key({v2[0], v1[0], v2[1], v0[0], v1[1], v2[2]}, 1);
  ret.add_to_key({v2[0], v1[0], v2[1], v0[0], v2[2], v1[1]}, 1);
  ret.add_to_key({v2[0], v1[0], v2[1], v1[1], v0[0], v2[2]}, 1);
  ret.add_to_key({v2[0], v1[0], v2[1], v1[1], v2[2], v0[0]}, 1);
  ret.add_to_key({v2[0], v1[0], v2[1], v2[2], v0[0], v1[1]}, 1);
  ret.add_to_key({v2[0], v1[0], v2[1], v2[2], v1[1], v0[0]}, 1);
  ret.add_to_key({v2[0], v2[1], v0[0], v1[0], v1[1], v2[2]}, 1);
  ret.add_to_key({v2[0], v2[1], v0[0], v1[0], v2[2], v1[1]}, 1);
  ret.add_to_key({v2[0], v2[1], v0[0], v2[2], v1[0], v1[1]}, 1);
  ret.add_to_key({v2[0], v2[1], v1[0], v0[0], v1[1], v2[2]}, 1);
  ret.add_to_key({v2[0], v2[1], v1[0], v0[0], v2[2], v1[1]}, 1);
  ret.add_to_key({v2[0], v2[1], v1[0], v1[1], v0[0], v2[2]}, 1);
  ret.add_to_key({v2[0], v2[1], v1[0], v1[1], v2[2], v0[0]}, 1);
  ret.add_to_key({v2[0], v2[1], v1[0], v2[2], v0[0], v1[1]}, 1);
  ret.add_to_key({v2[0], v2[1], v1[0], v2[2], v1[1], v0[0]}, 1);
  ret.add_to_key({v2[0], v2[1], v2[2], v0[0], v1[0], v1[1]}, 1);
  ret.add_to_key({v2[0], v2[1], v2[2], v1[0], v0[0], v1[1]}, 1);
  ret.add_to_key({v2[0], v2[1], v2[2], v1[0], v1[1], v0[0]}, 1);
  return ret;
}

template<typename MonomT>
Linear<SimpleLinearParam<MonomT>> shuffle_product_unrolled_2_2_2(const MonomT& v0, const MonomT& v1, const MonomT& v2) {
  Linear<SimpleLinearParam<MonomT>> ret;
  ret.add_to_key({v0[0], v0[1], v1[0], v1[1], v2[0], v2[1]}, 1);
  ret.add_to_key({v0[0], v0[1], v1[0], v2[0], v1[1], v2[1]}, 1);
  ret.add_to_key({v0[0], v0[1], v1[0], v2[0], v2[1], v1[1]}, 1);
  ret.add_to_key({v0[0], v0[1], v2[0], v1[0], v1[1], v2[1]}, 1);
  ret.add_to_key({v0[0], v0[1], v2[0], v1[0], v2[1], v1[1]}, 1);
  ret.add_to_key({v0[0], v0[1], v2[0], v2[1], v1[0], v1[1]}, 1);
  ret.add_to_key({v0[0], v1[0], v0[1], v1[1], v2[0], v2[1]}, 1);
  ret.add_to_key({v0[0], v1[0], v0[1], v2[0], v1[1], v2[1]}, 1);
  ret.add_to_key({v0[0], v1[0], v0[1], v2[0], v2[1], v1[1]}, 1);
  ret.add_to_key({v0[0], v1[0], v1[1], v0[1], v2[0], v2[1]}, 1);
  ret.add_to_key({v0[0], v1[0], v1[1], v2[0], v0[1], v2[1]}, 1);
  ret.add_to_key({v0[0], v1[0], v1[1], v2[0], v2[1], v0[1]}, 1);
  ret.add_to_key({v0[0], v1[0], v2[0], v0[1], v1[1], v2[1]}, 1);
  ret.add_to_key({v0[0], v1[0], v2[0], v0[1], v2[1], v1[1]}, 1);
  ret.add_to_key({v0[0], v1[0], v2[0], v1[1], v0[1], v2[1]}, 1);
  ret.add_to_key({v0[0], v1[0], v2[0], v1[1], v2[1], v0[1]}, 1);
  ret.add_to_key({v0[0], v1[0], v2[0], v2[1], v0[1], v1[1]}, 1);
  ret.add_to_key({v0[0], v1[0], v2[0], v2[1], v1[1], v0[1]}, 1);
  ret.add_to_key({v0[0], v2[0], v0[1], v1[0], v1[1], v2[1]}, 1);
  ret.add_to_key({v0[0], v2[0], v0[1], v1[0], v2[1], v1[1]}, 1);
  ret.add_to_key({v0[0], v2[0], v0[1], v2[1], v1[0], v1[1]}, 1);
  ret.add_to_key({v0[0], v2[0], v1[0], v0[1], v1[1], v2[1]}, 1);
  ret.add_to_key({v0[0], v2[0], v1[0], v0[1], v2[1], v1[1]}, 1);
  ret.add_to_key({v0[0], v2[0], v1[0], v1[1], v0[1], v2[1]}, 1);
  ret.add_to_key({v0[0], v2[0], v1[0], v1[1], v2[1], v0[1]}, 1);
  ret.add_to_key({v0[0], v2[0], v1[0], v2[1], v0[1], v1[1]}, 1);
  ret.add_to_key({v0[0], v2[0], v1[0], v2[1], v1[1], v0[1]}, 1);
  ret.add_to_key({v0[0], v2[0], v2[1], v0[1], v1[0], v1[1]}, 1);
  ret.add_to_key({v0[0], v2[0], v2[1], v1[0], v0[1], v1[1]}, 1);
  ret.add_to_key({v0[0], v2[0], v2[1], v1[0], v1[1], v0[1]}, 1);
  ret.add_to_key({v1[0], v0[0], v0[1], v1[1], v2[0], v2[1]}, 1);
  ret.add_to_key({v1[0], v0[0], v0[1], v2[0], v1[1], v2[1]}, 1);
  ret.add_to_key({v1[0], v0[0], v0[1], v2[0], v2[1], v1[1]}, 1);
  ret.add_to_key({v1[0], v0[0], v1[1], v0[1], v2[0], v2[1]}, 1);
  ret.add_to_key({v1[0], v0[0], v1[1], v2[0], v0[1], v2[1]}, 1);
  ret.add_to_key({v1[0], v0[0], v1[1], v2[0], v2[1], v0[1]}, 1);
  ret.add_to_key({v1[0], v0[0], v2[0], v0[1], v1[1], v2[1]}, 1);
  ret.add_to_key({v1[0], v0[0], v2[0], v0[1], v2[1], v1[1]}, 1);
  ret.add_to_key({v1[0], v0[0], v2[0], v1[1], v0[1], v2[1]}, 1);
  ret.add_to_key({v1[0], v0[0], v2[0], v1[1], v2[1], v0[1]}, 1);
  ret.add_to_key({v1[0], v0[0], v2[0], v2[1], v0[1], v1[1]}, 1);
  ret.add_to_key({v1[0], v0[0], v2[0], v2[1], v1[1], v0[1]}, 1);
  ret.add_to_key({v1[0], v1[1], v0[0], v0[1], v2[0], v2[1]}, 1);
  ret.add_to_key({v1[0], v1[1], v0[0], v2[0], v0[1], v2[1]}, 1);
  ret.add_to_key({v1[0], v1[1], v0[0], v2[0], v2[1], v0[1]}, 1);
  ret.add_to_key({v1[0], v1[1], v2[0], v0[0], v0[1], v2[1]}, 1);
  ret.add_to_key({v1[0], v1[1], v2[0], v0[0], v2[1], v0[1]}, 1);
  ret.add_to_key({v1[0], v1[1], v2[0], v2[1], v0[0], v0[1]}, 1);
  ret.add_to_key({v1[0], v2[0], v0[0], v0[1], v1[1], v2[1]}, 1);
  ret.add_to_key({v1[0], v2[0], v0[0], v0[1], v2[1], v1[1]}, 1);
  ret.add_to_key({v1[0], v2[0], v0[0], v1[1], v0[1], v2[1]}, 1);
  ret.add_to_key({v1[0], v2[0], v0[0], v1[1], v2[1], v0[1]}, 1);
  ret.add_to_key({v1[0], v2[0], v0[0], v2[1], v0[1], v1[1]}, 1);
  ret.add_to_key({v1[0], v2[0], v0[0], v2[1], v1[1], v0[1]}, 1);
  ret.add_to_key({v1[0], v2[0], v1[1], v0[0], v0[1], v2[1]}, 1);
  ret.add_to_key({v1[0], v2[0], v1[1], v0[0], v2[1], v0[1]}, 1);
  ret.add_to_key({v1[0], v2[0], v1[1], v2[1], v0[0], v0[1]}, 1);
  ret.add_to_key({v1[0], v2[0], v2[1], v0[0], v0[1], v1[1]}, 1);
  ret.add_to_key({v1[0], v2[0], v2[1], v0[0], v1[1], v0[1]}, 1);
  ret.add_to_key({v1[0], v2[0], v2[1], v1[1], v0[0], v0[1]}, 1);
  ret.add_to_key({v2[0], v0[0], v0[1], v1[0], v1[1], v2[1]}, 1);
  ret.add_to_key({v2[0], v0[0], v0[1], v1[0], v2[1], v1[1]}, 1);
  ret.add_to_key({v2[0], v0[0], v0[1], v2[1], v1[0], v1[1]}, 1);
  ret.add_to_key({v2[0], v0[0], v1[0], v0[1], v1[1], v2[1]}, 1);
  ret.add_to_key({v2[0], v0[0], v1[0], v0[1], v2[1], v1[1]}, 1);
  ret.add_to_key({v2[0], v0[0], v1[0], v1[1], v0[1], v2[1]}, 1);
  ret.add_to_key({v2[0], v0[0], v1[0], v1[1], v2[1], v0[1]}, 1);
  ret.add_to_key({v2[0], v0[0], v1[0], v2[1], v0[1], v1[1]}, 1);
  ret.add_to_key({v2[0], v0[0], v1[0], v2[1], v1[1], v0[1]}, 1);
  ret.add_to_key({v2[0], v0[0], v2[1], v0[1], v1[0], v1[1]}, 1);
  ret.add_to_key({v2[0], v0[0], v2[1], v1[0], v0[1], v1[1]}, 1);
  ret.add_to_key({v2[0], v0[0], v2[1], v1[0], v1[1], v0[1]}, 1);
  ret.add_to_key({v2[0], v1[0], v0[0], v0[1], v1[1], v2[1]}, 1);
  ret.add_to_key({v2[0], v1[0], v0[0], v0[1], v2[1], v1[1]}, 1);
  ret.add_to_key({v2[0], v1[0], v0[0], v1[1], v0[1], v2[1]}, 1);
  ret.add_to_key({v2[0], v1[0], v0[0], v1[1], v2[1], v0[1]}, 1);
  ret.add_to_key({v2[0], v1[0], v0[0], v2[1], v0[1], v1[1]}, 1);
  ret.add_to_key({v2[0], v1[0], v0[0], v2[1], v1[1], v0[1]}, 1);
  ret.add_to_key({v2[0], v1[0], v1[1], v0[0], v0[1], v2[1]}, 1);
  ret.add_to_key({v2[0], v1[0], v1[1], v0[0], v2[1], v0[1]}, 1);
  ret.add_to_key({v2[0], v1[0], v1[1], v2[1], v0[0], v0[1]}, 1);
  ret.add_to_key({v2[0], v1[0], v2[1], v0[0], v0[1], v1[1]}, 1);
  ret.add_to_key({v2[0], v1[0], v2[1], v0[0], v1[1], v0[1]}, 1);
  ret.add_to_key({v2[0], v1[0], v2[1], v1[1], v0[0], v0[1]}, 1);
  ret.add_to_key({v2[0], v2[1], v0[0], v0[1], v1[0], v1[1]}, 1);
  ret.add_to_key({v2[0], v2[1], v0[0], v1[0], v0[1], v1[1]}, 1);
  ret.add_to_key({v2[0], v2[1], v0[0], v1[0], v1[1], v0[1]}, 1);
  ret.add_to_key({v2[0], v2[1], v1[0], v0[0], v0[1], v1[1]}, 1);
  ret.add_to_key({v2[0], v2[1], v1[0], v0[0], v1[1], v0[1]}, 1);
  ret.add_to_key({v2[0], v2[1], v1[0], v1[1], v0[0], v0[1]}, 1);
  return ret;
}

template<typename MonomT>
Linear<SimpleLinearParam<MonomT>> shuffle_product_unrolled_multi(std::vector<MonomT>& v) {
  if (v.size() != 3) {
    return {};
  }
  absl::c_sort(v, [](const auto& a, const auto& b) { return a.size() < b.size(); });
  switch (v[0].size()) {
    case 1:
      switch (v[1].size()) {
        case 1:
          switch (v[2].size()) {
            case 1: return shuffle_product_unrolled_1_1_1(v[0], v[1], v[2]);
            case 2: return shuffle_product_unrolled_1_1_2(v[0], v[1], v[2]);
            case 3: return shuffle_product_unrolled_1_1_3(v[0], v[1], v[2]);
            case 4: return shuffle_product_unrolled_1_1_4(v[0], v[1], v[2]);
          }
          break;
        case 2:
          switch (v[2].size()) {
            case 2: return shuffle_product_unrolled_1_2_2(v[0], v[1], v[2]);
            case 3: return shuffle_product_unrolled_1_2_3(v[0], v[1], v[2]);
          }
          break;
      }
      break;
    case 2:
      switch (v[1].size()) {
        case 2:
          switch (v[2].size()) {
            case 2: return shuffle_product_unrolled_2_2_2(v[0], v[1], v[2]);
          }
          break;
      }
      break;
  }
  return {};
}

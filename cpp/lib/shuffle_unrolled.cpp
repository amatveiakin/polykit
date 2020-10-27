// TODO: Sort words by lenght and remove half of the functions.
// TODO: Pre-convert to char and make sure WordExpr::single does a simple memcpy.

#include "shuffle_unrolled.h"


#ifdef UNROLL_SHUFFLE

WordExpr shuffle_product_unrolled_1_1(const Word& u, const Word& v) {
  return (
     + WordExpr::single({u[0], v[0]})
     + WordExpr::single({v[0], u[0]})
  );
}

WordExpr shuffle_product_unrolled_1_2(const Word& u, const Word& v) {
  return (
     + WordExpr::single({u[0], v[0], v[1]})
     + WordExpr::single({v[0], u[0], v[1]})
     + WordExpr::single({v[0], v[1], u[0]})
  );
}

WordExpr shuffle_product_unrolled_1_3(const Word& u, const Word& v) {
  return (
     + WordExpr::single({u[0], v[0], v[1], v[2]})
     + WordExpr::single({v[0], u[0], v[1], v[2]})
     + WordExpr::single({v[0], v[1], u[0], v[2]})
     + WordExpr::single({v[0], v[1], v[2], u[0]})
  );
}

WordExpr shuffle_product_unrolled_1_4(const Word& u, const Word& v) {
  return (
     + WordExpr::single({u[0], v[0], v[1], v[2], v[3]})
     + WordExpr::single({v[0], u[0], v[1], v[2], v[3]})
     + WordExpr::single({v[0], v[1], u[0], v[2], v[3]})
     + WordExpr::single({v[0], v[1], v[2], u[0], v[3]})
     + WordExpr::single({v[0], v[1], v[2], v[3], u[0]})
  );
}

WordExpr shuffle_product_unrolled_1_5(const Word& u, const Word& v) {
  return (
     + WordExpr::single({u[0], v[0], v[1], v[2], v[3], v[4]})
     + WordExpr::single({v[0], u[0], v[1], v[2], v[3], v[4]})
     + WordExpr::single({v[0], v[1], u[0], v[2], v[3], v[4]})
     + WordExpr::single({v[0], v[1], v[2], u[0], v[3], v[4]})
     + WordExpr::single({v[0], v[1], v[2], v[3], u[0], v[4]})
     + WordExpr::single({v[0], v[1], v[2], v[3], v[4], u[0]})
  );
}

WordExpr shuffle_product_unrolled_1_6(const Word& u, const Word& v) {
  return (
     + WordExpr::single({u[0], v[0], v[1], v[2], v[3], v[4], v[5]})
     + WordExpr::single({v[0], u[0], v[1], v[2], v[3], v[4], v[5]})
     + WordExpr::single({v[0], v[1], u[0], v[2], v[3], v[4], v[5]})
     + WordExpr::single({v[0], v[1], v[2], u[0], v[3], v[4], v[5]})
     + WordExpr::single({v[0], v[1], v[2], v[3], u[0], v[4], v[5]})
     + WordExpr::single({v[0], v[1], v[2], v[3], v[4], u[0], v[5]})
     + WordExpr::single({v[0], v[1], v[2], v[3], v[4], v[5], u[0]})
  );
}

WordExpr shuffle_product_unrolled_1_7(const Word& u, const Word& v) {
  return (
     + WordExpr::single({u[0], v[0], v[1], v[2], v[3], v[4], v[5], v[6]})
     + WordExpr::single({v[0], u[0], v[1], v[2], v[3], v[4], v[5], v[6]})
     + WordExpr::single({v[0], v[1], u[0], v[2], v[3], v[4], v[5], v[6]})
     + WordExpr::single({v[0], v[1], v[2], u[0], v[3], v[4], v[5], v[6]})
     + WordExpr::single({v[0], v[1], v[2], v[3], u[0], v[4], v[5], v[6]})
     + WordExpr::single({v[0], v[1], v[2], v[3], v[4], u[0], v[5], v[6]})
     + WordExpr::single({v[0], v[1], v[2], v[3], v[4], v[5], u[0], v[6]})
     + WordExpr::single({v[0], v[1], v[2], v[3], v[4], v[5], v[6], u[0]})
  );
}

WordExpr shuffle_product_unrolled_2_2(const Word& u, const Word& v) {
  return (
     + WordExpr::single({u[0], u[1], v[0], v[1]})
     + WordExpr::single({u[0], v[0], u[1], v[1]})
     + WordExpr::single({u[0], v[0], v[1], u[1]})
     + WordExpr::single({v[0], u[0], u[1], v[1]})
     + WordExpr::single({v[0], u[0], v[1], u[1]})
     + WordExpr::single({v[0], v[1], u[0], u[1]})
  );
}

WordExpr shuffle_product_unrolled_2_3(const Word& u, const Word& v) {
  return (
     + WordExpr::single({u[0], u[1], v[0], v[1], v[2]})
     + WordExpr::single({u[0], v[0], u[1], v[1], v[2]})
     + WordExpr::single({u[0], v[0], v[1], u[1], v[2]})
     + WordExpr::single({u[0], v[0], v[1], v[2], u[1]})
     + WordExpr::single({v[0], u[0], u[1], v[1], v[2]})
     + WordExpr::single({v[0], u[0], v[1], u[1], v[2]})
     + WordExpr::single({v[0], u[0], v[1], v[2], u[1]})
     + WordExpr::single({v[0], v[1], u[0], u[1], v[2]})
     + WordExpr::single({v[0], v[1], u[0], v[2], u[1]})
     + WordExpr::single({v[0], v[1], v[2], u[0], u[1]})
  );
}

WordExpr shuffle_product_unrolled_2_4(const Word& u, const Word& v) {
  return (
     + WordExpr::single({u[0], u[1], v[0], v[1], v[2], v[3]})
     + WordExpr::single({u[0], v[0], u[1], v[1], v[2], v[3]})
     + WordExpr::single({u[0], v[0], v[1], u[1], v[2], v[3]})
     + WordExpr::single({u[0], v[0], v[1], v[2], u[1], v[3]})
     + WordExpr::single({u[0], v[0], v[1], v[2], v[3], u[1]})
     + WordExpr::single({v[0], u[0], u[1], v[1], v[2], v[3]})
     + WordExpr::single({v[0], u[0], v[1], u[1], v[2], v[3]})
     + WordExpr::single({v[0], u[0], v[1], v[2], u[1], v[3]})
     + WordExpr::single({v[0], u[0], v[1], v[2], v[3], u[1]})
     + WordExpr::single({v[0], v[1], u[0], u[1], v[2], v[3]})
     + WordExpr::single({v[0], v[1], u[0], v[2], u[1], v[3]})
     + WordExpr::single({v[0], v[1], u[0], v[2], v[3], u[1]})
     + WordExpr::single({v[0], v[1], v[2], u[0], u[1], v[3]})
     + WordExpr::single({v[0], v[1], v[2], u[0], v[3], u[1]})
     + WordExpr::single({v[0], v[1], v[2], v[3], u[0], u[1]})
  );
}

WordExpr shuffle_product_unrolled_2_5(const Word& u, const Word& v) {
  return (
     + WordExpr::single({u[0], u[1], v[0], v[1], v[2], v[3], v[4]})
     + WordExpr::single({u[0], v[0], u[1], v[1], v[2], v[3], v[4]})
     + WordExpr::single({u[0], v[0], v[1], u[1], v[2], v[3], v[4]})
     + WordExpr::single({u[0], v[0], v[1], v[2], u[1], v[3], v[4]})
     + WordExpr::single({u[0], v[0], v[1], v[2], v[3], u[1], v[4]})
     + WordExpr::single({u[0], v[0], v[1], v[2], v[3], v[4], u[1]})
     + WordExpr::single({v[0], u[0], u[1], v[1], v[2], v[3], v[4]})
     + WordExpr::single({v[0], u[0], v[1], u[1], v[2], v[3], v[4]})
     + WordExpr::single({v[0], u[0], v[1], v[2], u[1], v[3], v[4]})
     + WordExpr::single({v[0], u[0], v[1], v[2], v[3], u[1], v[4]})
     + WordExpr::single({v[0], u[0], v[1], v[2], v[3], v[4], u[1]})
     + WordExpr::single({v[0], v[1], u[0], u[1], v[2], v[3], v[4]})
     + WordExpr::single({v[0], v[1], u[0], v[2], u[1], v[3], v[4]})
     + WordExpr::single({v[0], v[1], u[0], v[2], v[3], u[1], v[4]})
     + WordExpr::single({v[0], v[1], u[0], v[2], v[3], v[4], u[1]})
     + WordExpr::single({v[0], v[1], v[2], u[0], u[1], v[3], v[4]})
     + WordExpr::single({v[0], v[1], v[2], u[0], v[3], u[1], v[4]})
     + WordExpr::single({v[0], v[1], v[2], u[0], v[3], v[4], u[1]})
     + WordExpr::single({v[0], v[1], v[2], v[3], u[0], u[1], v[4]})
     + WordExpr::single({v[0], v[1], v[2], v[3], u[0], v[4], u[1]})
     + WordExpr::single({v[0], v[1], v[2], v[3], v[4], u[0], u[1]})
  );
}

WordExpr shuffle_product_unrolled_2_6(const Word& u, const Word& v) {
  return (
     + WordExpr::single({u[0], u[1], v[0], v[1], v[2], v[3], v[4], v[5]})
     + WordExpr::single({u[0], v[0], u[1], v[1], v[2], v[3], v[4], v[5]})
     + WordExpr::single({u[0], v[0], v[1], u[1], v[2], v[3], v[4], v[5]})
     + WordExpr::single({u[0], v[0], v[1], v[2], u[1], v[3], v[4], v[5]})
     + WordExpr::single({u[0], v[0], v[1], v[2], v[3], u[1], v[4], v[5]})
     + WordExpr::single({u[0], v[0], v[1], v[2], v[3], v[4], u[1], v[5]})
     + WordExpr::single({u[0], v[0], v[1], v[2], v[3], v[4], v[5], u[1]})
     + WordExpr::single({v[0], u[0], u[1], v[1], v[2], v[3], v[4], v[5]})
     + WordExpr::single({v[0], u[0], v[1], u[1], v[2], v[3], v[4], v[5]})
     + WordExpr::single({v[0], u[0], v[1], v[2], u[1], v[3], v[4], v[5]})
     + WordExpr::single({v[0], u[0], v[1], v[2], v[3], u[1], v[4], v[5]})
     + WordExpr::single({v[0], u[0], v[1], v[2], v[3], v[4], u[1], v[5]})
     + WordExpr::single({v[0], u[0], v[1], v[2], v[3], v[4], v[5], u[1]})
     + WordExpr::single({v[0], v[1], u[0], u[1], v[2], v[3], v[4], v[5]})
     + WordExpr::single({v[0], v[1], u[0], v[2], u[1], v[3], v[4], v[5]})
     + WordExpr::single({v[0], v[1], u[0], v[2], v[3], u[1], v[4], v[5]})
     + WordExpr::single({v[0], v[1], u[0], v[2], v[3], v[4], u[1], v[5]})
     + WordExpr::single({v[0], v[1], u[0], v[2], v[3], v[4], v[5], u[1]})
     + WordExpr::single({v[0], v[1], v[2], u[0], u[1], v[3], v[4], v[5]})
     + WordExpr::single({v[0], v[1], v[2], u[0], v[3], u[1], v[4], v[5]})
     + WordExpr::single({v[0], v[1], v[2], u[0], v[3], v[4], u[1], v[5]})
     + WordExpr::single({v[0], v[1], v[2], u[0], v[3], v[4], v[5], u[1]})
     + WordExpr::single({v[0], v[1], v[2], v[3], u[0], u[1], v[4], v[5]})
     + WordExpr::single({v[0], v[1], v[2], v[3], u[0], v[4], u[1], v[5]})
     + WordExpr::single({v[0], v[1], v[2], v[3], u[0], v[4], v[5], u[1]})
     + WordExpr::single({v[0], v[1], v[2], v[3], v[4], u[0], u[1], v[5]})
     + WordExpr::single({v[0], v[1], v[2], v[3], v[4], u[0], v[5], u[1]})
     + WordExpr::single({v[0], v[1], v[2], v[3], v[4], v[5], u[0], u[1]})
  );
}

WordExpr shuffle_product_unrolled_3_3(const Word& u, const Word& v) {
  return (
     + WordExpr::single({u[0], u[1], u[2], v[0], v[1], v[2]})
     + WordExpr::single({u[0], u[1], v[0], u[2], v[1], v[2]})
     + WordExpr::single({u[0], u[1], v[0], v[1], u[2], v[2]})
     + WordExpr::single({u[0], u[1], v[0], v[1], v[2], u[2]})
     + WordExpr::single({u[0], v[0], u[1], u[2], v[1], v[2]})
     + WordExpr::single({u[0], v[0], u[1], v[1], u[2], v[2]})
     + WordExpr::single({u[0], v[0], u[1], v[1], v[2], u[2]})
     + WordExpr::single({u[0], v[0], v[1], u[1], u[2], v[2]})
     + WordExpr::single({u[0], v[0], v[1], u[1], v[2], u[2]})
     + WordExpr::single({u[0], v[0], v[1], v[2], u[1], u[2]})
     + WordExpr::single({v[0], u[0], u[1], u[2], v[1], v[2]})
     + WordExpr::single({v[0], u[0], u[1], v[1], u[2], v[2]})
     + WordExpr::single({v[0], u[0], u[1], v[1], v[2], u[2]})
     + WordExpr::single({v[0], u[0], v[1], u[1], u[2], v[2]})
     + WordExpr::single({v[0], u[0], v[1], u[1], v[2], u[2]})
     + WordExpr::single({v[0], u[0], v[1], v[2], u[1], u[2]})
     + WordExpr::single({v[0], v[1], u[0], u[1], u[2], v[2]})
     + WordExpr::single({v[0], v[1], u[0], u[1], v[2], u[2]})
     + WordExpr::single({v[0], v[1], u[0], v[2], u[1], u[2]})
     + WordExpr::single({v[0], v[1], v[2], u[0], u[1], u[2]})
  );
}

WordExpr shuffle_product_unrolled_3_4(const Word& u, const Word& v) {
  return (
     + WordExpr::single({u[0], u[1], u[2], v[0], v[1], v[2], v[3]})
     + WordExpr::single({u[0], u[1], v[0], u[2], v[1], v[2], v[3]})
     + WordExpr::single({u[0], u[1], v[0], v[1], u[2], v[2], v[3]})
     + WordExpr::single({u[0], u[1], v[0], v[1], v[2], u[2], v[3]})
     + WordExpr::single({u[0], u[1], v[0], v[1], v[2], v[3], u[2]})
     + WordExpr::single({u[0], v[0], u[1], u[2], v[1], v[2], v[3]})
     + WordExpr::single({u[0], v[0], u[1], v[1], u[2], v[2], v[3]})
     + WordExpr::single({u[0], v[0], u[1], v[1], v[2], u[2], v[3]})
     + WordExpr::single({u[0], v[0], u[1], v[1], v[2], v[3], u[2]})
     + WordExpr::single({u[0], v[0], v[1], u[1], u[2], v[2], v[3]})
     + WordExpr::single({u[0], v[0], v[1], u[1], v[2], u[2], v[3]})
     + WordExpr::single({u[0], v[0], v[1], u[1], v[2], v[3], u[2]})
     + WordExpr::single({u[0], v[0], v[1], v[2], u[1], u[2], v[3]})
     + WordExpr::single({u[0], v[0], v[1], v[2], u[1], v[3], u[2]})
     + WordExpr::single({u[0], v[0], v[1], v[2], v[3], u[1], u[2]})
     + WordExpr::single({v[0], u[0], u[1], u[2], v[1], v[2], v[3]})
     + WordExpr::single({v[0], u[0], u[1], v[1], u[2], v[2], v[3]})
     + WordExpr::single({v[0], u[0], u[1], v[1], v[2], u[2], v[3]})
     + WordExpr::single({v[0], u[0], u[1], v[1], v[2], v[3], u[2]})
     + WordExpr::single({v[0], u[0], v[1], u[1], u[2], v[2], v[3]})
     + WordExpr::single({v[0], u[0], v[1], u[1], v[2], u[2], v[3]})
     + WordExpr::single({v[0], u[0], v[1], u[1], v[2], v[3], u[2]})
     + WordExpr::single({v[0], u[0], v[1], v[2], u[1], u[2], v[3]})
     + WordExpr::single({v[0], u[0], v[1], v[2], u[1], v[3], u[2]})
     + WordExpr::single({v[0], u[0], v[1], v[2], v[3], u[1], u[2]})
     + WordExpr::single({v[0], v[1], u[0], u[1], u[2], v[2], v[3]})
     + WordExpr::single({v[0], v[1], u[0], u[1], v[2], u[2], v[3]})
     + WordExpr::single({v[0], v[1], u[0], u[1], v[2], v[3], u[2]})
     + WordExpr::single({v[0], v[1], u[0], v[2], u[1], u[2], v[3]})
     + WordExpr::single({v[0], v[1], u[0], v[2], u[1], v[3], u[2]})
     + WordExpr::single({v[0], v[1], u[0], v[2], v[3], u[1], u[2]})
     + WordExpr::single({v[0], v[1], v[2], u[0], u[1], u[2], v[3]})
     + WordExpr::single({v[0], v[1], v[2], u[0], u[1], v[3], u[2]})
     + WordExpr::single({v[0], v[1], v[2], u[0], v[3], u[1], u[2]})
     + WordExpr::single({v[0], v[1], v[2], v[3], u[0], u[1], u[2]})
  );
}

WordExpr shuffle_product_unrolled_3_5(const Word& u, const Word& v) {
  return (
     + WordExpr::single({u[0], u[1], u[2], v[0], v[1], v[2], v[3], v[4]})
     + WordExpr::single({u[0], u[1], v[0], u[2], v[1], v[2], v[3], v[4]})
     + WordExpr::single({u[0], u[1], v[0], v[1], u[2], v[2], v[3], v[4]})
     + WordExpr::single({u[0], u[1], v[0], v[1], v[2], u[2], v[3], v[4]})
     + WordExpr::single({u[0], u[1], v[0], v[1], v[2], v[3], u[2], v[4]})
     + WordExpr::single({u[0], u[1], v[0], v[1], v[2], v[3], v[4], u[2]})
     + WordExpr::single({u[0], v[0], u[1], u[2], v[1], v[2], v[3], v[4]})
     + WordExpr::single({u[0], v[0], u[1], v[1], u[2], v[2], v[3], v[4]})
     + WordExpr::single({u[0], v[0], u[1], v[1], v[2], u[2], v[3], v[4]})
     + WordExpr::single({u[0], v[0], u[1], v[1], v[2], v[3], u[2], v[4]})
     + WordExpr::single({u[0], v[0], u[1], v[1], v[2], v[3], v[4], u[2]})
     + WordExpr::single({u[0], v[0], v[1], u[1], u[2], v[2], v[3], v[4]})
     + WordExpr::single({u[0], v[0], v[1], u[1], v[2], u[2], v[3], v[4]})
     + WordExpr::single({u[0], v[0], v[1], u[1], v[2], v[3], u[2], v[4]})
     + WordExpr::single({u[0], v[0], v[1], u[1], v[2], v[3], v[4], u[2]})
     + WordExpr::single({u[0], v[0], v[1], v[2], u[1], u[2], v[3], v[4]})
     + WordExpr::single({u[0], v[0], v[1], v[2], u[1], v[3], u[2], v[4]})
     + WordExpr::single({u[0], v[0], v[1], v[2], u[1], v[3], v[4], u[2]})
     + WordExpr::single({u[0], v[0], v[1], v[2], v[3], u[1], u[2], v[4]})
     + WordExpr::single({u[0], v[0], v[1], v[2], v[3], u[1], v[4], u[2]})
     + WordExpr::single({u[0], v[0], v[1], v[2], v[3], v[4], u[1], u[2]})
     + WordExpr::single({v[0], u[0], u[1], u[2], v[1], v[2], v[3], v[4]})
     + WordExpr::single({v[0], u[0], u[1], v[1], u[2], v[2], v[3], v[4]})
     + WordExpr::single({v[0], u[0], u[1], v[1], v[2], u[2], v[3], v[4]})
     + WordExpr::single({v[0], u[0], u[1], v[1], v[2], v[3], u[2], v[4]})
     + WordExpr::single({v[0], u[0], u[1], v[1], v[2], v[3], v[4], u[2]})
     + WordExpr::single({v[0], u[0], v[1], u[1], u[2], v[2], v[3], v[4]})
     + WordExpr::single({v[0], u[0], v[1], u[1], v[2], u[2], v[3], v[4]})
     + WordExpr::single({v[0], u[0], v[1], u[1], v[2], v[3], u[2], v[4]})
     + WordExpr::single({v[0], u[0], v[1], u[1], v[2], v[3], v[4], u[2]})
     + WordExpr::single({v[0], u[0], v[1], v[2], u[1], u[2], v[3], v[4]})
     + WordExpr::single({v[0], u[0], v[1], v[2], u[1], v[3], u[2], v[4]})
     + WordExpr::single({v[0], u[0], v[1], v[2], u[1], v[3], v[4], u[2]})
     + WordExpr::single({v[0], u[0], v[1], v[2], v[3], u[1], u[2], v[4]})
     + WordExpr::single({v[0], u[0], v[1], v[2], v[3], u[1], v[4], u[2]})
     + WordExpr::single({v[0], u[0], v[1], v[2], v[3], v[4], u[1], u[2]})
     + WordExpr::single({v[0], v[1], u[0], u[1], u[2], v[2], v[3], v[4]})
     + WordExpr::single({v[0], v[1], u[0], u[1], v[2], u[2], v[3], v[4]})
     + WordExpr::single({v[0], v[1], u[0], u[1], v[2], v[3], u[2], v[4]})
     + WordExpr::single({v[0], v[1], u[0], u[1], v[2], v[3], v[4], u[2]})
     + WordExpr::single({v[0], v[1], u[0], v[2], u[1], u[2], v[3], v[4]})
     + WordExpr::single({v[0], v[1], u[0], v[2], u[1], v[3], u[2], v[4]})
     + WordExpr::single({v[0], v[1], u[0], v[2], u[1], v[3], v[4], u[2]})
     + WordExpr::single({v[0], v[1], u[0], v[2], v[3], u[1], u[2], v[4]})
     + WordExpr::single({v[0], v[1], u[0], v[2], v[3], u[1], v[4], u[2]})
     + WordExpr::single({v[0], v[1], u[0], v[2], v[3], v[4], u[1], u[2]})
     + WordExpr::single({v[0], v[1], v[2], u[0], u[1], u[2], v[3], v[4]})
     + WordExpr::single({v[0], v[1], v[2], u[0], u[1], v[3], u[2], v[4]})
     + WordExpr::single({v[0], v[1], v[2], u[0], u[1], v[3], v[4], u[2]})
     + WordExpr::single({v[0], v[1], v[2], u[0], v[3], u[1], u[2], v[4]})
     + WordExpr::single({v[0], v[1], v[2], u[0], v[3], u[1], v[4], u[2]})
     + WordExpr::single({v[0], v[1], v[2], u[0], v[3], v[4], u[1], u[2]})
     + WordExpr::single({v[0], v[1], v[2], v[3], u[0], u[1], u[2], v[4]})
     + WordExpr::single({v[0], v[1], v[2], v[3], u[0], u[1], v[4], u[2]})
     + WordExpr::single({v[0], v[1], v[2], v[3], u[0], v[4], u[1], u[2]})
     + WordExpr::single({v[0], v[1], v[2], v[3], v[4], u[0], u[1], u[2]})
  );
}

WordExpr shuffle_product_unrolled_4_4(const Word& u, const Word& v) {
  return (
     + WordExpr::single({u[0], u[1], u[2], u[3], v[0], v[1], v[2], v[3]})
     + WordExpr::single({u[0], u[1], u[2], v[0], u[3], v[1], v[2], v[3]})
     + WordExpr::single({u[0], u[1], u[2], v[0], v[1], u[3], v[2], v[3]})
     + WordExpr::single({u[0], u[1], u[2], v[0], v[1], v[2], u[3], v[3]})
     + WordExpr::single({u[0], u[1], u[2], v[0], v[1], v[2], v[3], u[3]})
     + WordExpr::single({u[0], u[1], v[0], u[2], u[3], v[1], v[2], v[3]})
     + WordExpr::single({u[0], u[1], v[0], u[2], v[1], u[3], v[2], v[3]})
     + WordExpr::single({u[0], u[1], v[0], u[2], v[1], v[2], u[3], v[3]})
     + WordExpr::single({u[0], u[1], v[0], u[2], v[1], v[2], v[3], u[3]})
     + WordExpr::single({u[0], u[1], v[0], v[1], u[2], u[3], v[2], v[3]})
     + WordExpr::single({u[0], u[1], v[0], v[1], u[2], v[2], u[3], v[3]})
     + WordExpr::single({u[0], u[1], v[0], v[1], u[2], v[2], v[3], u[3]})
     + WordExpr::single({u[0], u[1], v[0], v[1], v[2], u[2], u[3], v[3]})
     + WordExpr::single({u[0], u[1], v[0], v[1], v[2], u[2], v[3], u[3]})
     + WordExpr::single({u[0], u[1], v[0], v[1], v[2], v[3], u[2], u[3]})
     + WordExpr::single({u[0], v[0], u[1], u[2], u[3], v[1], v[2], v[3]})
     + WordExpr::single({u[0], v[0], u[1], u[2], v[1], u[3], v[2], v[3]})
     + WordExpr::single({u[0], v[0], u[1], u[2], v[1], v[2], u[3], v[3]})
     + WordExpr::single({u[0], v[0], u[1], u[2], v[1], v[2], v[3], u[3]})
     + WordExpr::single({u[0], v[0], u[1], v[1], u[2], u[3], v[2], v[3]})
     + WordExpr::single({u[0], v[0], u[1], v[1], u[2], v[2], u[3], v[3]})
     + WordExpr::single({u[0], v[0], u[1], v[1], u[2], v[2], v[3], u[3]})
     + WordExpr::single({u[0], v[0], u[1], v[1], v[2], u[2], u[3], v[3]})
     + WordExpr::single({u[0], v[0], u[1], v[1], v[2], u[2], v[3], u[3]})
     + WordExpr::single({u[0], v[0], u[1], v[1], v[2], v[3], u[2], u[3]})
     + WordExpr::single({u[0], v[0], v[1], u[1], u[2], u[3], v[2], v[3]})
     + WordExpr::single({u[0], v[0], v[1], u[1], u[2], v[2], u[3], v[3]})
     + WordExpr::single({u[0], v[0], v[1], u[1], u[2], v[2], v[3], u[3]})
     + WordExpr::single({u[0], v[0], v[1], u[1], v[2], u[2], u[3], v[3]})
     + WordExpr::single({u[0], v[0], v[1], u[1], v[2], u[2], v[3], u[3]})
     + WordExpr::single({u[0], v[0], v[1], u[1], v[2], v[3], u[2], u[3]})
     + WordExpr::single({u[0], v[0], v[1], v[2], u[1], u[2], u[3], v[3]})
     + WordExpr::single({u[0], v[0], v[1], v[2], u[1], u[2], v[3], u[3]})
     + WordExpr::single({u[0], v[0], v[1], v[2], u[1], v[3], u[2], u[3]})
     + WordExpr::single({u[0], v[0], v[1], v[2], v[3], u[1], u[2], u[3]})
     + WordExpr::single({v[0], u[0], u[1], u[2], u[3], v[1], v[2], v[3]})
     + WordExpr::single({v[0], u[0], u[1], u[2], v[1], u[3], v[2], v[3]})
     + WordExpr::single({v[0], u[0], u[1], u[2], v[1], v[2], u[3], v[3]})
     + WordExpr::single({v[0], u[0], u[1], u[2], v[1], v[2], v[3], u[3]})
     + WordExpr::single({v[0], u[0], u[1], v[1], u[2], u[3], v[2], v[3]})
     + WordExpr::single({v[0], u[0], u[1], v[1], u[2], v[2], u[3], v[3]})
     + WordExpr::single({v[0], u[0], u[1], v[1], u[2], v[2], v[3], u[3]})
     + WordExpr::single({v[0], u[0], u[1], v[1], v[2], u[2], u[3], v[3]})
     + WordExpr::single({v[0], u[0], u[1], v[1], v[2], u[2], v[3], u[3]})
     + WordExpr::single({v[0], u[0], u[1], v[1], v[2], v[3], u[2], u[3]})
     + WordExpr::single({v[0], u[0], v[1], u[1], u[2], u[3], v[2], v[3]})
     + WordExpr::single({v[0], u[0], v[1], u[1], u[2], v[2], u[3], v[3]})
     + WordExpr::single({v[0], u[0], v[1], u[1], u[2], v[2], v[3], u[3]})
     + WordExpr::single({v[0], u[0], v[1], u[1], v[2], u[2], u[3], v[3]})
     + WordExpr::single({v[0], u[0], v[1], u[1], v[2], u[2], v[3], u[3]})
     + WordExpr::single({v[0], u[0], v[1], u[1], v[2], v[3], u[2], u[3]})
     + WordExpr::single({v[0], u[0], v[1], v[2], u[1], u[2], u[3], v[3]})
     + WordExpr::single({v[0], u[0], v[1], v[2], u[1], u[2], v[3], u[3]})
     + WordExpr::single({v[0], u[0], v[1], v[2], u[1], v[3], u[2], u[3]})
     + WordExpr::single({v[0], u[0], v[1], v[2], v[3], u[1], u[2], u[3]})
     + WordExpr::single({v[0], v[1], u[0], u[1], u[2], u[3], v[2], v[3]})
     + WordExpr::single({v[0], v[1], u[0], u[1], u[2], v[2], u[3], v[3]})
     + WordExpr::single({v[0], v[1], u[0], u[1], u[2], v[2], v[3], u[3]})
     + WordExpr::single({v[0], v[1], u[0], u[1], v[2], u[2], u[3], v[3]})
     + WordExpr::single({v[0], v[1], u[0], u[1], v[2], u[2], v[3], u[3]})
     + WordExpr::single({v[0], v[1], u[0], u[1], v[2], v[3], u[2], u[3]})
     + WordExpr::single({v[0], v[1], u[0], v[2], u[1], u[2], u[3], v[3]})
     + WordExpr::single({v[0], v[1], u[0], v[2], u[1], u[2], v[3], u[3]})
     + WordExpr::single({v[0], v[1], u[0], v[2], u[1], v[3], u[2], u[3]})
     + WordExpr::single({v[0], v[1], u[0], v[2], v[3], u[1], u[2], u[3]})
     + WordExpr::single({v[0], v[1], v[2], u[0], u[1], u[2], u[3], v[3]})
     + WordExpr::single({v[0], v[1], v[2], u[0], u[1], u[2], v[3], u[3]})
     + WordExpr::single({v[0], v[1], v[2], u[0], u[1], v[3], u[2], u[3]})
     + WordExpr::single({v[0], v[1], v[2], u[0], v[3], u[1], u[2], u[3]})
     + WordExpr::single({v[0], v[1], v[2], v[3], u[0], u[1], u[2], u[3]})
  );
}

WordExpr shuffle_product_unrolled(Word u, Word v) {
  if (u.size() > v.size()) {
    std::swap(u, v);
  }
  switch (u.size()) {
    case 1:
    switch (v.size()) {
      case  1: return shuffle_product_unrolled_1_1(u, v);
      case  2: return shuffle_product_unrolled_1_2(u, v);
      case  3: return shuffle_product_unrolled_1_3(u, v);
      case  4: return shuffle_product_unrolled_1_4(u, v);
      case  5: return shuffle_product_unrolled_1_5(u, v);
      case  6: return shuffle_product_unrolled_1_6(u, v);
      case  7: return shuffle_product_unrolled_1_7(u, v);
    }
    break;
    case 2:
    switch (v.size()) {
      case  2: return shuffle_product_unrolled_2_2(u, v);
      case  3: return shuffle_product_unrolled_2_3(u, v);
      case  4: return shuffle_product_unrolled_2_4(u, v);
      case  5: return shuffle_product_unrolled_2_5(u, v);
      case  6: return shuffle_product_unrolled_2_6(u, v);
    }
    break;
    case 3:
    switch (v.size()) {
      case  3: return shuffle_product_unrolled_3_3(u, v);
      case  4: return shuffle_product_unrolled_3_4(u, v);
      case  5: return shuffle_product_unrolled_3_5(u, v);
    }
    break;
    case 4:
    switch (v.size()) {
      case  4: return shuffle_product_unrolled_4_4(u, v);
    }
    break;
  }
  return {};
}

#endif

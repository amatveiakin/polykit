#include "shuffle_unrolled.h"


IntWordExpr shuffle_product_unrolled_1_1(const IntWord& u, const IntWord& v) {
  return (
     + IntWordExpr::single({u[0], v[0]})
     + IntWordExpr::single({v[0], u[0]})
  );
}

IntWordExpr shuffle_product_unrolled_1_2(const IntWord& u, const IntWord& v) {
  return (
     + IntWordExpr::single({u[0], v[0], v[1]})
     + IntWordExpr::single({v[0], u[0], v[1]})
     + IntWordExpr::single({v[0], v[1], u[0]})
  );
}

IntWordExpr shuffle_product_unrolled_1_3(const IntWord& u, const IntWord& v) {
  return (
     + IntWordExpr::single({u[0], v[0], v[1], v[2]})
     + IntWordExpr::single({v[0], u[0], v[1], v[2]})
     + IntWordExpr::single({v[0], v[1], u[0], v[2]})
     + IntWordExpr::single({v[0], v[1], v[2], u[0]})
  );
}

IntWordExpr shuffle_product_unrolled_1_4(const IntWord& u, const IntWord& v) {
  return (
     + IntWordExpr::single({u[0], v[0], v[1], v[2], v[3]})
     + IntWordExpr::single({v[0], u[0], v[1], v[2], v[3]})
     + IntWordExpr::single({v[0], v[1], u[0], v[2], v[3]})
     + IntWordExpr::single({v[0], v[1], v[2], u[0], v[3]})
     + IntWordExpr::single({v[0], v[1], v[2], v[3], u[0]})
  );
}

IntWordExpr shuffle_product_unrolled_1_5(const IntWord& u, const IntWord& v) {
  return (
     + IntWordExpr::single({u[0], v[0], v[1], v[2], v[3], v[4]})
     + IntWordExpr::single({v[0], u[0], v[1], v[2], v[3], v[4]})
     + IntWordExpr::single({v[0], v[1], u[0], v[2], v[3], v[4]})
     + IntWordExpr::single({v[0], v[1], v[2], u[0], v[3], v[4]})
     + IntWordExpr::single({v[0], v[1], v[2], v[3], u[0], v[4]})
     + IntWordExpr::single({v[0], v[1], v[2], v[3], v[4], u[0]})
  );
}

IntWordExpr shuffle_product_unrolled_1_6(const IntWord& u, const IntWord& v) {
  return (
     + IntWordExpr::single({u[0], v[0], v[1], v[2], v[3], v[4], v[5]})
     + IntWordExpr::single({v[0], u[0], v[1], v[2], v[3], v[4], v[5]})
     + IntWordExpr::single({v[0], v[1], u[0], v[2], v[3], v[4], v[5]})
     + IntWordExpr::single({v[0], v[1], v[2], u[0], v[3], v[4], v[5]})
     + IntWordExpr::single({v[0], v[1], v[2], v[3], u[0], v[4], v[5]})
     + IntWordExpr::single({v[0], v[1], v[2], v[3], v[4], u[0], v[5]})
     + IntWordExpr::single({v[0], v[1], v[2], v[3], v[4], v[5], u[0]})
  );
}

IntWordExpr shuffle_product_unrolled_1_7(const IntWord& u, const IntWord& v) {
  return (
     + IntWordExpr::single({u[0], v[0], v[1], v[2], v[3], v[4], v[5], v[6]})
     + IntWordExpr::single({v[0], u[0], v[1], v[2], v[3], v[4], v[5], v[6]})
     + IntWordExpr::single({v[0], v[1], u[0], v[2], v[3], v[4], v[5], v[6]})
     + IntWordExpr::single({v[0], v[1], v[2], u[0], v[3], v[4], v[5], v[6]})
     + IntWordExpr::single({v[0], v[1], v[2], v[3], u[0], v[4], v[5], v[6]})
     + IntWordExpr::single({v[0], v[1], v[2], v[3], v[4], u[0], v[5], v[6]})
     + IntWordExpr::single({v[0], v[1], v[2], v[3], v[4], v[5], u[0], v[6]})
     + IntWordExpr::single({v[0], v[1], v[2], v[3], v[4], v[5], v[6], u[0]})
  );
}

IntWordExpr shuffle_product_unrolled_2_1(const IntWord& u, const IntWord& v) {
  return (
     + IntWordExpr::single({u[0], u[1], v[0]})
     + IntWordExpr::single({u[0], v[0], u[1]})
     + IntWordExpr::single({v[0], u[0], u[1]})
  );
}

IntWordExpr shuffle_product_unrolled_2_2(const IntWord& u, const IntWord& v) {
  return (
     + IntWordExpr::single({u[0], u[1], v[0], v[1]})
     + IntWordExpr::single({u[0], v[0], u[1], v[1]})
     + IntWordExpr::single({u[0], v[0], v[1], u[1]})
     + IntWordExpr::single({v[0], u[0], u[1], v[1]})
     + IntWordExpr::single({v[0], u[0], v[1], u[1]})
     + IntWordExpr::single({v[0], v[1], u[0], u[1]})
  );
}

IntWordExpr shuffle_product_unrolled_2_3(const IntWord& u, const IntWord& v) {
  return (
     + IntWordExpr::single({u[0], u[1], v[0], v[1], v[2]})
     + IntWordExpr::single({u[0], v[0], u[1], v[1], v[2]})
     + IntWordExpr::single({u[0], v[0], v[1], u[1], v[2]})
     + IntWordExpr::single({u[0], v[0], v[1], v[2], u[1]})
     + IntWordExpr::single({v[0], u[0], u[1], v[1], v[2]})
     + IntWordExpr::single({v[0], u[0], v[1], u[1], v[2]})
     + IntWordExpr::single({v[0], u[0], v[1], v[2], u[1]})
     + IntWordExpr::single({v[0], v[1], u[0], u[1], v[2]})
     + IntWordExpr::single({v[0], v[1], u[0], v[2], u[1]})
     + IntWordExpr::single({v[0], v[1], v[2], u[0], u[1]})
  );
}

IntWordExpr shuffle_product_unrolled_2_4(const IntWord& u, const IntWord& v) {
  return (
     + IntWordExpr::single({u[0], u[1], v[0], v[1], v[2], v[3]})
     + IntWordExpr::single({u[0], v[0], u[1], v[1], v[2], v[3]})
     + IntWordExpr::single({u[0], v[0], v[1], u[1], v[2], v[3]})
     + IntWordExpr::single({u[0], v[0], v[1], v[2], u[1], v[3]})
     + IntWordExpr::single({u[0], v[0], v[1], v[2], v[3], u[1]})
     + IntWordExpr::single({v[0], u[0], u[1], v[1], v[2], v[3]})
     + IntWordExpr::single({v[0], u[0], v[1], u[1], v[2], v[3]})
     + IntWordExpr::single({v[0], u[0], v[1], v[2], u[1], v[3]})
     + IntWordExpr::single({v[0], u[0], v[1], v[2], v[3], u[1]})
     + IntWordExpr::single({v[0], v[1], u[0], u[1], v[2], v[3]})
     + IntWordExpr::single({v[0], v[1], u[0], v[2], u[1], v[3]})
     + IntWordExpr::single({v[0], v[1], u[0], v[2], v[3], u[1]})
     + IntWordExpr::single({v[0], v[1], v[2], u[0], u[1], v[3]})
     + IntWordExpr::single({v[0], v[1], v[2], u[0], v[3], u[1]})
     + IntWordExpr::single({v[0], v[1], v[2], v[3], u[0], u[1]})
  );
}

IntWordExpr shuffle_product_unrolled_2_5(const IntWord& u, const IntWord& v) {
  return (
     + IntWordExpr::single({u[0], u[1], v[0], v[1], v[2], v[3], v[4]})
     + IntWordExpr::single({u[0], v[0], u[1], v[1], v[2], v[3], v[4]})
     + IntWordExpr::single({u[0], v[0], v[1], u[1], v[2], v[3], v[4]})
     + IntWordExpr::single({u[0], v[0], v[1], v[2], u[1], v[3], v[4]})
     + IntWordExpr::single({u[0], v[0], v[1], v[2], v[3], u[1], v[4]})
     + IntWordExpr::single({u[0], v[0], v[1], v[2], v[3], v[4], u[1]})
     + IntWordExpr::single({v[0], u[0], u[1], v[1], v[2], v[3], v[4]})
     + IntWordExpr::single({v[0], u[0], v[1], u[1], v[2], v[3], v[4]})
     + IntWordExpr::single({v[0], u[0], v[1], v[2], u[1], v[3], v[4]})
     + IntWordExpr::single({v[0], u[0], v[1], v[2], v[3], u[1], v[4]})
     + IntWordExpr::single({v[0], u[0], v[1], v[2], v[3], v[4], u[1]})
     + IntWordExpr::single({v[0], v[1], u[0], u[1], v[2], v[3], v[4]})
     + IntWordExpr::single({v[0], v[1], u[0], v[2], u[1], v[3], v[4]})
     + IntWordExpr::single({v[0], v[1], u[0], v[2], v[3], u[1], v[4]})
     + IntWordExpr::single({v[0], v[1], u[0], v[2], v[3], v[4], u[1]})
     + IntWordExpr::single({v[0], v[1], v[2], u[0], u[1], v[3], v[4]})
     + IntWordExpr::single({v[0], v[1], v[2], u[0], v[3], u[1], v[4]})
     + IntWordExpr::single({v[0], v[1], v[2], u[0], v[3], v[4], u[1]})
     + IntWordExpr::single({v[0], v[1], v[2], v[3], u[0], u[1], v[4]})
     + IntWordExpr::single({v[0], v[1], v[2], v[3], u[0], v[4], u[1]})
     + IntWordExpr::single({v[0], v[1], v[2], v[3], v[4], u[0], u[1]})
  );
}

IntWordExpr shuffle_product_unrolled_2_6(const IntWord& u, const IntWord& v) {
  return (
     + IntWordExpr::single({u[0], u[1], v[0], v[1], v[2], v[3], v[4], v[5]})
     + IntWordExpr::single({u[0], v[0], u[1], v[1], v[2], v[3], v[4], v[5]})
     + IntWordExpr::single({u[0], v[0], v[1], u[1], v[2], v[3], v[4], v[5]})
     + IntWordExpr::single({u[0], v[0], v[1], v[2], u[1], v[3], v[4], v[5]})
     + IntWordExpr::single({u[0], v[0], v[1], v[2], v[3], u[1], v[4], v[5]})
     + IntWordExpr::single({u[0], v[0], v[1], v[2], v[3], v[4], u[1], v[5]})
     + IntWordExpr::single({u[0], v[0], v[1], v[2], v[3], v[4], v[5], u[1]})
     + IntWordExpr::single({v[0], u[0], u[1], v[1], v[2], v[3], v[4], v[5]})
     + IntWordExpr::single({v[0], u[0], v[1], u[1], v[2], v[3], v[4], v[5]})
     + IntWordExpr::single({v[0], u[0], v[1], v[2], u[1], v[3], v[4], v[5]})
     + IntWordExpr::single({v[0], u[0], v[1], v[2], v[3], u[1], v[4], v[5]})
     + IntWordExpr::single({v[0], u[0], v[1], v[2], v[3], v[4], u[1], v[5]})
     + IntWordExpr::single({v[0], u[0], v[1], v[2], v[3], v[4], v[5], u[1]})
     + IntWordExpr::single({v[0], v[1], u[0], u[1], v[2], v[3], v[4], v[5]})
     + IntWordExpr::single({v[0], v[1], u[0], v[2], u[1], v[3], v[4], v[5]})
     + IntWordExpr::single({v[0], v[1], u[0], v[2], v[3], u[1], v[4], v[5]})
     + IntWordExpr::single({v[0], v[1], u[0], v[2], v[3], v[4], u[1], v[5]})
     + IntWordExpr::single({v[0], v[1], u[0], v[2], v[3], v[4], v[5], u[1]})
     + IntWordExpr::single({v[0], v[1], v[2], u[0], u[1], v[3], v[4], v[5]})
     + IntWordExpr::single({v[0], v[1], v[2], u[0], v[3], u[1], v[4], v[5]})
     + IntWordExpr::single({v[0], v[1], v[2], u[0], v[3], v[4], u[1], v[5]})
     + IntWordExpr::single({v[0], v[1], v[2], u[0], v[3], v[4], v[5], u[1]})
     + IntWordExpr::single({v[0], v[1], v[2], v[3], u[0], u[1], v[4], v[5]})
     + IntWordExpr::single({v[0], v[1], v[2], v[3], u[0], v[4], u[1], v[5]})
     + IntWordExpr::single({v[0], v[1], v[2], v[3], u[0], v[4], v[5], u[1]})
     + IntWordExpr::single({v[0], v[1], v[2], v[3], v[4], u[0], u[1], v[5]})
     + IntWordExpr::single({v[0], v[1], v[2], v[3], v[4], u[0], v[5], u[1]})
     + IntWordExpr::single({v[0], v[1], v[2], v[3], v[4], v[5], u[0], u[1]})
  );
}

IntWordExpr shuffle_product_unrolled_3_1(const IntWord& u, const IntWord& v) {
  return (
     + IntWordExpr::single({u[0], u[1], u[2], v[0]})
     + IntWordExpr::single({u[0], u[1], v[0], u[2]})
     + IntWordExpr::single({u[0], v[0], u[1], u[2]})
     + IntWordExpr::single({v[0], u[0], u[1], u[2]})
  );
}

IntWordExpr shuffle_product_unrolled_3_2(const IntWord& u, const IntWord& v) {
  return (
     + IntWordExpr::single({u[0], u[1], u[2], v[0], v[1]})
     + IntWordExpr::single({u[0], u[1], v[0], u[2], v[1]})
     + IntWordExpr::single({u[0], u[1], v[0], v[1], u[2]})
     + IntWordExpr::single({u[0], v[0], u[1], u[2], v[1]})
     + IntWordExpr::single({u[0], v[0], u[1], v[1], u[2]})
     + IntWordExpr::single({u[0], v[0], v[1], u[1], u[2]})
     + IntWordExpr::single({v[0], u[0], u[1], u[2], v[1]})
     + IntWordExpr::single({v[0], u[0], u[1], v[1], u[2]})
     + IntWordExpr::single({v[0], u[0], v[1], u[1], u[2]})
     + IntWordExpr::single({v[0], v[1], u[0], u[1], u[2]})
  );
}

IntWordExpr shuffle_product_unrolled_3_3(const IntWord& u, const IntWord& v) {
  return (
     + IntWordExpr::single({u[0], u[1], u[2], v[0], v[1], v[2]})
     + IntWordExpr::single({u[0], u[1], v[0], u[2], v[1], v[2]})
     + IntWordExpr::single({u[0], u[1], v[0], v[1], u[2], v[2]})
     + IntWordExpr::single({u[0], u[1], v[0], v[1], v[2], u[2]})
     + IntWordExpr::single({u[0], v[0], u[1], u[2], v[1], v[2]})
     + IntWordExpr::single({u[0], v[0], u[1], v[1], u[2], v[2]})
     + IntWordExpr::single({u[0], v[0], u[1], v[1], v[2], u[2]})
     + IntWordExpr::single({u[0], v[0], v[1], u[1], u[2], v[2]})
     + IntWordExpr::single({u[0], v[0], v[1], u[1], v[2], u[2]})
     + IntWordExpr::single({u[0], v[0], v[1], v[2], u[1], u[2]})
     + IntWordExpr::single({v[0], u[0], u[1], u[2], v[1], v[2]})
     + IntWordExpr::single({v[0], u[0], u[1], v[1], u[2], v[2]})
     + IntWordExpr::single({v[0], u[0], u[1], v[1], v[2], u[2]})
     + IntWordExpr::single({v[0], u[0], v[1], u[1], u[2], v[2]})
     + IntWordExpr::single({v[0], u[0], v[1], u[1], v[2], u[2]})
     + IntWordExpr::single({v[0], u[0], v[1], v[2], u[1], u[2]})
     + IntWordExpr::single({v[0], v[1], u[0], u[1], u[2], v[2]})
     + IntWordExpr::single({v[0], v[1], u[0], u[1], v[2], u[2]})
     + IntWordExpr::single({v[0], v[1], u[0], v[2], u[1], u[2]})
     + IntWordExpr::single({v[0], v[1], v[2], u[0], u[1], u[2]})
  );
}

IntWordExpr shuffle_product_unrolled_3_4(const IntWord& u, const IntWord& v) {
  return (
     + IntWordExpr::single({u[0], u[1], u[2], v[0], v[1], v[2], v[3]})
     + IntWordExpr::single({u[0], u[1], v[0], u[2], v[1], v[2], v[3]})
     + IntWordExpr::single({u[0], u[1], v[0], v[1], u[2], v[2], v[3]})
     + IntWordExpr::single({u[0], u[1], v[0], v[1], v[2], u[2], v[3]})
     + IntWordExpr::single({u[0], u[1], v[0], v[1], v[2], v[3], u[2]})
     + IntWordExpr::single({u[0], v[0], u[1], u[2], v[1], v[2], v[3]})
     + IntWordExpr::single({u[0], v[0], u[1], v[1], u[2], v[2], v[3]})
     + IntWordExpr::single({u[0], v[0], u[1], v[1], v[2], u[2], v[3]})
     + IntWordExpr::single({u[0], v[0], u[1], v[1], v[2], v[3], u[2]})
     + IntWordExpr::single({u[0], v[0], v[1], u[1], u[2], v[2], v[3]})
     + IntWordExpr::single({u[0], v[0], v[1], u[1], v[2], u[2], v[3]})
     + IntWordExpr::single({u[0], v[0], v[1], u[1], v[2], v[3], u[2]})
     + IntWordExpr::single({u[0], v[0], v[1], v[2], u[1], u[2], v[3]})
     + IntWordExpr::single({u[0], v[0], v[1], v[2], u[1], v[3], u[2]})
     + IntWordExpr::single({u[0], v[0], v[1], v[2], v[3], u[1], u[2]})
     + IntWordExpr::single({v[0], u[0], u[1], u[2], v[1], v[2], v[3]})
     + IntWordExpr::single({v[0], u[0], u[1], v[1], u[2], v[2], v[3]})
     + IntWordExpr::single({v[0], u[0], u[1], v[1], v[2], u[2], v[3]})
     + IntWordExpr::single({v[0], u[0], u[1], v[1], v[2], v[3], u[2]})
     + IntWordExpr::single({v[0], u[0], v[1], u[1], u[2], v[2], v[3]})
     + IntWordExpr::single({v[0], u[0], v[1], u[1], v[2], u[2], v[3]})
     + IntWordExpr::single({v[0], u[0], v[1], u[1], v[2], v[3], u[2]})
     + IntWordExpr::single({v[0], u[0], v[1], v[2], u[1], u[2], v[3]})
     + IntWordExpr::single({v[0], u[0], v[1], v[2], u[1], v[3], u[2]})
     + IntWordExpr::single({v[0], u[0], v[1], v[2], v[3], u[1], u[2]})
     + IntWordExpr::single({v[0], v[1], u[0], u[1], u[2], v[2], v[3]})
     + IntWordExpr::single({v[0], v[1], u[0], u[1], v[2], u[2], v[3]})
     + IntWordExpr::single({v[0], v[1], u[0], u[1], v[2], v[3], u[2]})
     + IntWordExpr::single({v[0], v[1], u[0], v[2], u[1], u[2], v[3]})
     + IntWordExpr::single({v[0], v[1], u[0], v[2], u[1], v[3], u[2]})
     + IntWordExpr::single({v[0], v[1], u[0], v[2], v[3], u[1], u[2]})
     + IntWordExpr::single({v[0], v[1], v[2], u[0], u[1], u[2], v[3]})
     + IntWordExpr::single({v[0], v[1], v[2], u[0], u[1], v[3], u[2]})
     + IntWordExpr::single({v[0], v[1], v[2], u[0], v[3], u[1], u[2]})
     + IntWordExpr::single({v[0], v[1], v[2], v[3], u[0], u[1], u[2]})
  );
}

IntWordExpr shuffle_product_unrolled_3_5(const IntWord& u, const IntWord& v) {
  return (
     + IntWordExpr::single({u[0], u[1], u[2], v[0], v[1], v[2], v[3], v[4]})
     + IntWordExpr::single({u[0], u[1], v[0], u[2], v[1], v[2], v[3], v[4]})
     + IntWordExpr::single({u[0], u[1], v[0], v[1], u[2], v[2], v[3], v[4]})
     + IntWordExpr::single({u[0], u[1], v[0], v[1], v[2], u[2], v[3], v[4]})
     + IntWordExpr::single({u[0], u[1], v[0], v[1], v[2], v[3], u[2], v[4]})
     + IntWordExpr::single({u[0], u[1], v[0], v[1], v[2], v[3], v[4], u[2]})
     + IntWordExpr::single({u[0], v[0], u[1], u[2], v[1], v[2], v[3], v[4]})
     + IntWordExpr::single({u[0], v[0], u[1], v[1], u[2], v[2], v[3], v[4]})
     + IntWordExpr::single({u[0], v[0], u[1], v[1], v[2], u[2], v[3], v[4]})
     + IntWordExpr::single({u[0], v[0], u[1], v[1], v[2], v[3], u[2], v[4]})
     + IntWordExpr::single({u[0], v[0], u[1], v[1], v[2], v[3], v[4], u[2]})
     + IntWordExpr::single({u[0], v[0], v[1], u[1], u[2], v[2], v[3], v[4]})
     + IntWordExpr::single({u[0], v[0], v[1], u[1], v[2], u[2], v[3], v[4]})
     + IntWordExpr::single({u[0], v[0], v[1], u[1], v[2], v[3], u[2], v[4]})
     + IntWordExpr::single({u[0], v[0], v[1], u[1], v[2], v[3], v[4], u[2]})
     + IntWordExpr::single({u[0], v[0], v[1], v[2], u[1], u[2], v[3], v[4]})
     + IntWordExpr::single({u[0], v[0], v[1], v[2], u[1], v[3], u[2], v[4]})
     + IntWordExpr::single({u[0], v[0], v[1], v[2], u[1], v[3], v[4], u[2]})
     + IntWordExpr::single({u[0], v[0], v[1], v[2], v[3], u[1], u[2], v[4]})
     + IntWordExpr::single({u[0], v[0], v[1], v[2], v[3], u[1], v[4], u[2]})
     + IntWordExpr::single({u[0], v[0], v[1], v[2], v[3], v[4], u[1], u[2]})
     + IntWordExpr::single({v[0], u[0], u[1], u[2], v[1], v[2], v[3], v[4]})
     + IntWordExpr::single({v[0], u[0], u[1], v[1], u[2], v[2], v[3], v[4]})
     + IntWordExpr::single({v[0], u[0], u[1], v[1], v[2], u[2], v[3], v[4]})
     + IntWordExpr::single({v[0], u[0], u[1], v[1], v[2], v[3], u[2], v[4]})
     + IntWordExpr::single({v[0], u[0], u[1], v[1], v[2], v[3], v[4], u[2]})
     + IntWordExpr::single({v[0], u[0], v[1], u[1], u[2], v[2], v[3], v[4]})
     + IntWordExpr::single({v[0], u[0], v[1], u[1], v[2], u[2], v[3], v[4]})
     + IntWordExpr::single({v[0], u[0], v[1], u[1], v[2], v[3], u[2], v[4]})
     + IntWordExpr::single({v[0], u[0], v[1], u[1], v[2], v[3], v[4], u[2]})
     + IntWordExpr::single({v[0], u[0], v[1], v[2], u[1], u[2], v[3], v[4]})
     + IntWordExpr::single({v[0], u[0], v[1], v[2], u[1], v[3], u[2], v[4]})
     + IntWordExpr::single({v[0], u[0], v[1], v[2], u[1], v[3], v[4], u[2]})
     + IntWordExpr::single({v[0], u[0], v[1], v[2], v[3], u[1], u[2], v[4]})
     + IntWordExpr::single({v[0], u[0], v[1], v[2], v[3], u[1], v[4], u[2]})
     + IntWordExpr::single({v[0], u[0], v[1], v[2], v[3], v[4], u[1], u[2]})
     + IntWordExpr::single({v[0], v[1], u[0], u[1], u[2], v[2], v[3], v[4]})
     + IntWordExpr::single({v[0], v[1], u[0], u[1], v[2], u[2], v[3], v[4]})
     + IntWordExpr::single({v[0], v[1], u[0], u[1], v[2], v[3], u[2], v[4]})
     + IntWordExpr::single({v[0], v[1], u[0], u[1], v[2], v[3], v[4], u[2]})
     + IntWordExpr::single({v[0], v[1], u[0], v[2], u[1], u[2], v[3], v[4]})
     + IntWordExpr::single({v[0], v[1], u[0], v[2], u[1], v[3], u[2], v[4]})
     + IntWordExpr::single({v[0], v[1], u[0], v[2], u[1], v[3], v[4], u[2]})
     + IntWordExpr::single({v[0], v[1], u[0], v[2], v[3], u[1], u[2], v[4]})
     + IntWordExpr::single({v[0], v[1], u[0], v[2], v[3], u[1], v[4], u[2]})
     + IntWordExpr::single({v[0], v[1], u[0], v[2], v[3], v[4], u[1], u[2]})
     + IntWordExpr::single({v[0], v[1], v[2], u[0], u[1], u[2], v[3], v[4]})
     + IntWordExpr::single({v[0], v[1], v[2], u[0], u[1], v[3], u[2], v[4]})
     + IntWordExpr::single({v[0], v[1], v[2], u[0], u[1], v[3], v[4], u[2]})
     + IntWordExpr::single({v[0], v[1], v[2], u[0], v[3], u[1], u[2], v[4]})
     + IntWordExpr::single({v[0], v[1], v[2], u[0], v[3], u[1], v[4], u[2]})
     + IntWordExpr::single({v[0], v[1], v[2], u[0], v[3], v[4], u[1], u[2]})
     + IntWordExpr::single({v[0], v[1], v[2], v[3], u[0], u[1], u[2], v[4]})
     + IntWordExpr::single({v[0], v[1], v[2], v[3], u[0], u[1], v[4], u[2]})
     + IntWordExpr::single({v[0], v[1], v[2], v[3], u[0], v[4], u[1], u[2]})
     + IntWordExpr::single({v[0], v[1], v[2], v[3], v[4], u[0], u[1], u[2]})
  );
}

IntWordExpr shuffle_product_unrolled_4_1(const IntWord& u, const IntWord& v) {
  return (
     + IntWordExpr::single({u[0], u[1], u[2], u[3], v[0]})
     + IntWordExpr::single({u[0], u[1], u[2], v[0], u[3]})
     + IntWordExpr::single({u[0], u[1], v[0], u[2], u[3]})
     + IntWordExpr::single({u[0], v[0], u[1], u[2], u[3]})
     + IntWordExpr::single({v[0], u[0], u[1], u[2], u[3]})
  );
}

IntWordExpr shuffle_product_unrolled_4_2(const IntWord& u, const IntWord& v) {
  return (
     + IntWordExpr::single({u[0], u[1], u[2], u[3], v[0], v[1]})
     + IntWordExpr::single({u[0], u[1], u[2], v[0], u[3], v[1]})
     + IntWordExpr::single({u[0], u[1], u[2], v[0], v[1], u[3]})
     + IntWordExpr::single({u[0], u[1], v[0], u[2], u[3], v[1]})
     + IntWordExpr::single({u[0], u[1], v[0], u[2], v[1], u[3]})
     + IntWordExpr::single({u[0], u[1], v[0], v[1], u[2], u[3]})
     + IntWordExpr::single({u[0], v[0], u[1], u[2], u[3], v[1]})
     + IntWordExpr::single({u[0], v[0], u[1], u[2], v[1], u[3]})
     + IntWordExpr::single({u[0], v[0], u[1], v[1], u[2], u[3]})
     + IntWordExpr::single({u[0], v[0], v[1], u[1], u[2], u[3]})
     + IntWordExpr::single({v[0], u[0], u[1], u[2], u[3], v[1]})
     + IntWordExpr::single({v[0], u[0], u[1], u[2], v[1], u[3]})
     + IntWordExpr::single({v[0], u[0], u[1], v[1], u[2], u[3]})
     + IntWordExpr::single({v[0], u[0], v[1], u[1], u[2], u[3]})
     + IntWordExpr::single({v[0], v[1], u[0], u[1], u[2], u[3]})
  );
}

IntWordExpr shuffle_product_unrolled_4_3(const IntWord& u, const IntWord& v) {
  return (
     + IntWordExpr::single({u[0], u[1], u[2], u[3], v[0], v[1], v[2]})
     + IntWordExpr::single({u[0], u[1], u[2], v[0], u[3], v[1], v[2]})
     + IntWordExpr::single({u[0], u[1], u[2], v[0], v[1], u[3], v[2]})
     + IntWordExpr::single({u[0], u[1], u[2], v[0], v[1], v[2], u[3]})
     + IntWordExpr::single({u[0], u[1], v[0], u[2], u[3], v[1], v[2]})
     + IntWordExpr::single({u[0], u[1], v[0], u[2], v[1], u[3], v[2]})
     + IntWordExpr::single({u[0], u[1], v[0], u[2], v[1], v[2], u[3]})
     + IntWordExpr::single({u[0], u[1], v[0], v[1], u[2], u[3], v[2]})
     + IntWordExpr::single({u[0], u[1], v[0], v[1], u[2], v[2], u[3]})
     + IntWordExpr::single({u[0], u[1], v[0], v[1], v[2], u[2], u[3]})
     + IntWordExpr::single({u[0], v[0], u[1], u[2], u[3], v[1], v[2]})
     + IntWordExpr::single({u[0], v[0], u[1], u[2], v[1], u[3], v[2]})
     + IntWordExpr::single({u[0], v[0], u[1], u[2], v[1], v[2], u[3]})
     + IntWordExpr::single({u[0], v[0], u[1], v[1], u[2], u[3], v[2]})
     + IntWordExpr::single({u[0], v[0], u[1], v[1], u[2], v[2], u[3]})
     + IntWordExpr::single({u[0], v[0], u[1], v[1], v[2], u[2], u[3]})
     + IntWordExpr::single({u[0], v[0], v[1], u[1], u[2], u[3], v[2]})
     + IntWordExpr::single({u[0], v[0], v[1], u[1], u[2], v[2], u[3]})
     + IntWordExpr::single({u[0], v[0], v[1], u[1], v[2], u[2], u[3]})
     + IntWordExpr::single({u[0], v[0], v[1], v[2], u[1], u[2], u[3]})
     + IntWordExpr::single({v[0], u[0], u[1], u[2], u[3], v[1], v[2]})
     + IntWordExpr::single({v[0], u[0], u[1], u[2], v[1], u[3], v[2]})
     + IntWordExpr::single({v[0], u[0], u[1], u[2], v[1], v[2], u[3]})
     + IntWordExpr::single({v[0], u[0], u[1], v[1], u[2], u[3], v[2]})
     + IntWordExpr::single({v[0], u[0], u[1], v[1], u[2], v[2], u[3]})
     + IntWordExpr::single({v[0], u[0], u[1], v[1], v[2], u[2], u[3]})
     + IntWordExpr::single({v[0], u[0], v[1], u[1], u[2], u[3], v[2]})
     + IntWordExpr::single({v[0], u[0], v[1], u[1], u[2], v[2], u[3]})
     + IntWordExpr::single({v[0], u[0], v[1], u[1], v[2], u[2], u[3]})
     + IntWordExpr::single({v[0], u[0], v[1], v[2], u[1], u[2], u[3]})
     + IntWordExpr::single({v[0], v[1], u[0], u[1], u[2], u[3], v[2]})
     + IntWordExpr::single({v[0], v[1], u[0], u[1], u[2], v[2], u[3]})
     + IntWordExpr::single({v[0], v[1], u[0], u[1], v[2], u[2], u[3]})
     + IntWordExpr::single({v[0], v[1], u[0], v[2], u[1], u[2], u[3]})
     + IntWordExpr::single({v[0], v[1], v[2], u[0], u[1], u[2], u[3]})
  );
}

IntWordExpr shuffle_product_unrolled_4_4(const IntWord& u, const IntWord& v) {
  return (
     + IntWordExpr::single({u[0], u[1], u[2], u[3], v[0], v[1], v[2], v[3]})
     + IntWordExpr::single({u[0], u[1], u[2], v[0], u[3], v[1], v[2], v[3]})
     + IntWordExpr::single({u[0], u[1], u[2], v[0], v[1], u[3], v[2], v[3]})
     + IntWordExpr::single({u[0], u[1], u[2], v[0], v[1], v[2], u[3], v[3]})
     + IntWordExpr::single({u[0], u[1], u[2], v[0], v[1], v[2], v[3], u[3]})
     + IntWordExpr::single({u[0], u[1], v[0], u[2], u[3], v[1], v[2], v[3]})
     + IntWordExpr::single({u[0], u[1], v[0], u[2], v[1], u[3], v[2], v[3]})
     + IntWordExpr::single({u[0], u[1], v[0], u[2], v[1], v[2], u[3], v[3]})
     + IntWordExpr::single({u[0], u[1], v[0], u[2], v[1], v[2], v[3], u[3]})
     + IntWordExpr::single({u[0], u[1], v[0], v[1], u[2], u[3], v[2], v[3]})
     + IntWordExpr::single({u[0], u[1], v[0], v[1], u[2], v[2], u[3], v[3]})
     + IntWordExpr::single({u[0], u[1], v[0], v[1], u[2], v[2], v[3], u[3]})
     + IntWordExpr::single({u[0], u[1], v[0], v[1], v[2], u[2], u[3], v[3]})
     + IntWordExpr::single({u[0], u[1], v[0], v[1], v[2], u[2], v[3], u[3]})
     + IntWordExpr::single({u[0], u[1], v[0], v[1], v[2], v[3], u[2], u[3]})
     + IntWordExpr::single({u[0], v[0], u[1], u[2], u[3], v[1], v[2], v[3]})
     + IntWordExpr::single({u[0], v[0], u[1], u[2], v[1], u[3], v[2], v[3]})
     + IntWordExpr::single({u[0], v[0], u[1], u[2], v[1], v[2], u[3], v[3]})
     + IntWordExpr::single({u[0], v[0], u[1], u[2], v[1], v[2], v[3], u[3]})
     + IntWordExpr::single({u[0], v[0], u[1], v[1], u[2], u[3], v[2], v[3]})
     + IntWordExpr::single({u[0], v[0], u[1], v[1], u[2], v[2], u[3], v[3]})
     + IntWordExpr::single({u[0], v[0], u[1], v[1], u[2], v[2], v[3], u[3]})
     + IntWordExpr::single({u[0], v[0], u[1], v[1], v[2], u[2], u[3], v[3]})
     + IntWordExpr::single({u[0], v[0], u[1], v[1], v[2], u[2], v[3], u[3]})
     + IntWordExpr::single({u[0], v[0], u[1], v[1], v[2], v[3], u[2], u[3]})
     + IntWordExpr::single({u[0], v[0], v[1], u[1], u[2], u[3], v[2], v[3]})
     + IntWordExpr::single({u[0], v[0], v[1], u[1], u[2], v[2], u[3], v[3]})
     + IntWordExpr::single({u[0], v[0], v[1], u[1], u[2], v[2], v[3], u[3]})
     + IntWordExpr::single({u[0], v[0], v[1], u[1], v[2], u[2], u[3], v[3]})
     + IntWordExpr::single({u[0], v[0], v[1], u[1], v[2], u[2], v[3], u[3]})
     + IntWordExpr::single({u[0], v[0], v[1], u[1], v[2], v[3], u[2], u[3]})
     + IntWordExpr::single({u[0], v[0], v[1], v[2], u[1], u[2], u[3], v[3]})
     + IntWordExpr::single({u[0], v[0], v[1], v[2], u[1], u[2], v[3], u[3]})
     + IntWordExpr::single({u[0], v[0], v[1], v[2], u[1], v[3], u[2], u[3]})
     + IntWordExpr::single({u[0], v[0], v[1], v[2], v[3], u[1], u[2], u[3]})
     + IntWordExpr::single({v[0], u[0], u[1], u[2], u[3], v[1], v[2], v[3]})
     + IntWordExpr::single({v[0], u[0], u[1], u[2], v[1], u[3], v[2], v[3]})
     + IntWordExpr::single({v[0], u[0], u[1], u[2], v[1], v[2], u[3], v[3]})
     + IntWordExpr::single({v[0], u[0], u[1], u[2], v[1], v[2], v[3], u[3]})
     + IntWordExpr::single({v[0], u[0], u[1], v[1], u[2], u[3], v[2], v[3]})
     + IntWordExpr::single({v[0], u[0], u[1], v[1], u[2], v[2], u[3], v[3]})
     + IntWordExpr::single({v[0], u[0], u[1], v[1], u[2], v[2], v[3], u[3]})
     + IntWordExpr::single({v[0], u[0], u[1], v[1], v[2], u[2], u[3], v[3]})
     + IntWordExpr::single({v[0], u[0], u[1], v[1], v[2], u[2], v[3], u[3]})
     + IntWordExpr::single({v[0], u[0], u[1], v[1], v[2], v[3], u[2], u[3]})
     + IntWordExpr::single({v[0], u[0], v[1], u[1], u[2], u[3], v[2], v[3]})
     + IntWordExpr::single({v[0], u[0], v[1], u[1], u[2], v[2], u[3], v[3]})
     + IntWordExpr::single({v[0], u[0], v[1], u[1], u[2], v[2], v[3], u[3]})
     + IntWordExpr::single({v[0], u[0], v[1], u[1], v[2], u[2], u[3], v[3]})
     + IntWordExpr::single({v[0], u[0], v[1], u[1], v[2], u[2], v[3], u[3]})
     + IntWordExpr::single({v[0], u[0], v[1], u[1], v[2], v[3], u[2], u[3]})
     + IntWordExpr::single({v[0], u[0], v[1], v[2], u[1], u[2], u[3], v[3]})
     + IntWordExpr::single({v[0], u[0], v[1], v[2], u[1], u[2], v[3], u[3]})
     + IntWordExpr::single({v[0], u[0], v[1], v[2], u[1], v[3], u[2], u[3]})
     + IntWordExpr::single({v[0], u[0], v[1], v[2], v[3], u[1], u[2], u[3]})
     + IntWordExpr::single({v[0], v[1], u[0], u[1], u[2], u[3], v[2], v[3]})
     + IntWordExpr::single({v[0], v[1], u[0], u[1], u[2], v[2], u[3], v[3]})
     + IntWordExpr::single({v[0], v[1], u[0], u[1], u[2], v[2], v[3], u[3]})
     + IntWordExpr::single({v[0], v[1], u[0], u[1], v[2], u[2], u[3], v[3]})
     + IntWordExpr::single({v[0], v[1], u[0], u[1], v[2], u[2], v[3], u[3]})
     + IntWordExpr::single({v[0], v[1], u[0], u[1], v[2], v[3], u[2], u[3]})
     + IntWordExpr::single({v[0], v[1], u[0], v[2], u[1], u[2], u[3], v[3]})
     + IntWordExpr::single({v[0], v[1], u[0], v[2], u[1], u[2], v[3], u[3]})
     + IntWordExpr::single({v[0], v[1], u[0], v[2], u[1], v[3], u[2], u[3]})
     + IntWordExpr::single({v[0], v[1], u[0], v[2], v[3], u[1], u[2], u[3]})
     + IntWordExpr::single({v[0], v[1], v[2], u[0], u[1], u[2], u[3], v[3]})
     + IntWordExpr::single({v[0], v[1], v[2], u[0], u[1], u[2], v[3], u[3]})
     + IntWordExpr::single({v[0], v[1], v[2], u[0], u[1], v[3], u[2], u[3]})
     + IntWordExpr::single({v[0], v[1], v[2], u[0], v[3], u[1], u[2], u[3]})
     + IntWordExpr::single({v[0], v[1], v[2], v[3], u[0], u[1], u[2], u[3]})
  );
}

IntWordExpr shuffle_product_unrolled_5_1(const IntWord& u, const IntWord& v) {
  return (
     + IntWordExpr::single({u[0], u[1], u[2], u[3], u[4], v[0]})
     + IntWordExpr::single({u[0], u[1], u[2], u[3], v[0], u[4]})
     + IntWordExpr::single({u[0], u[1], u[2], v[0], u[3], u[4]})
     + IntWordExpr::single({u[0], u[1], v[0], u[2], u[3], u[4]})
     + IntWordExpr::single({u[0], v[0], u[1], u[2], u[3], u[4]})
     + IntWordExpr::single({v[0], u[0], u[1], u[2], u[3], u[4]})
  );
}

IntWordExpr shuffle_product_unrolled_5_2(const IntWord& u, const IntWord& v) {
  return (
     + IntWordExpr::single({u[0], u[1], u[2], u[3], u[4], v[0], v[1]})
     + IntWordExpr::single({u[0], u[1], u[2], u[3], v[0], u[4], v[1]})
     + IntWordExpr::single({u[0], u[1], u[2], u[3], v[0], v[1], u[4]})
     + IntWordExpr::single({u[0], u[1], u[2], v[0], u[3], u[4], v[1]})
     + IntWordExpr::single({u[0], u[1], u[2], v[0], u[3], v[1], u[4]})
     + IntWordExpr::single({u[0], u[1], u[2], v[0], v[1], u[3], u[4]})
     + IntWordExpr::single({u[0], u[1], v[0], u[2], u[3], u[4], v[1]})
     + IntWordExpr::single({u[0], u[1], v[0], u[2], u[3], v[1], u[4]})
     + IntWordExpr::single({u[0], u[1], v[0], u[2], v[1], u[3], u[4]})
     + IntWordExpr::single({u[0], u[1], v[0], v[1], u[2], u[3], u[4]})
     + IntWordExpr::single({u[0], v[0], u[1], u[2], u[3], u[4], v[1]})
     + IntWordExpr::single({u[0], v[0], u[1], u[2], u[3], v[1], u[4]})
     + IntWordExpr::single({u[0], v[0], u[1], u[2], v[1], u[3], u[4]})
     + IntWordExpr::single({u[0], v[0], u[1], v[1], u[2], u[3], u[4]})
     + IntWordExpr::single({u[0], v[0], v[1], u[1], u[2], u[3], u[4]})
     + IntWordExpr::single({v[0], u[0], u[1], u[2], u[3], u[4], v[1]})
     + IntWordExpr::single({v[0], u[0], u[1], u[2], u[3], v[1], u[4]})
     + IntWordExpr::single({v[0], u[0], u[1], u[2], v[1], u[3], u[4]})
     + IntWordExpr::single({v[0], u[0], u[1], v[1], u[2], u[3], u[4]})
     + IntWordExpr::single({v[0], u[0], v[1], u[1], u[2], u[3], u[4]})
     + IntWordExpr::single({v[0], v[1], u[0], u[1], u[2], u[3], u[4]})
  );
}

IntWordExpr shuffle_product_unrolled_5_3(const IntWord& u, const IntWord& v) {
  return (
     + IntWordExpr::single({u[0], u[1], u[2], u[3], u[4], v[0], v[1], v[2]})
     + IntWordExpr::single({u[0], u[1], u[2], u[3], v[0], u[4], v[1], v[2]})
     + IntWordExpr::single({u[0], u[1], u[2], u[3], v[0], v[1], u[4], v[2]})
     + IntWordExpr::single({u[0], u[1], u[2], u[3], v[0], v[1], v[2], u[4]})
     + IntWordExpr::single({u[0], u[1], u[2], v[0], u[3], u[4], v[1], v[2]})
     + IntWordExpr::single({u[0], u[1], u[2], v[0], u[3], v[1], u[4], v[2]})
     + IntWordExpr::single({u[0], u[1], u[2], v[0], u[3], v[1], v[2], u[4]})
     + IntWordExpr::single({u[0], u[1], u[2], v[0], v[1], u[3], u[4], v[2]})
     + IntWordExpr::single({u[0], u[1], u[2], v[0], v[1], u[3], v[2], u[4]})
     + IntWordExpr::single({u[0], u[1], u[2], v[0], v[1], v[2], u[3], u[4]})
     + IntWordExpr::single({u[0], u[1], v[0], u[2], u[3], u[4], v[1], v[2]})
     + IntWordExpr::single({u[0], u[1], v[0], u[2], u[3], v[1], u[4], v[2]})
     + IntWordExpr::single({u[0], u[1], v[0], u[2], u[3], v[1], v[2], u[4]})
     + IntWordExpr::single({u[0], u[1], v[0], u[2], v[1], u[3], u[4], v[2]})
     + IntWordExpr::single({u[0], u[1], v[0], u[2], v[1], u[3], v[2], u[4]})
     + IntWordExpr::single({u[0], u[1], v[0], u[2], v[1], v[2], u[3], u[4]})
     + IntWordExpr::single({u[0], u[1], v[0], v[1], u[2], u[3], u[4], v[2]})
     + IntWordExpr::single({u[0], u[1], v[0], v[1], u[2], u[3], v[2], u[4]})
     + IntWordExpr::single({u[0], u[1], v[0], v[1], u[2], v[2], u[3], u[4]})
     + IntWordExpr::single({u[0], u[1], v[0], v[1], v[2], u[2], u[3], u[4]})
     + IntWordExpr::single({u[0], v[0], u[1], u[2], u[3], u[4], v[1], v[2]})
     + IntWordExpr::single({u[0], v[0], u[1], u[2], u[3], v[1], u[4], v[2]})
     + IntWordExpr::single({u[0], v[0], u[1], u[2], u[3], v[1], v[2], u[4]})
     + IntWordExpr::single({u[0], v[0], u[1], u[2], v[1], u[3], u[4], v[2]})
     + IntWordExpr::single({u[0], v[0], u[1], u[2], v[1], u[3], v[2], u[4]})
     + IntWordExpr::single({u[0], v[0], u[1], u[2], v[1], v[2], u[3], u[4]})
     + IntWordExpr::single({u[0], v[0], u[1], v[1], u[2], u[3], u[4], v[2]})
     + IntWordExpr::single({u[0], v[0], u[1], v[1], u[2], u[3], v[2], u[4]})
     + IntWordExpr::single({u[0], v[0], u[1], v[1], u[2], v[2], u[3], u[4]})
     + IntWordExpr::single({u[0], v[0], u[1], v[1], v[2], u[2], u[3], u[4]})
     + IntWordExpr::single({u[0], v[0], v[1], u[1], u[2], u[3], u[4], v[2]})
     + IntWordExpr::single({u[0], v[0], v[1], u[1], u[2], u[3], v[2], u[4]})
     + IntWordExpr::single({u[0], v[0], v[1], u[1], u[2], v[2], u[3], u[4]})
     + IntWordExpr::single({u[0], v[0], v[1], u[1], v[2], u[2], u[3], u[4]})
     + IntWordExpr::single({u[0], v[0], v[1], v[2], u[1], u[2], u[3], u[4]})
     + IntWordExpr::single({v[0], u[0], u[1], u[2], u[3], u[4], v[1], v[2]})
     + IntWordExpr::single({v[0], u[0], u[1], u[2], u[3], v[1], u[4], v[2]})
     + IntWordExpr::single({v[0], u[0], u[1], u[2], u[3], v[1], v[2], u[4]})
     + IntWordExpr::single({v[0], u[0], u[1], u[2], v[1], u[3], u[4], v[2]})
     + IntWordExpr::single({v[0], u[0], u[1], u[2], v[1], u[3], v[2], u[4]})
     + IntWordExpr::single({v[0], u[0], u[1], u[2], v[1], v[2], u[3], u[4]})
     + IntWordExpr::single({v[0], u[0], u[1], v[1], u[2], u[3], u[4], v[2]})
     + IntWordExpr::single({v[0], u[0], u[1], v[1], u[2], u[3], v[2], u[4]})
     + IntWordExpr::single({v[0], u[0], u[1], v[1], u[2], v[2], u[3], u[4]})
     + IntWordExpr::single({v[0], u[0], u[1], v[1], v[2], u[2], u[3], u[4]})
     + IntWordExpr::single({v[0], u[0], v[1], u[1], u[2], u[3], u[4], v[2]})
     + IntWordExpr::single({v[0], u[0], v[1], u[1], u[2], u[3], v[2], u[4]})
     + IntWordExpr::single({v[0], u[0], v[1], u[1], u[2], v[2], u[3], u[4]})
     + IntWordExpr::single({v[0], u[0], v[1], u[1], v[2], u[2], u[3], u[4]})
     + IntWordExpr::single({v[0], u[0], v[1], v[2], u[1], u[2], u[3], u[4]})
     + IntWordExpr::single({v[0], v[1], u[0], u[1], u[2], u[3], u[4], v[2]})
     + IntWordExpr::single({v[0], v[1], u[0], u[1], u[2], u[3], v[2], u[4]})
     + IntWordExpr::single({v[0], v[1], u[0], u[1], u[2], v[2], u[3], u[4]})
     + IntWordExpr::single({v[0], v[1], u[0], u[1], v[2], u[2], u[3], u[4]})
     + IntWordExpr::single({v[0], v[1], u[0], v[2], u[1], u[2], u[3], u[4]})
     + IntWordExpr::single({v[0], v[1], v[2], u[0], u[1], u[2], u[3], u[4]})
  );
}

IntWordExpr shuffle_product_unrolled_6_1(const IntWord& u, const IntWord& v) {
  return (
     + IntWordExpr::single({u[0], u[1], u[2], u[3], u[4], u[5], v[0]})
     + IntWordExpr::single({u[0], u[1], u[2], u[3], u[4], v[0], u[5]})
     + IntWordExpr::single({u[0], u[1], u[2], u[3], v[0], u[4], u[5]})
     + IntWordExpr::single({u[0], u[1], u[2], v[0], u[3], u[4], u[5]})
     + IntWordExpr::single({u[0], u[1], v[0], u[2], u[3], u[4], u[5]})
     + IntWordExpr::single({u[0], v[0], u[1], u[2], u[3], u[4], u[5]})
     + IntWordExpr::single({v[0], u[0], u[1], u[2], u[3], u[4], u[5]})
  );
}

IntWordExpr shuffle_product_unrolled_6_2(const IntWord& u, const IntWord& v) {
  return (
     + IntWordExpr::single({u[0], u[1], u[2], u[3], u[4], u[5], v[0], v[1]})
     + IntWordExpr::single({u[0], u[1], u[2], u[3], u[4], v[0], u[5], v[1]})
     + IntWordExpr::single({u[0], u[1], u[2], u[3], u[4], v[0], v[1], u[5]})
     + IntWordExpr::single({u[0], u[1], u[2], u[3], v[0], u[4], u[5], v[1]})
     + IntWordExpr::single({u[0], u[1], u[2], u[3], v[0], u[4], v[1], u[5]})
     + IntWordExpr::single({u[0], u[1], u[2], u[3], v[0], v[1], u[4], u[5]})
     + IntWordExpr::single({u[0], u[1], u[2], v[0], u[3], u[4], u[5], v[1]})
     + IntWordExpr::single({u[0], u[1], u[2], v[0], u[3], u[4], v[1], u[5]})
     + IntWordExpr::single({u[0], u[1], u[2], v[0], u[3], v[1], u[4], u[5]})
     + IntWordExpr::single({u[0], u[1], u[2], v[0], v[1], u[3], u[4], u[5]})
     + IntWordExpr::single({u[0], u[1], v[0], u[2], u[3], u[4], u[5], v[1]})
     + IntWordExpr::single({u[0], u[1], v[0], u[2], u[3], u[4], v[1], u[5]})
     + IntWordExpr::single({u[0], u[1], v[0], u[2], u[3], v[1], u[4], u[5]})
     + IntWordExpr::single({u[0], u[1], v[0], u[2], v[1], u[3], u[4], u[5]})
     + IntWordExpr::single({u[0], u[1], v[0], v[1], u[2], u[3], u[4], u[5]})
     + IntWordExpr::single({u[0], v[0], u[1], u[2], u[3], u[4], u[5], v[1]})
     + IntWordExpr::single({u[0], v[0], u[1], u[2], u[3], u[4], v[1], u[5]})
     + IntWordExpr::single({u[0], v[0], u[1], u[2], u[3], v[1], u[4], u[5]})
     + IntWordExpr::single({u[0], v[0], u[1], u[2], v[1], u[3], u[4], u[5]})
     + IntWordExpr::single({u[0], v[0], u[1], v[1], u[2], u[3], u[4], u[5]})
     + IntWordExpr::single({u[0], v[0], v[1], u[1], u[2], u[3], u[4], u[5]})
     + IntWordExpr::single({v[0], u[0], u[1], u[2], u[3], u[4], u[5], v[1]})
     + IntWordExpr::single({v[0], u[0], u[1], u[2], u[3], u[4], v[1], u[5]})
     + IntWordExpr::single({v[0], u[0], u[1], u[2], u[3], v[1], u[4], u[5]})
     + IntWordExpr::single({v[0], u[0], u[1], u[2], v[1], u[3], u[4], u[5]})
     + IntWordExpr::single({v[0], u[0], u[1], v[1], u[2], u[3], u[4], u[5]})
     + IntWordExpr::single({v[0], u[0], v[1], u[1], u[2], u[3], u[4], u[5]})
     + IntWordExpr::single({v[0], v[1], u[0], u[1], u[2], u[3], u[4], u[5]})
  );
}

IntWordExpr shuffle_product_unrolled_7_1(const IntWord& u, const IntWord& v) {
  return (
     + IntWordExpr::single({u[0], u[1], u[2], u[3], u[4], u[5], u[6], v[0]})
     + IntWordExpr::single({u[0], u[1], u[2], u[3], u[4], u[5], v[0], u[6]})
     + IntWordExpr::single({u[0], u[1], u[2], u[3], u[4], v[0], u[5], u[6]})
     + IntWordExpr::single({u[0], u[1], u[2], u[3], v[0], u[4], u[5], u[6]})
     + IntWordExpr::single({u[0], u[1], u[2], v[0], u[3], u[4], u[5], u[6]})
     + IntWordExpr::single({u[0], u[1], v[0], u[2], u[3], u[4], u[5], u[6]})
     + IntWordExpr::single({u[0], v[0], u[1], u[2], u[3], u[4], u[5], u[6]})
     + IntWordExpr::single({v[0], u[0], u[1], u[2], u[3], u[4], u[5], u[6]})
  );
}

IntWordExpr shuffle_power_unrolled(const IntWord& u, const IntWord& v) {
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
      case  1: return shuffle_product_unrolled_2_1(u, v);
      case  2: return shuffle_product_unrolled_2_2(u, v);
      case  3: return shuffle_product_unrolled_2_3(u, v);
      case  4: return shuffle_product_unrolled_2_4(u, v);
      case  5: return shuffle_product_unrolled_2_5(u, v);
      case  6: return shuffle_product_unrolled_2_6(u, v);
    }
    break;
    case 3:
    switch (v.size()) {
      case  1: return shuffle_product_unrolled_3_1(u, v);
      case  2: return shuffle_product_unrolled_3_2(u, v);
      case  3: return shuffle_product_unrolled_3_3(u, v);
      case  4: return shuffle_product_unrolled_3_4(u, v);
      case  5: return shuffle_product_unrolled_3_5(u, v);
    }
    break;
    case 4:
    switch (v.size()) {
      case  1: return shuffle_product_unrolled_4_1(u, v);
      case  2: return shuffle_product_unrolled_4_2(u, v);
      case  3: return shuffle_product_unrolled_4_3(u, v);
      case  4: return shuffle_product_unrolled_4_4(u, v);
    }
    break;
    case 5:
    switch (v.size()) {
      case  1: return shuffle_product_unrolled_5_1(u, v);
      case  2: return shuffle_product_unrolled_5_2(u, v);
      case  3: return shuffle_product_unrolled_5_3(u, v);
    }
    break;
    case 6:
    switch (v.size()) {
      case  1: return shuffle_product_unrolled_6_1(u, v);
      case  2: return shuffle_product_unrolled_6_2(u, v);
    }
    break;
    case 7:
    switch (v.size()) {
      case  1: return shuffle_product_unrolled_7_1(u, v);
    }
    break;
  }
  return {};
}

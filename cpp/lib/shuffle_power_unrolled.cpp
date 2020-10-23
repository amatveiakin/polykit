/*
#include "shuffle_unrolled.h"

#include "absl/strings/str_cat.h"


IntWordExpr shuffle_power_unrolled_len_1_pow_2(const IntWord& w) {
  return (
    +2*IntWordExpr::single({w[0], w[0]})
  );
}

IntWordExpr shuffle_power_unrolled_len_1_pow_3(const IntWord& w) {
  return (
    +6*IntWordExpr::single({w[0], w[0], w[0]})
  );
}

IntWordExpr shuffle_power_unrolled_len_1_pow_4(const IntWord& w) {
  return (
    +24*IntWordExpr::single({w[0], w[0], w[0], w[0]})
  );
}

IntWordExpr shuffle_power_unrolled_len_1_pow_5(const IntWord& w) {
  return (
    +120*IntWordExpr::single({w[0], w[0], w[0], w[0], w[0]})
  );
}

IntWordExpr shuffle_power_unrolled_len_1_pow_6(const IntWord& w) {
  return (
    +720*IntWordExpr::single({w[0], w[0], w[0], w[0], w[0], w[0]})
  );
}

IntWordExpr shuffle_power_unrolled_len_1_pow_7(const IntWord& w) {
  return (
    +5040*IntWordExpr::single({w[0], w[0], w[0], w[0], w[0], w[0], w[0]})
  );
}

IntWordExpr shuffle_power_unrolled_len_1_pow_8(const IntWord& w) {
  return (
    +40320*IntWordExpr::single({w[0], w[0], w[0], w[0], w[0], w[0], w[0], w[0]})
  );
}

IntWordExpr shuffle_power_unrolled_len_1_pow_9(const IntWord& w) {
  return (
    +362880*IntWordExpr::single({w[0], w[0], w[0], w[0], w[0], w[0], w[0], w[0], w[0]})
  );
}

IntWordExpr shuffle_power_unrolled_len_1_pow_10(const IntWord& w) {
  return (
    +3628800*IntWordExpr::single({w[0], w[0], w[0], w[0], w[0], w[0], w[0], w[0], w[0], w[0]})
  );
}

IntWordExpr shuffle_power_unrolled_len_2_pow_2(const IntWord& w) {
  return (
    +4*IntWordExpr::single({w[0], w[0], w[1], w[1]})
    +2*IntWordExpr::single({w[0], w[1], w[0], w[1]})
  );
}

IntWordExpr shuffle_power_unrolled_len_2_pow_3(const IntWord& w) {
  return (
    +36*IntWordExpr::single({w[0], w[0], w[0], w[1], w[1], w[1]})
    +24*IntWordExpr::single({w[0], w[0], w[1], w[0], w[1], w[1]})
    +12*IntWordExpr::single({w[0], w[0], w[1], w[1], w[0], w[1]})
    +12*IntWordExpr::single({w[0], w[1], w[0], w[0], w[1], w[1]})
     +6*IntWordExpr::single({w[0], w[1], w[0], w[1], w[0], w[1]})
  );
}

IntWordExpr shuffle_power_unrolled_len_2_pow_4(const IntWord& w) {
  return (
    +576*IntWordExpr::single({w[0], w[0], w[0], w[0], w[1], w[1], w[1], w[1]})
    +432*IntWordExpr::single({w[0], w[0], w[0], w[1], w[0], w[1], w[1], w[1]})
    +288*IntWordExpr::single({w[0], w[0], w[0], w[1], w[1], w[0], w[1], w[1]})
    +144*IntWordExpr::single({w[0], w[0], w[0], w[1], w[1], w[1], w[0], w[1]})
    +288*IntWordExpr::single({w[0], w[0], w[1], w[0], w[0], w[1], w[1], w[1]})
    +192*IntWordExpr::single({w[0], w[0], w[1], w[0], w[1], w[0], w[1], w[1]})
     +96*IntWordExpr::single({w[0], w[0], w[1], w[0], w[1], w[1], w[0], w[1]})
     +96*IntWordExpr::single({w[0], w[0], w[1], w[1], w[0], w[0], w[1], w[1]})
     +48*IntWordExpr::single({w[0], w[0], w[1], w[1], w[0], w[1], w[0], w[1]})
    +144*IntWordExpr::single({w[0], w[1], w[0], w[0], w[0], w[1], w[1], w[1]})
     +96*IntWordExpr::single({w[0], w[1], w[0], w[0], w[1], w[0], w[1], w[1]})
     +48*IntWordExpr::single({w[0], w[1], w[0], w[0], w[1], w[1], w[0], w[1]})
     +48*IntWordExpr::single({w[0], w[1], w[0], w[1], w[0], w[0], w[1], w[1]})
     +24*IntWordExpr::single({w[0], w[1], w[0], w[1], w[0], w[1], w[0], w[1]})
  );
}

IntWordExpr shuffle_power_unrolled_len_2_pow_5(const IntWord& w) {
  return (
    +14400*IntWordExpr::single({w[0], w[0], w[0], w[0], w[0], w[1], w[1], w[1], w[1], w[1]})
    +11520*IntWordExpr::single({w[0], w[0], w[0], w[0], w[1], w[0], w[1], w[1], w[1], w[1]})
     +8640*IntWordExpr::single({w[0], w[0], w[0], w[0], w[1], w[1], w[0], w[1], w[1], w[1]})
     +5760*IntWordExpr::single({w[0], w[0], w[0], w[0], w[1], w[1], w[1], w[0], w[1], w[1]})
     +2880*IntWordExpr::single({w[0], w[0], w[0], w[0], w[1], w[1], w[1], w[1], w[0], w[1]})
     +8640*IntWordExpr::single({w[0], w[0], w[0], w[1], w[0], w[0], w[1], w[1], w[1], w[1]})
     +6480*IntWordExpr::single({w[0], w[0], w[0], w[1], w[0], w[1], w[0], w[1], w[1], w[1]})
     +4320*IntWordExpr::single({w[0], w[0], w[0], w[1], w[0], w[1], w[1], w[0], w[1], w[1]})
     +2160*IntWordExpr::single({w[0], w[0], w[0], w[1], w[0], w[1], w[1], w[1], w[0], w[1]})
     +4320*IntWordExpr::single({w[0], w[0], w[0], w[1], w[1], w[0], w[0], w[1], w[1], w[1]})
     +2880*IntWordExpr::single({w[0], w[0], w[0], w[1], w[1], w[0], w[1], w[0], w[1], w[1]})
     +1440*IntWordExpr::single({w[0], w[0], w[0], w[1], w[1], w[0], w[1], w[1], w[0], w[1]})
     +1440*IntWordExpr::single({w[0], w[0], w[0], w[1], w[1], w[1], w[0], w[0], w[1], w[1]})
      +720*IntWordExpr::single({w[0], w[0], w[0], w[1], w[1], w[1], w[0], w[1], w[0], w[1]})
     +5760*IntWordExpr::single({w[0], w[0], w[1], w[0], w[0], w[0], w[1], w[1], w[1], w[1]})
     +4320*IntWordExpr::single({w[0], w[0], w[1], w[0], w[0], w[1], w[0], w[1], w[1], w[1]})
     +2880*IntWordExpr::single({w[0], w[0], w[1], w[0], w[0], w[1], w[1], w[0], w[1], w[1]})
     +1440*IntWordExpr::single({w[0], w[0], w[1], w[0], w[0], w[1], w[1], w[1], w[0], w[1]})
     +2880*IntWordExpr::single({w[0], w[0], w[1], w[0], w[1], w[0], w[0], w[1], w[1], w[1]})
     +1920*IntWordExpr::single({w[0], w[0], w[1], w[0], w[1], w[0], w[1], w[0], w[1], w[1]})
      +960*IntWordExpr::single({w[0], w[0], w[1], w[0], w[1], w[0], w[1], w[1], w[0], w[1]})
      +960*IntWordExpr::single({w[0], w[0], w[1], w[0], w[1], w[1], w[0], w[0], w[1], w[1]})
      +480*IntWordExpr::single({w[0], w[0], w[1], w[0], w[1], w[1], w[0], w[1], w[0], w[1]})
     +1440*IntWordExpr::single({w[0], w[0], w[1], w[1], w[0], w[0], w[0], w[1], w[1], w[1]})
      +960*IntWordExpr::single({w[0], w[0], w[1], w[1], w[0], w[0], w[1], w[0], w[1], w[1]})
      +480*IntWordExpr::single({w[0], w[0], w[1], w[1], w[0], w[0], w[1], w[1], w[0], w[1]})
      +480*IntWordExpr::single({w[0], w[0], w[1], w[1], w[0], w[1], w[0], w[0], w[1], w[1]})
      +240*IntWordExpr::single({w[0], w[0], w[1], w[1], w[0], w[1], w[0], w[1], w[0], w[1]})
     +2880*IntWordExpr::single({w[0], w[1], w[0], w[0], w[0], w[0], w[1], w[1], w[1], w[1]})
     +2160*IntWordExpr::single({w[0], w[1], w[0], w[0], w[0], w[1], w[0], w[1], w[1], w[1]})
     +1440*IntWordExpr::single({w[0], w[1], w[0], w[0], w[0], w[1], w[1], w[0], w[1], w[1]})
      +720*IntWordExpr::single({w[0], w[1], w[0], w[0], w[0], w[1], w[1], w[1], w[0], w[1]})
     +1440*IntWordExpr::single({w[0], w[1], w[0], w[0], w[1], w[0], w[0], w[1], w[1], w[1]})
      +960*IntWordExpr::single({w[0], w[1], w[0], w[0], w[1], w[0], w[1], w[0], w[1], w[1]})
      +480*IntWordExpr::single({w[0], w[1], w[0], w[0], w[1], w[0], w[1], w[1], w[0], w[1]})
      +480*IntWordExpr::single({w[0], w[1], w[0], w[0], w[1], w[1], w[0], w[0], w[1], w[1]})
      +240*IntWordExpr::single({w[0], w[1], w[0], w[0], w[1], w[1], w[0], w[1], w[0], w[1]})
      +720*IntWordExpr::single({w[0], w[1], w[0], w[1], w[0], w[0], w[0], w[1], w[1], w[1]})
      +480*IntWordExpr::single({w[0], w[1], w[0], w[1], w[0], w[0], w[1], w[0], w[1], w[1]})
      +240*IntWordExpr::single({w[0], w[1], w[0], w[1], w[0], w[0], w[1], w[1], w[0], w[1]})
      +240*IntWordExpr::single({w[0], w[1], w[0], w[1], w[0], w[1], w[0], w[0], w[1], w[1]})
      +120*IntWordExpr::single({w[0], w[1], w[0], w[1], w[0], w[1], w[0], w[1], w[0], w[1]})
  );
}

IntWordExpr shuffle_power_unrolled_len_3_pow_2(const IntWord& w) {
  return (
    +8*IntWordExpr::single({w[0], w[0], w[1], w[1], w[2], w[2]})
    +4*IntWordExpr::single({w[0], w[0], w[1], w[2], w[1], w[2]})
    +4*IntWordExpr::single({w[0], w[1], w[0], w[1], w[2], w[2]})
    +2*IntWordExpr::single({w[0], w[1], w[0], w[2], w[1], w[2]})
    +2*IntWordExpr::single({w[0], w[1], w[2], w[0], w[1], w[2]})
  );
}

IntWordExpr shuffle_power_unrolled_len_3_pow_3(const IntWord& w) {
  return (
    +216*IntWordExpr::single({w[0], w[0], w[0], w[1], w[1], w[1], w[2], w[2], w[2]})
    +144*IntWordExpr::single({w[0], w[0], w[0], w[1], w[1], w[2], w[1], w[2], w[2]})
     +72*IntWordExpr::single({w[0], w[0], w[0], w[1], w[1], w[2], w[2], w[1], w[2]})
     +72*IntWordExpr::single({w[0], w[0], w[0], w[1], w[2], w[1], w[1], w[2], w[2]})
     +36*IntWordExpr::single({w[0], w[0], w[0], w[1], w[2], w[1], w[2], w[1], w[2]})
    +144*IntWordExpr::single({w[0], w[0], w[1], w[0], w[1], w[1], w[2], w[2], w[2]})
     +96*IntWordExpr::single({w[0], w[0], w[1], w[0], w[1], w[2], w[1], w[2], w[2]})
     +48*IntWordExpr::single({w[0], w[0], w[1], w[0], w[1], w[2], w[2], w[1], w[2]})
     +48*IntWordExpr::single({w[0], w[0], w[1], w[0], w[2], w[1], w[1], w[2], w[2]})
     +24*IntWordExpr::single({w[0], w[0], w[1], w[0], w[2], w[1], w[2], w[1], w[2]})
     +72*IntWordExpr::single({w[0], w[0], w[1], w[1], w[0], w[1], w[2], w[2], w[2]})
     +48*IntWordExpr::single({w[0], w[0], w[1], w[1], w[0], w[2], w[1], w[2], w[2]})
     +24*IntWordExpr::single({w[0], w[0], w[1], w[1], w[0], w[2], w[2], w[1], w[2]})
     +48*IntWordExpr::single({w[0], w[0], w[1], w[1], w[2], w[0], w[1], w[2], w[2]})
     +24*IntWordExpr::single({w[0], w[0], w[1], w[1], w[2], w[0], w[2], w[1], w[2]})
     +24*IntWordExpr::single({w[0], w[0], w[1], w[1], w[2], w[2], w[0], w[1], w[2]})
     +48*IntWordExpr::single({w[0], w[0], w[1], w[2], w[0], w[1], w[1], w[2], w[2]})
     +24*IntWordExpr::single({w[0], w[0], w[1], w[2], w[0], w[1], w[2], w[1], w[2]})
     +24*IntWordExpr::single({w[0], w[0], w[1], w[2], w[1], w[0], w[1], w[2], w[2]})
     +12*IntWordExpr::single({w[0], w[0], w[1], w[2], w[1], w[0], w[2], w[1], w[2]})
     +12*IntWordExpr::single({w[0], w[0], w[1], w[2], w[1], w[2], w[0], w[1], w[2]})
     +72*IntWordExpr::single({w[0], w[1], w[0], w[0], w[1], w[1], w[2], w[2], w[2]})
     +48*IntWordExpr::single({w[0], w[1], w[0], w[0], w[1], w[2], w[1], w[2], w[2]})
     +24*IntWordExpr::single({w[0], w[1], w[0], w[0], w[1], w[2], w[2], w[1], w[2]})
     +24*IntWordExpr::single({w[0], w[1], w[0], w[0], w[2], w[1], w[1], w[2], w[2]})
     +12*IntWordExpr::single({w[0], w[1], w[0], w[0], w[2], w[1], w[2], w[1], w[2]})
     +36*IntWordExpr::single({w[0], w[1], w[0], w[1], w[0], w[1], w[2], w[2], w[2]})
     +24*IntWordExpr::single({w[0], w[1], w[0], w[1], w[0], w[2], w[1], w[2], w[2]})
     +12*IntWordExpr::single({w[0], w[1], w[0], w[1], w[0], w[2], w[2], w[1], w[2]})
     +24*IntWordExpr::single({w[0], w[1], w[0], w[1], w[2], w[0], w[1], w[2], w[2]})
     +12*IntWordExpr::single({w[0], w[1], w[0], w[1], w[2], w[0], w[2], w[1], w[2]})
     +12*IntWordExpr::single({w[0], w[1], w[0], w[1], w[2], w[2], w[0], w[1], w[2]})
     +24*IntWordExpr::single({w[0], w[1], w[0], w[2], w[0], w[1], w[1], w[2], w[2]})
     +12*IntWordExpr::single({w[0], w[1], w[0], w[2], w[0], w[1], w[2], w[1], w[2]})
     +12*IntWordExpr::single({w[0], w[1], w[0], w[2], w[1], w[0], w[1], w[2], w[2]})
      +6*IntWordExpr::single({w[0], w[1], w[0], w[2], w[1], w[0], w[2], w[1], w[2]})
      +6*IntWordExpr::single({w[0], w[1], w[0], w[2], w[1], w[2], w[0], w[1], w[2]})
     +24*IntWordExpr::single({w[0], w[1], w[2], w[0], w[0], w[1], w[1], w[2], w[2]})
     +12*IntWordExpr::single({w[0], w[1], w[2], w[0], w[0], w[1], w[2], w[1], w[2]})
     +12*IntWordExpr::single({w[0], w[1], w[2], w[0], w[1], w[0], w[1], w[2], w[2]})
      +6*IntWordExpr::single({w[0], w[1], w[2], w[0], w[1], w[0], w[2], w[1], w[2]})
      +6*IntWordExpr::single({w[0], w[1], w[2], w[0], w[1], w[2], w[0], w[1], w[2]})
  );
}

IntWordExpr shuffle_power_unrolled_len_4_pow_2(const IntWord& w) {
  return (
    +16*IntWordExpr::single({w[0], w[0], w[1], w[1], w[2], w[2], w[3], w[3]})
     +8*IntWordExpr::single({w[0], w[0], w[1], w[1], w[2], w[3], w[2], w[3]})
     +8*IntWordExpr::single({w[0], w[0], w[1], w[2], w[1], w[2], w[3], w[3]})
     +4*IntWordExpr::single({w[0], w[0], w[1], w[2], w[1], w[3], w[2], w[3]})
     +4*IntWordExpr::single({w[0], w[0], w[1], w[2], w[3], w[1], w[2], w[3]})
     +8*IntWordExpr::single({w[0], w[1], w[0], w[1], w[2], w[2], w[3], w[3]})
     +4*IntWordExpr::single({w[0], w[1], w[0], w[1], w[2], w[3], w[2], w[3]})
     +4*IntWordExpr::single({w[0], w[1], w[0], w[2], w[1], w[2], w[3], w[3]})
     +2*IntWordExpr::single({w[0], w[1], w[0], w[2], w[1], w[3], w[2], w[3]})
     +2*IntWordExpr::single({w[0], w[1], w[0], w[2], w[3], w[1], w[2], w[3]})
     +4*IntWordExpr::single({w[0], w[1], w[2], w[0], w[1], w[2], w[3], w[3]})
     +2*IntWordExpr::single({w[0], w[1], w[2], w[0], w[1], w[3], w[2], w[3]})
     +2*IntWordExpr::single({w[0], w[1], w[2], w[0], w[3], w[1], w[2], w[3]})
     +2*IntWordExpr::single({w[0], w[1], w[2], w[3], w[0], w[1], w[2], w[3]})
  );
}

IntWordExpr shuffle_power_unrolled_len_5_pow_2(const IntWord& w) {
  return (
    +32*IntWordExpr::single({w[0], w[0], w[1], w[1], w[2], w[2], w[3], w[3], w[4], w[4]})
    +16*IntWordExpr::single({w[0], w[0], w[1], w[1], w[2], w[2], w[3], w[4], w[3], w[4]})
    +16*IntWordExpr::single({w[0], w[0], w[1], w[1], w[2], w[3], w[2], w[3], w[4], w[4]})
     +8*IntWordExpr::single({w[0], w[0], w[1], w[1], w[2], w[3], w[2], w[4], w[3], w[4]})
     +8*IntWordExpr::single({w[0], w[0], w[1], w[1], w[2], w[3], w[4], w[2], w[3], w[4]})
    +16*IntWordExpr::single({w[0], w[0], w[1], w[2], w[1], w[2], w[3], w[3], w[4], w[4]})
     +8*IntWordExpr::single({w[0], w[0], w[1], w[2], w[1], w[2], w[3], w[4], w[3], w[4]})
     +8*IntWordExpr::single({w[0], w[0], w[1], w[2], w[1], w[3], w[2], w[3], w[4], w[4]})
     +4*IntWordExpr::single({w[0], w[0], w[1], w[2], w[1], w[3], w[2], w[4], w[3], w[4]})
     +4*IntWordExpr::single({w[0], w[0], w[1], w[2], w[1], w[3], w[4], w[2], w[3], w[4]})
     +8*IntWordExpr::single({w[0], w[0], w[1], w[2], w[3], w[1], w[2], w[3], w[4], w[4]})
     +4*IntWordExpr::single({w[0], w[0], w[1], w[2], w[3], w[1], w[2], w[4], w[3], w[4]})
     +4*IntWordExpr::single({w[0], w[0], w[1], w[2], w[3], w[1], w[4], w[2], w[3], w[4]})
     +4*IntWordExpr::single({w[0], w[0], w[1], w[2], w[3], w[4], w[1], w[2], w[3], w[4]})
    +16*IntWordExpr::single({w[0], w[1], w[0], w[1], w[2], w[2], w[3], w[3], w[4], w[4]})
     +8*IntWordExpr::single({w[0], w[1], w[0], w[1], w[2], w[2], w[3], w[4], w[3], w[4]})
     +8*IntWordExpr::single({w[0], w[1], w[0], w[1], w[2], w[3], w[2], w[3], w[4], w[4]})
     +4*IntWordExpr::single({w[0], w[1], w[0], w[1], w[2], w[3], w[2], w[4], w[3], w[4]})
     +4*IntWordExpr::single({w[0], w[1], w[0], w[1], w[2], w[3], w[4], w[2], w[3], w[4]})
     +8*IntWordExpr::single({w[0], w[1], w[0], w[2], w[1], w[2], w[3], w[3], w[4], w[4]})
     +4*IntWordExpr::single({w[0], w[1], w[0], w[2], w[1], w[2], w[3], w[4], w[3], w[4]})
     +4*IntWordExpr::single({w[0], w[1], w[0], w[2], w[1], w[3], w[2], w[3], w[4], w[4]})
     +2*IntWordExpr::single({w[0], w[1], w[0], w[2], w[1], w[3], w[2], w[4], w[3], w[4]})
     +2*IntWordExpr::single({w[0], w[1], w[0], w[2], w[1], w[3], w[4], w[2], w[3], w[4]})
     +4*IntWordExpr::single({w[0], w[1], w[0], w[2], w[3], w[1], w[2], w[3], w[4], w[4]})
     +2*IntWordExpr::single({w[0], w[1], w[0], w[2], w[3], w[1], w[2], w[4], w[3], w[4]})
     +2*IntWordExpr::single({w[0], w[1], w[0], w[2], w[3], w[1], w[4], w[2], w[3], w[4]})
     +2*IntWordExpr::single({w[0], w[1], w[0], w[2], w[3], w[4], w[1], w[2], w[3], w[4]})
     +8*IntWordExpr::single({w[0], w[1], w[2], w[0], w[1], w[2], w[3], w[3], w[4], w[4]})
     +4*IntWordExpr::single({w[0], w[1], w[2], w[0], w[1], w[2], w[3], w[4], w[3], w[4]})
     +4*IntWordExpr::single({w[0], w[1], w[2], w[0], w[1], w[3], w[2], w[3], w[4], w[4]})
     +2*IntWordExpr::single({w[0], w[1], w[2], w[0], w[1], w[3], w[2], w[4], w[3], w[4]})
     +2*IntWordExpr::single({w[0], w[1], w[2], w[0], w[1], w[3], w[4], w[2], w[3], w[4]})
     +4*IntWordExpr::single({w[0], w[1], w[2], w[0], w[3], w[1], w[2], w[3], w[4], w[4]})
     +2*IntWordExpr::single({w[0], w[1], w[2], w[0], w[3], w[1], w[2], w[4], w[3], w[4]})
     +2*IntWordExpr::single({w[0], w[1], w[2], w[0], w[3], w[1], w[4], w[2], w[3], w[4]})
     +2*IntWordExpr::single({w[0], w[1], w[2], w[0], w[3], w[4], w[1], w[2], w[3], w[4]})
     +4*IntWordExpr::single({w[0], w[1], w[2], w[3], w[0], w[1], w[2], w[3], w[4], w[4]})
     +2*IntWordExpr::single({w[0], w[1], w[2], w[3], w[0], w[1], w[2], w[4], w[3], w[4]})
     +2*IntWordExpr::single({w[0], w[1], w[2], w[3], w[0], w[1], w[4], w[2], w[3], w[4]})
     +2*IntWordExpr::single({w[0], w[1], w[2], w[3], w[0], w[4], w[1], w[2], w[3], w[4]})
     +2*IntWordExpr::single({w[0], w[1], w[2], w[3], w[4], w[0], w[1], w[2], w[3], w[4]})
  );
}

IntWordExpr shuffle_power_unrolled(IntWord word, int pow) {  switch (word.size()) {
    case 1:
    switch (pow) {
      case  2: return shuffle_power_unrolled_len_1_pow_2(word);
      case  3: return shuffle_power_unrolled_len_1_pow_3(word);
      case  4: return shuffle_power_unrolled_len_1_pow_4(word);
      case  5: return shuffle_power_unrolled_len_1_pow_5(word);
      case  6: return shuffle_power_unrolled_len_1_pow_6(word);
      case  7: return shuffle_power_unrolled_len_1_pow_7(word);
      case  8: return shuffle_power_unrolled_len_1_pow_8(word);
      case  9: return shuffle_power_unrolled_len_1_pow_9(word);
      case 10: return shuffle_power_unrolled_len_1_pow_10(word);
    }
    break;
    case 2:
    switch (pow) {
      case  2: return shuffle_power_unrolled_len_2_pow_2(word);
      case  3: return shuffle_power_unrolled_len_2_pow_3(word);
      case  4: return shuffle_power_unrolled_len_2_pow_4(word);
      case  5: return shuffle_power_unrolled_len_2_pow_5(word);
    }
    break;
    case 3:
    switch (pow) {
      case  2: return shuffle_power_unrolled_len_3_pow_2(word);
      case  3: return shuffle_power_unrolled_len_3_pow_3(word);
    }
    break;
    case 4:
    switch (pow) {
      case  2: return shuffle_power_unrolled_len_4_pow_2(word);
    }
    break;
    case 5:
    switch (pow) {
      case  2: return shuffle_power_unrolled_len_5_pow_2(word);
    }
    break;
  }
  FAIL(absl::StrCat(
      "Shuffle power unroll doesn't exit for word = ",
      to_string(word), "; power = ", pow));
  }
*/

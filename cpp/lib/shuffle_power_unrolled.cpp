/*
#include "shuffle_unrolled.h"

#include "absl/strings/str_cat.h"


WordExpr shuffle_power_unrolled_len_1_pow_2(const Word& w) {
  return (
    +2*WordExpr::single({w[0], w[0]})
  );
}

WordExpr shuffle_power_unrolled_len_1_pow_3(const Word& w) {
  return (
    +6*WordExpr::single({w[0], w[0], w[0]})
  );
}

WordExpr shuffle_power_unrolled_len_1_pow_4(const Word& w) {
  return (
    +24*WordExpr::single({w[0], w[0], w[0], w[0]})
  );
}

WordExpr shuffle_power_unrolled_len_1_pow_5(const Word& w) {
  return (
    +120*WordExpr::single({w[0], w[0], w[0], w[0], w[0]})
  );
}

WordExpr shuffle_power_unrolled_len_1_pow_6(const Word& w) {
  return (
    +720*WordExpr::single({w[0], w[0], w[0], w[0], w[0], w[0]})
  );
}

WordExpr shuffle_power_unrolled_len_1_pow_7(const Word& w) {
  return (
    +5040*WordExpr::single({w[0], w[0], w[0], w[0], w[0], w[0], w[0]})
  );
}

WordExpr shuffle_power_unrolled_len_1_pow_8(const Word& w) {
  return (
    +40320*WordExpr::single({w[0], w[0], w[0], w[0], w[0], w[0], w[0], w[0]})
  );
}

WordExpr shuffle_power_unrolled_len_1_pow_9(const Word& w) {
  return (
    +362880*WordExpr::single({w[0], w[0], w[0], w[0], w[0], w[0], w[0], w[0], w[0]})
  );
}

WordExpr shuffle_power_unrolled_len_1_pow_10(const Word& w) {
  return (
    +3628800*WordExpr::single({w[0], w[0], w[0], w[0], w[0], w[0], w[0], w[0], w[0], w[0]})
  );
}

WordExpr shuffle_power_unrolled_len_2_pow_2(const Word& w) {
  return (
    +4*WordExpr::single({w[0], w[0], w[1], w[1]})
    +2*WordExpr::single({w[0], w[1], w[0], w[1]})
  );
}

WordExpr shuffle_power_unrolled_len_2_pow_3(const Word& w) {
  return (
    +36*WordExpr::single({w[0], w[0], w[0], w[1], w[1], w[1]})
    +24*WordExpr::single({w[0], w[0], w[1], w[0], w[1], w[1]})
    +12*WordExpr::single({w[0], w[0], w[1], w[1], w[0], w[1]})
    +12*WordExpr::single({w[0], w[1], w[0], w[0], w[1], w[1]})
     +6*WordExpr::single({w[0], w[1], w[0], w[1], w[0], w[1]})
  );
}

WordExpr shuffle_power_unrolled_len_2_pow_4(const Word& w) {
  return (
    +576*WordExpr::single({w[0], w[0], w[0], w[0], w[1], w[1], w[1], w[1]})
    +432*WordExpr::single({w[0], w[0], w[0], w[1], w[0], w[1], w[1], w[1]})
    +288*WordExpr::single({w[0], w[0], w[0], w[1], w[1], w[0], w[1], w[1]})
    +144*WordExpr::single({w[0], w[0], w[0], w[1], w[1], w[1], w[0], w[1]})
    +288*WordExpr::single({w[0], w[0], w[1], w[0], w[0], w[1], w[1], w[1]})
    +192*WordExpr::single({w[0], w[0], w[1], w[0], w[1], w[0], w[1], w[1]})
     +96*WordExpr::single({w[0], w[0], w[1], w[0], w[1], w[1], w[0], w[1]})
     +96*WordExpr::single({w[0], w[0], w[1], w[1], w[0], w[0], w[1], w[1]})
     +48*WordExpr::single({w[0], w[0], w[1], w[1], w[0], w[1], w[0], w[1]})
    +144*WordExpr::single({w[0], w[1], w[0], w[0], w[0], w[1], w[1], w[1]})
     +96*WordExpr::single({w[0], w[1], w[0], w[0], w[1], w[0], w[1], w[1]})
     +48*WordExpr::single({w[0], w[1], w[0], w[0], w[1], w[1], w[0], w[1]})
     +48*WordExpr::single({w[0], w[1], w[0], w[1], w[0], w[0], w[1], w[1]})
     +24*WordExpr::single({w[0], w[1], w[0], w[1], w[0], w[1], w[0], w[1]})
  );
}

WordExpr shuffle_power_unrolled_len_2_pow_5(const Word& w) {
  return (
    +14400*WordExpr::single({w[0], w[0], w[0], w[0], w[0], w[1], w[1], w[1], w[1], w[1]})
    +11520*WordExpr::single({w[0], w[0], w[0], w[0], w[1], w[0], w[1], w[1], w[1], w[1]})
     +8640*WordExpr::single({w[0], w[0], w[0], w[0], w[1], w[1], w[0], w[1], w[1], w[1]})
     +5760*WordExpr::single({w[0], w[0], w[0], w[0], w[1], w[1], w[1], w[0], w[1], w[1]})
     +2880*WordExpr::single({w[0], w[0], w[0], w[0], w[1], w[1], w[1], w[1], w[0], w[1]})
     +8640*WordExpr::single({w[0], w[0], w[0], w[1], w[0], w[0], w[1], w[1], w[1], w[1]})
     +6480*WordExpr::single({w[0], w[0], w[0], w[1], w[0], w[1], w[0], w[1], w[1], w[1]})
     +4320*WordExpr::single({w[0], w[0], w[0], w[1], w[0], w[1], w[1], w[0], w[1], w[1]})
     +2160*WordExpr::single({w[0], w[0], w[0], w[1], w[0], w[1], w[1], w[1], w[0], w[1]})
     +4320*WordExpr::single({w[0], w[0], w[0], w[1], w[1], w[0], w[0], w[1], w[1], w[1]})
     +2880*WordExpr::single({w[0], w[0], w[0], w[1], w[1], w[0], w[1], w[0], w[1], w[1]})
     +1440*WordExpr::single({w[0], w[0], w[0], w[1], w[1], w[0], w[1], w[1], w[0], w[1]})
     +1440*WordExpr::single({w[0], w[0], w[0], w[1], w[1], w[1], w[0], w[0], w[1], w[1]})
      +720*WordExpr::single({w[0], w[0], w[0], w[1], w[1], w[1], w[0], w[1], w[0], w[1]})
     +5760*WordExpr::single({w[0], w[0], w[1], w[0], w[0], w[0], w[1], w[1], w[1], w[1]})
     +4320*WordExpr::single({w[0], w[0], w[1], w[0], w[0], w[1], w[0], w[1], w[1], w[1]})
     +2880*WordExpr::single({w[0], w[0], w[1], w[0], w[0], w[1], w[1], w[0], w[1], w[1]})
     +1440*WordExpr::single({w[0], w[0], w[1], w[0], w[0], w[1], w[1], w[1], w[0], w[1]})
     +2880*WordExpr::single({w[0], w[0], w[1], w[0], w[1], w[0], w[0], w[1], w[1], w[1]})
     +1920*WordExpr::single({w[0], w[0], w[1], w[0], w[1], w[0], w[1], w[0], w[1], w[1]})
      +960*WordExpr::single({w[0], w[0], w[1], w[0], w[1], w[0], w[1], w[1], w[0], w[1]})
      +960*WordExpr::single({w[0], w[0], w[1], w[0], w[1], w[1], w[0], w[0], w[1], w[1]})
      +480*WordExpr::single({w[0], w[0], w[1], w[0], w[1], w[1], w[0], w[1], w[0], w[1]})
     +1440*WordExpr::single({w[0], w[0], w[1], w[1], w[0], w[0], w[0], w[1], w[1], w[1]})
      +960*WordExpr::single({w[0], w[0], w[1], w[1], w[0], w[0], w[1], w[0], w[1], w[1]})
      +480*WordExpr::single({w[0], w[0], w[1], w[1], w[0], w[0], w[1], w[1], w[0], w[1]})
      +480*WordExpr::single({w[0], w[0], w[1], w[1], w[0], w[1], w[0], w[0], w[1], w[1]})
      +240*WordExpr::single({w[0], w[0], w[1], w[1], w[0], w[1], w[0], w[1], w[0], w[1]})
     +2880*WordExpr::single({w[0], w[1], w[0], w[0], w[0], w[0], w[1], w[1], w[1], w[1]})
     +2160*WordExpr::single({w[0], w[1], w[0], w[0], w[0], w[1], w[0], w[1], w[1], w[1]})
     +1440*WordExpr::single({w[0], w[1], w[0], w[0], w[0], w[1], w[1], w[0], w[1], w[1]})
      +720*WordExpr::single({w[0], w[1], w[0], w[0], w[0], w[1], w[1], w[1], w[0], w[1]})
     +1440*WordExpr::single({w[0], w[1], w[0], w[0], w[1], w[0], w[0], w[1], w[1], w[1]})
      +960*WordExpr::single({w[0], w[1], w[0], w[0], w[1], w[0], w[1], w[0], w[1], w[1]})
      +480*WordExpr::single({w[0], w[1], w[0], w[0], w[1], w[0], w[1], w[1], w[0], w[1]})
      +480*WordExpr::single({w[0], w[1], w[0], w[0], w[1], w[1], w[0], w[0], w[1], w[1]})
      +240*WordExpr::single({w[0], w[1], w[0], w[0], w[1], w[1], w[0], w[1], w[0], w[1]})
      +720*WordExpr::single({w[0], w[1], w[0], w[1], w[0], w[0], w[0], w[1], w[1], w[1]})
      +480*WordExpr::single({w[0], w[1], w[0], w[1], w[0], w[0], w[1], w[0], w[1], w[1]})
      +240*WordExpr::single({w[0], w[1], w[0], w[1], w[0], w[0], w[1], w[1], w[0], w[1]})
      +240*WordExpr::single({w[0], w[1], w[0], w[1], w[0], w[1], w[0], w[0], w[1], w[1]})
      +120*WordExpr::single({w[0], w[1], w[0], w[1], w[0], w[1], w[0], w[1], w[0], w[1]})
  );
}

WordExpr shuffle_power_unrolled_len_3_pow_2(const Word& w) {
  return (
    +8*WordExpr::single({w[0], w[0], w[1], w[1], w[2], w[2]})
    +4*WordExpr::single({w[0], w[0], w[1], w[2], w[1], w[2]})
    +4*WordExpr::single({w[0], w[1], w[0], w[1], w[2], w[2]})
    +2*WordExpr::single({w[0], w[1], w[0], w[2], w[1], w[2]})
    +2*WordExpr::single({w[0], w[1], w[2], w[0], w[1], w[2]})
  );
}

WordExpr shuffle_power_unrolled_len_3_pow_3(const Word& w) {
  return (
    +216*WordExpr::single({w[0], w[0], w[0], w[1], w[1], w[1], w[2], w[2], w[2]})
    +144*WordExpr::single({w[0], w[0], w[0], w[1], w[1], w[2], w[1], w[2], w[2]})
     +72*WordExpr::single({w[0], w[0], w[0], w[1], w[1], w[2], w[2], w[1], w[2]})
     +72*WordExpr::single({w[0], w[0], w[0], w[1], w[2], w[1], w[1], w[2], w[2]})
     +36*WordExpr::single({w[0], w[0], w[0], w[1], w[2], w[1], w[2], w[1], w[2]})
    +144*WordExpr::single({w[0], w[0], w[1], w[0], w[1], w[1], w[2], w[2], w[2]})
     +96*WordExpr::single({w[0], w[0], w[1], w[0], w[1], w[2], w[1], w[2], w[2]})
     +48*WordExpr::single({w[0], w[0], w[1], w[0], w[1], w[2], w[2], w[1], w[2]})
     +48*WordExpr::single({w[0], w[0], w[1], w[0], w[2], w[1], w[1], w[2], w[2]})
     +24*WordExpr::single({w[0], w[0], w[1], w[0], w[2], w[1], w[2], w[1], w[2]})
     +72*WordExpr::single({w[0], w[0], w[1], w[1], w[0], w[1], w[2], w[2], w[2]})
     +48*WordExpr::single({w[0], w[0], w[1], w[1], w[0], w[2], w[1], w[2], w[2]})
     +24*WordExpr::single({w[0], w[0], w[1], w[1], w[0], w[2], w[2], w[1], w[2]})
     +48*WordExpr::single({w[0], w[0], w[1], w[1], w[2], w[0], w[1], w[2], w[2]})
     +24*WordExpr::single({w[0], w[0], w[1], w[1], w[2], w[0], w[2], w[1], w[2]})
     +24*WordExpr::single({w[0], w[0], w[1], w[1], w[2], w[2], w[0], w[1], w[2]})
     +48*WordExpr::single({w[0], w[0], w[1], w[2], w[0], w[1], w[1], w[2], w[2]})
     +24*WordExpr::single({w[0], w[0], w[1], w[2], w[0], w[1], w[2], w[1], w[2]})
     +24*WordExpr::single({w[0], w[0], w[1], w[2], w[1], w[0], w[1], w[2], w[2]})
     +12*WordExpr::single({w[0], w[0], w[1], w[2], w[1], w[0], w[2], w[1], w[2]})
     +12*WordExpr::single({w[0], w[0], w[1], w[2], w[1], w[2], w[0], w[1], w[2]})
     +72*WordExpr::single({w[0], w[1], w[0], w[0], w[1], w[1], w[2], w[2], w[2]})
     +48*WordExpr::single({w[0], w[1], w[0], w[0], w[1], w[2], w[1], w[2], w[2]})
     +24*WordExpr::single({w[0], w[1], w[0], w[0], w[1], w[2], w[2], w[1], w[2]})
     +24*WordExpr::single({w[0], w[1], w[0], w[0], w[2], w[1], w[1], w[2], w[2]})
     +12*WordExpr::single({w[0], w[1], w[0], w[0], w[2], w[1], w[2], w[1], w[2]})
     +36*WordExpr::single({w[0], w[1], w[0], w[1], w[0], w[1], w[2], w[2], w[2]})
     +24*WordExpr::single({w[0], w[1], w[0], w[1], w[0], w[2], w[1], w[2], w[2]})
     +12*WordExpr::single({w[0], w[1], w[0], w[1], w[0], w[2], w[2], w[1], w[2]})
     +24*WordExpr::single({w[0], w[1], w[0], w[1], w[2], w[0], w[1], w[2], w[2]})
     +12*WordExpr::single({w[0], w[1], w[0], w[1], w[2], w[0], w[2], w[1], w[2]})
     +12*WordExpr::single({w[0], w[1], w[0], w[1], w[2], w[2], w[0], w[1], w[2]})
     +24*WordExpr::single({w[0], w[1], w[0], w[2], w[0], w[1], w[1], w[2], w[2]})
     +12*WordExpr::single({w[0], w[1], w[0], w[2], w[0], w[1], w[2], w[1], w[2]})
     +12*WordExpr::single({w[0], w[1], w[0], w[2], w[1], w[0], w[1], w[2], w[2]})
      +6*WordExpr::single({w[0], w[1], w[0], w[2], w[1], w[0], w[2], w[1], w[2]})
      +6*WordExpr::single({w[0], w[1], w[0], w[2], w[1], w[2], w[0], w[1], w[2]})
     +24*WordExpr::single({w[0], w[1], w[2], w[0], w[0], w[1], w[1], w[2], w[2]})
     +12*WordExpr::single({w[0], w[1], w[2], w[0], w[0], w[1], w[2], w[1], w[2]})
     +12*WordExpr::single({w[0], w[1], w[2], w[0], w[1], w[0], w[1], w[2], w[2]})
      +6*WordExpr::single({w[0], w[1], w[2], w[0], w[1], w[0], w[2], w[1], w[2]})
      +6*WordExpr::single({w[0], w[1], w[2], w[0], w[1], w[2], w[0], w[1], w[2]})
  );
}

WordExpr shuffle_power_unrolled_len_4_pow_2(const Word& w) {
  return (
    +16*WordExpr::single({w[0], w[0], w[1], w[1], w[2], w[2], w[3], w[3]})
     +8*WordExpr::single({w[0], w[0], w[1], w[1], w[2], w[3], w[2], w[3]})
     +8*WordExpr::single({w[0], w[0], w[1], w[2], w[1], w[2], w[3], w[3]})
     +4*WordExpr::single({w[0], w[0], w[1], w[2], w[1], w[3], w[2], w[3]})
     +4*WordExpr::single({w[0], w[0], w[1], w[2], w[3], w[1], w[2], w[3]})
     +8*WordExpr::single({w[0], w[1], w[0], w[1], w[2], w[2], w[3], w[3]})
     +4*WordExpr::single({w[0], w[1], w[0], w[1], w[2], w[3], w[2], w[3]})
     +4*WordExpr::single({w[0], w[1], w[0], w[2], w[1], w[2], w[3], w[3]})
     +2*WordExpr::single({w[0], w[1], w[0], w[2], w[1], w[3], w[2], w[3]})
     +2*WordExpr::single({w[0], w[1], w[0], w[2], w[3], w[1], w[2], w[3]})
     +4*WordExpr::single({w[0], w[1], w[2], w[0], w[1], w[2], w[3], w[3]})
     +2*WordExpr::single({w[0], w[1], w[2], w[0], w[1], w[3], w[2], w[3]})
     +2*WordExpr::single({w[0], w[1], w[2], w[0], w[3], w[1], w[2], w[3]})
     +2*WordExpr::single({w[0], w[1], w[2], w[3], w[0], w[1], w[2], w[3]})
  );
}

WordExpr shuffle_power_unrolled_len_5_pow_2(const Word& w) {
  return (
    +32*WordExpr::single({w[0], w[0], w[1], w[1], w[2], w[2], w[3], w[3], w[4], w[4]})
    +16*WordExpr::single({w[0], w[0], w[1], w[1], w[2], w[2], w[3], w[4], w[3], w[4]})
    +16*WordExpr::single({w[0], w[0], w[1], w[1], w[2], w[3], w[2], w[3], w[4], w[4]})
     +8*WordExpr::single({w[0], w[0], w[1], w[1], w[2], w[3], w[2], w[4], w[3], w[4]})
     +8*WordExpr::single({w[0], w[0], w[1], w[1], w[2], w[3], w[4], w[2], w[3], w[4]})
    +16*WordExpr::single({w[0], w[0], w[1], w[2], w[1], w[2], w[3], w[3], w[4], w[4]})
     +8*WordExpr::single({w[0], w[0], w[1], w[2], w[1], w[2], w[3], w[4], w[3], w[4]})
     +8*WordExpr::single({w[0], w[0], w[1], w[2], w[1], w[3], w[2], w[3], w[4], w[4]})
     +4*WordExpr::single({w[0], w[0], w[1], w[2], w[1], w[3], w[2], w[4], w[3], w[4]})
     +4*WordExpr::single({w[0], w[0], w[1], w[2], w[1], w[3], w[4], w[2], w[3], w[4]})
     +8*WordExpr::single({w[0], w[0], w[1], w[2], w[3], w[1], w[2], w[3], w[4], w[4]})
     +4*WordExpr::single({w[0], w[0], w[1], w[2], w[3], w[1], w[2], w[4], w[3], w[4]})
     +4*WordExpr::single({w[0], w[0], w[1], w[2], w[3], w[1], w[4], w[2], w[3], w[4]})
     +4*WordExpr::single({w[0], w[0], w[1], w[2], w[3], w[4], w[1], w[2], w[3], w[4]})
    +16*WordExpr::single({w[0], w[1], w[0], w[1], w[2], w[2], w[3], w[3], w[4], w[4]})
     +8*WordExpr::single({w[0], w[1], w[0], w[1], w[2], w[2], w[3], w[4], w[3], w[4]})
     +8*WordExpr::single({w[0], w[1], w[0], w[1], w[2], w[3], w[2], w[3], w[4], w[4]})
     +4*WordExpr::single({w[0], w[1], w[0], w[1], w[2], w[3], w[2], w[4], w[3], w[4]})
     +4*WordExpr::single({w[0], w[1], w[0], w[1], w[2], w[3], w[4], w[2], w[3], w[4]})
     +8*WordExpr::single({w[0], w[1], w[0], w[2], w[1], w[2], w[3], w[3], w[4], w[4]})
     +4*WordExpr::single({w[0], w[1], w[0], w[2], w[1], w[2], w[3], w[4], w[3], w[4]})
     +4*WordExpr::single({w[0], w[1], w[0], w[2], w[1], w[3], w[2], w[3], w[4], w[4]})
     +2*WordExpr::single({w[0], w[1], w[0], w[2], w[1], w[3], w[2], w[4], w[3], w[4]})
     +2*WordExpr::single({w[0], w[1], w[0], w[2], w[1], w[3], w[4], w[2], w[3], w[4]})
     +4*WordExpr::single({w[0], w[1], w[0], w[2], w[3], w[1], w[2], w[3], w[4], w[4]})
     +2*WordExpr::single({w[0], w[1], w[0], w[2], w[3], w[1], w[2], w[4], w[3], w[4]})
     +2*WordExpr::single({w[0], w[1], w[0], w[2], w[3], w[1], w[4], w[2], w[3], w[4]})
     +2*WordExpr::single({w[0], w[1], w[0], w[2], w[3], w[4], w[1], w[2], w[3], w[4]})
     +8*WordExpr::single({w[0], w[1], w[2], w[0], w[1], w[2], w[3], w[3], w[4], w[4]})
     +4*WordExpr::single({w[0], w[1], w[2], w[0], w[1], w[2], w[3], w[4], w[3], w[4]})
     +4*WordExpr::single({w[0], w[1], w[2], w[0], w[1], w[3], w[2], w[3], w[4], w[4]})
     +2*WordExpr::single({w[0], w[1], w[2], w[0], w[1], w[3], w[2], w[4], w[3], w[4]})
     +2*WordExpr::single({w[0], w[1], w[2], w[0], w[1], w[3], w[4], w[2], w[3], w[4]})
     +4*WordExpr::single({w[0], w[1], w[2], w[0], w[3], w[1], w[2], w[3], w[4], w[4]})
     +2*WordExpr::single({w[0], w[1], w[2], w[0], w[3], w[1], w[2], w[4], w[3], w[4]})
     +2*WordExpr::single({w[0], w[1], w[2], w[0], w[3], w[1], w[4], w[2], w[3], w[4]})
     +2*WordExpr::single({w[0], w[1], w[2], w[0], w[3], w[4], w[1], w[2], w[3], w[4]})
     +4*WordExpr::single({w[0], w[1], w[2], w[3], w[0], w[1], w[2], w[3], w[4], w[4]})
     +2*WordExpr::single({w[0], w[1], w[2], w[3], w[0], w[1], w[2], w[4], w[3], w[4]})
     +2*WordExpr::single({w[0], w[1], w[2], w[3], w[0], w[1], w[4], w[2], w[3], w[4]})
     +2*WordExpr::single({w[0], w[1], w[2], w[3], w[0], w[4], w[1], w[2], w[3], w[4]})
     +2*WordExpr::single({w[0], w[1], w[2], w[3], w[4], w[0], w[1], w[2], w[3], w[4]})
  );
}

WordExpr shuffle_power_unrolled(Word word, int pow) {  switch (word.size()) {
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

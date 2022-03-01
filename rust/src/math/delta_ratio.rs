use crate::base::neg_one_pow;
use crate::math::{D, DeltaExpr};


pub fn cross_ratio(a: i32, b: i32, c: i32, d: i32) -> DeltaExpr {
    D(a, b) + D(c, d) - D(b, c) - D(d, a)
}

pub fn cross_ratio_vec(p: &[i32]) -> DeltaExpr {
    let n = p.len();
    assert!(n % 2 == 0);
    let mut ret = DeltaExpr::new();
    for i in 0..n {
        ret += neg_one_pow(i) * D(p[i], p[(i+1)%n]);
    }
    ret
}

// 1 - cross_ratio(a, b, c, d)
pub fn neg_cross_ratio(a: i32, b: i32, c: i32, d: i32) -> DeltaExpr {
    cross_ratio(a, c, b, d)
}

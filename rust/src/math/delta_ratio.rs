use crate::base::neg_one_pow;
use crate::math::{XArg, D, DeltaExpr};


pub fn cross_ratio(a: impl XArg, b: impl XArg, c: impl XArg, d: impl XArg) -> DeltaExpr {
    D(a, b) + D(c, d) - D(b, c) - D(d, a)
}

pub fn cross_ratio_vec(p: &[impl XArg]) -> DeltaExpr {
    let n = p.len();
    assert!(n % 2 == 0);
    let mut ret = DeltaExpr::zero();
    for i in 0..n {
        ret += neg_one_pow(i) * D(p[i], p[(i+1)%n]);
    }
    ret
}

// 1 - cross_ratio(a, b, c, d)
pub fn neg_cross_ratio(a: impl XArg, b: impl XArg, c: impl XArg, d: impl XArg) -> DeltaExpr {
    cross_ratio(a, c, b, d)
}

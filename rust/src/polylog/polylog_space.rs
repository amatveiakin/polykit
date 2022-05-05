#![allow(non_snake_case)]

use itertools::Itertools;

use crate::math::{XArg, DeltaExpr};
use crate::polylog::QLi;


pub fn CB_naive_via_QLi_fours(weight: i32, points: &[impl XArg]) -> Vec<DeltaExpr> {
    points.iter().combinations(4).map(|p| QLi(weight, &from_vec_ref(p))).collect()
}


fn from_vec_ref<T: Copy>(vec: Vec<&T>) -> Vec<T> {
    vec.into_iter().copied().collect()
}

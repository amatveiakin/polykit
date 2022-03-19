#![allow(non_snake_case)]

use itertools::Itertools;
use tuple_conv::TupleOrVec;

use crate::base::{StringTools};
use crate::math::{GammaExpr, pullback};
use crate::polylog::{QLi};


pub fn GrQLi(weight: i32, qli_points: &[i32], bonus_points: &[i32]) -> GammaExpr {
    pullback(
        QLi(weight, qli_points),
        bonus_points
    ).drop_annotations().annotate(
        format!(
            "GrQLi_{}({})",
            weight,
            [
                bonus_points.iter().sorted().join(","),
                qli_points.iter().join(",")
            ].iter().join_non_empty(" / ")
        )
    )
}

// TODO: Add a natural way to support zero args; the best I've found for now is `Vec::<i32>::new()`.
pub fn GrQLi1<QLiPoints: TupleOrVec<i32>, BonusPoints: TupleOrVec<i32>>(qli_points: QLiPoints, bonus_points: BonusPoints) -> GammaExpr { GrQLi(1, &qli_points.as_vec(), &bonus_points.as_vec()) }
pub fn GrQLi2<QLiPoints: TupleOrVec<i32>, BonusPoints: TupleOrVec<i32>>(qli_points: QLiPoints, bonus_points: BonusPoints) -> GammaExpr { GrQLi(2, &qli_points.as_vec(), &bonus_points.as_vec()) }
pub fn GrQLi3<QLiPoints: TupleOrVec<i32>, BonusPoints: TupleOrVec<i32>>(qli_points: QLiPoints, bonus_points: BonusPoints) -> GammaExpr { GrQLi(3, &qli_points.as_vec(), &bonus_points.as_vec()) }
pub fn GrQLi4<QLiPoints: TupleOrVec<i32>, BonusPoints: TupleOrVec<i32>>(qli_points: QLiPoints, bonus_points: BonusPoints) -> GammaExpr { GrQLi(4, &qli_points.as_vec(), &bonus_points.as_vec()) }
pub fn GrQLi5<QLiPoints: TupleOrVec<i32>, BonusPoints: TupleOrVec<i32>>(qli_points: QLiPoints, bonus_points: BonusPoints) -> GammaExpr { GrQLi(5, &qli_points.as_vec(), &bonus_points.as_vec()) }
pub fn GrQLi6<QLiPoints: TupleOrVec<i32>, BonusPoints: TupleOrVec<i32>>(qli_points: QLiPoints, bonus_points: BonusPoints) -> GammaExpr { GrQLi(6, &qli_points.as_vec(), &bonus_points.as_vec()) }
pub fn GrQLi7<QLiPoints: TupleOrVec<i32>, BonusPoints: TupleOrVec<i32>>(qli_points: QLiPoints, bonus_points: BonusPoints) -> GammaExpr { GrQLi(7, &qli_points.as_vec(), &bonus_points.as_vec()) }
pub fn GrQLi8<QLiPoints: TupleOrVec<i32>, BonusPoints: TupleOrVec<i32>>(qli_points: QLiPoints, bonus_points: BonusPoints) -> GammaExpr { GrQLi(8, &qli_points.as_vec(), &bonus_points.as_vec()) }

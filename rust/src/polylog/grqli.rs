#![allow(non_snake_case)]

use itertools::Itertools;

use crate::base::{StringTools};
use crate::math::{to_x_vec, GammaExpr, pullback};
use crate::polylog::{QLi};


pub fn GrQLi(weight: i32, qli_points: &[i32], bonus_points: &[i32]) -> GammaExpr {
    pullback(
        QLi(weight, &to_x_vec(qli_points)),
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

#[macro_export] macro_rules! GrQLi1 { ( $( $x:expr ),* ; $( $y:expr ),* ) => { $crate::polylog::GrQLi(1, &[ $( $x, )* ], &[ $( $y, )* ]) } }
#[macro_export] macro_rules! GrQLi2 { ( $( $x:expr ),* ; $( $y:expr ),* ) => { $crate::polylog::GrQLi(2, &[ $( $x, )* ], &[ $( $y, )* ]) } }
#[macro_export] macro_rules! GrQLi3 { ( $( $x:expr ),* ; $( $y:expr ),* ) => { $crate::polylog::GrQLi(3, &[ $( $x, )* ], &[ $( $y, )* ]) } }
#[macro_export] macro_rules! GrQLi4 { ( $( $x:expr ),* ; $( $y:expr ),* ) => { $crate::polylog::GrQLi(4, &[ $( $x, )* ], &[ $( $y, )* ]) } }
#[macro_export] macro_rules! GrQLi5 { ( $( $x:expr ),* ; $( $y:expr ),* ) => { $crate::polylog::GrQLi(5, &[ $( $x, )* ], &[ $( $y, )* ]) } }
#[macro_export] macro_rules! GrQLi6 { ( $( $x:expr ),* ; $( $y:expr ),* ) => { $crate::polylog::GrQLi(6, &[ $( $x, )* ], &[ $( $y, )* ]) } }
#[macro_export] macro_rules! GrQLi7 { ( $( $x:expr ),* ; $( $y:expr ),* ) => { $crate::polylog::GrQLi(7, &[ $( $x, )* ], &[ $( $y, )* ]) } }
#[macro_export] macro_rules! GrQLi8 { ( $( $x:expr ),* ; $( $y:expr ),* ) => { $crate::polylog::GrQLi(8, &[ $( $x, )* ], &[ $( $y, )* ]) } }

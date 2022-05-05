#![allow(non_snake_case)]

use math_format_macro::math_format;
use crate::math::{X, XArg, DeltaExpr, cross_ratio_vec, neg_cross_ratio, tensor_product};


#[derive(Debug, Clone)]
struct Point {
    x: X,
    odd: bool,
}

fn qli_node_func(p: &[Point]) -> DeltaExpr {
    assert_eq!(p.len(), 4);
    if p[0].odd {
        neg_cross_ratio(p[0].x, p[1].x, p[2].x, p[3].x)
    } else {
        -neg_cross_ratio(p[1].x, p[2].x, p[3].x, p[0].x)
    }
}

fn QLi_impl(weight: i32, points: &[Point]) -> DeltaExpr {
    let num_points = points.len() as i32;
    if num_points < 4 || num_points % 2 != 0 {
        panic!("Bad number of QLi points: {}", num_points);
    }
    let min_weight = (num_points - 2) / 2;
    if weight < min_weight {
        panic!("Weight too low: {}", weight);
    }

    let subsums = || {
        let mut ret = DeltaExpr::zero();
        for i in 0 .. (num_points as usize) - 3 {
            let foundation = [&points[..i+1], &points[i+3..]].concat();
            ret += tensor_product(
                &qli_node_func(&points[i..i+4]),
                &QLi_impl(weight - 1, &foundation)
            );
        }
        ret
    };
    if weight == min_weight {
        if num_points == 4 {
            qli_node_func(points)
        } else {
            subsums()
        }
    } else {
        let cross_ratio_args: Vec<_> = points.iter().map(|p| p.x).collect();
        let mut ret = tensor_product(
            &cross_ratio_vec(&cross_ratio_args),
            &QLi_impl(weight - 1, points)
        );
        if num_points > 4 {
            ret += subsums();
        }
        ret
    }
}

pub fn QLi(weight: i32, points: &[impl XArg]) -> DeltaExpr {
    let tagged_points: Vec<Point> =
        points.iter().enumerate().map(|(i, p)| Point{ x: p.as_x(), odd: (i+1) % 2 == 1 }).collect();
    QLi_impl(weight, &tagged_points).annotate(
        math_format!(r"\op{QLi}_<>(<,>)", weight, points.to_vec())
    )
}

#[macro_export] macro_rules! QLi1 { ( $( $x:expr ),* ) => { $crate::polylog::QLi(1, &[ $( $crate::math::X::from($x), )* ]) } }
#[macro_export] macro_rules! QLi2 { ( $( $x:expr ),* ) => { $crate::polylog::QLi(2, &[ $( $crate::math::X::from($x), )* ]) } }
#[macro_export] macro_rules! QLi3 { ( $( $x:expr ),* ) => { $crate::polylog::QLi(3, &[ $( $crate::math::X::from($x), )* ]) } }
#[macro_export] macro_rules! QLi4 { ( $( $x:expr ),* ) => { $crate::polylog::QLi(4, &[ $( $crate::math::X::from($x), )* ]) } }
#[macro_export] macro_rules! QLi5 { ( $( $x:expr ),* ) => { $crate::polylog::QLi(5, &[ $( $crate::math::X::from($x), )* ]) } }
#[macro_export] macro_rules! QLi6 { ( $( $x:expr ),* ) => { $crate::polylog::QLi(6, &[ $( $crate::math::X::from($x), )* ]) } }
#[macro_export] macro_rules! QLi7 { ( $( $x:expr ),* ) => { $crate::polylog::QLi(7, &[ $( $crate::math::X::from($x), )* ]) } }
#[macro_export] macro_rules! QLi8 { ( $( $x:expr ),* ) => { $crate::polylog::QLi(8, &[ $( $crate::math::X::from($x), )* ]) } }
